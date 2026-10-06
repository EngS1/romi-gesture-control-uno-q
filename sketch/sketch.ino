// SPDX-FileCopyrightText: Copyright (C) 2026 Daouda SYLLA
//
// SPDX-License-Identifier: MPL-2.0

#include <zephyr/kernel.h>
#include "Arduino_RouterBridge.h"

// Préparation de la mémoire pour nos tâches RTOS
#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(cligno_gauche_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(cligno_droite_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(update_volant_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(alerte_sonore_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(ping_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(ecoute_stack, STACK_SIZE);

K_MUTEX_DEFINE(bluetooth_mutex);

bool immobiliser = false, avancer = false, freiner = false, arret_obstacle = false;
unsigned int dist_count = 0;
unsigned int dist_droite,dist_gauche;
unsigned int global_dist_droite = 100,global_dist_gauche = 100;

struct k_thread thread_cligno_gauche, thread_cligno_droite, thread_update_volant,thread_ecoute, thread_ping, thread_alerte_sonore;

// cligno variables
byte left_cligno_state, right_cligno_state;

//variable pour l'angle de braquage du volant
int volant = 0, volant0 = 0;
char s[20];

// Fonction générique pour envoyer des messages à ROMI
void sendMessageToRomi(String message){
  k_mutex_lock(&bluetooth_mutex, K_FOREVER);
  Serial.println(message);
  k_mutex_unlock(&bluetooth_mutex);
}


// --- Tâches Cligno ---
void cligno_gauche(void *d1, void *d2, void *d3) {
  while(1){
    if(volant < -19){
      left_cligno_state = !left_cligno_state;
      sendMessageToRomi(left_cligno_state?"*C2;": "*C0;");
    }else if (left_cligno_state){
      left_cligno_state = 0;
      sendMessageToRomi("*C0;");
      
    }
      k_msleep(500);
  }
}

void cligno_droite(void *d1, void *d2, void *d3) {
  while(1){
    if(volant > 19){
      right_cligno_state = !right_cligno_state;
      sendMessageToRomi(right_cligno_state?"*C1;": "*C0;");
    }else if (right_cligno_state){
      right_cligno_state = 0;;
      sendMessageToRomi("*C0;");
    }
    k_msleep(500);
  }
}

//Alerte Sonore
void alerte_sonore(void *d1, void *d2, void *d3) {
  while(1){
    if((global_dist_gauche < 20 || global_dist_droite < 20) && avancer){
      arret_obstacle = true;
      //Serial.println("Alerte");
      sendMessageToRomi("*K2;*B100;");
      
    }else{
      if(arret_obstacle){
        sendMessageToRomi("*B0;");
        arret_obstacle = false;
      }
    }
    k_msleep(200);
  }
}

//Contrôle de distance
// Requête (Ping) des télémètres
// ==========================================================
void tachePing(void *d1, void *d2, void *d3) {
  k_msleep(3000); // Laisse le temps au système de démarrer
  sendMessageToRomi("*L1;");
  while (1) {
    // // Envoi de l'identificateur 0x60103 sans données (DLC=0)
    sendMessageToRomi("00060103");
    // On demande la distance 10 fois par seconde (100 ms)
    k_msleep(100); 
  }
}

void tacheEcoute(void *d1, void *d2, void *d3) {
  char buffer[64];
  int index = 0;
  while (1) {
    // Si des données arrivent sur le Bluetooth
    while (Serial.available()) {
      char c = Serial.read();
      
      // Fin de ligne détectée : on analyse le message complet
      if (c == '\n' || c == '\r') {
        if (index > 0) {
          buffer[index] = '\0'; // Termine la chaîne de caractères
          
          // Vérifie si le message contient notre réponse radar "60103"
          if (strstr(buffer, "60103") != NULL) {
            
            // On cherche le premier espace (qui sépare l'ID des données)
            char *ptr = strchr(buffer, ' '); 
            if (ptr != NULL) {
              unsigned int dist_g, dist_c, dist_d;
              // On convertit les 3 valeurs Hexadécimales en nombres entiers
              int count = sscanf(ptr, "%x %x %x", &dist_g, &dist_c, &dist_d);

              if(dist_g == 0 ){
                dist_gauche += 100;
              }else{
                dist_gauche += dist_g;
              }
              if(dist_d == 0){
              dist_droite += 100;
              }else{
                dist_droite += dist_d;
              }
              dist_count++;
              if(dist_count == 5){
                dist_droite /= 5;
                dist_gauche /= 5;
                dist_count = 0;
                global_dist_droite= dist_droite;
                global_dist_gauche = dist_gauche;
                dist_droite = 0;
                dist_gauche = 0;
              }
              
            }
          } 
          index = 0; // Réinitialise le buffer pour le message suivant
        }
      } 
      else {
        // Stocke le caractère dans le buffer
        if (index < 63) {
          buffer[index++] = c;
        }
      }
    }
    k_msleep(75); // Petite pause pour laisser le buffer Bluetooth se remplir
  }
}

//MAJ volant
void update_volant(void *d1, void *d2, void *d3) {
  while(1){
    if(volant != volant0){
      sprintf(s,"*V%d;", volant);
      sendMessageToRomi(s);
      volant0 = volant;
    }
    k_msleep(100);
  }
}

// --- Fonction appelée par le code Python via le Bridge ---
void ai_command(String command){
  if(command == "VG" && volant != -100){
    volant -=10;
  } else if (command == "VD" && volant != 100){
    volant +=10;
  }else if (command == "AV"){
        avancer = true;
        sendMessageToRomi("*G10;");
  }else if (command == "FR"){
        avancer = false;
        sendMessageToRomi("*G0;");
  }else{
    //Données non reconnues;
  }
}

void setup() {

  // 2. Initialisation du pont de communication RPC
  Bridge.begin();
  Bridge.provide("ai_command", ai_command);
  
  // Initialise la communication avec le HC-05 sur les broches 0 et 1 (Serial ou Serial1 selon la version d'APPLab)
  // La vitesse par défaut d'un HC-05 en mode communication est souvent 38400 bauds
  Serial.begin(38400);
  while (!Serial) {
    ; // Attend que le port USB soit prêt (spécifique aux cartes modernes)
  } 
  
  // Initializing cligno variables
  left_cligno_state = 0;
  right_cligno_state = 0;

  // 3. Lancement des tâches indépendantes Zephyr
  k_thread_create(&thread_cligno_gauche, cligno_gauche_stack, K_THREAD_STACK_SIZEOF(cligno_gauche_stack),cligno_gauche, NULL, NULL, NULL, 7, 0, K_NO_WAIT);
  k_thread_create(&thread_cligno_droite, cligno_droite_stack, K_THREAD_STACK_SIZEOF(cligno_droite_stack),cligno_droite, NULL, NULL, NULL, 7, 0, K_NO_WAIT);
  k_thread_create(&thread_update_volant, update_volant_stack, K_THREAD_STACK_SIZEOF(update_volant_stack),update_volant, NULL, NULL, NULL, 7, 0, K_NO_WAIT);
  k_thread_create(&thread_ping, ping_stack, K_THREAD_STACK_SIZEOF(ping_stack),tachePing, NULL, NULL, NULL, 6, 0, K_NO_WAIT);              
  k_thread_create(&thread_ecoute, ecoute_stack, K_THREAD_STACK_SIZEOF(ecoute_stack), tacheEcoute, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
  k_thread_create(&thread_alerte_sonore, alerte_sonore_stack, K_THREAD_STACK_SIZEOF(alerte_sonore_stack),alerte_sonore, NULL, NULL, NULL, 5, 0, K_NO_WAIT);

}
 

void loop() {
}