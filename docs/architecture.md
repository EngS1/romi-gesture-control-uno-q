# Architecture

## Deux « moitiés » sur une seule carte
L'Arduino UNO Q associe un processeur Linux (Python) et un microcontrôleur temps réel (Zephyr). Les deux communiquent par un **pont RPC** (`Arduino_RouterBridge`).

| Côté | Composant | Rôle | Fichier |
|---|---|---|---|
| Linux (Python) | Microprocesseur Qualcomm Dragonwing QRB2210 (Arm Cortex 64 bits, 4 cœurs) | Reçoit le flux du smartphone, exécute la détection, sert l'interface web, traduit les détections en commandes | `python/main.py` |
| Microcontrôleur (Zephyr, C++) | STM32U585 (Arm Cortex 32 bits) | Dialogue avec le robot en Bluetooth, tâches temps réel | `sketch/sketch.ino` |

## Flux de données
1. Le smartphone (app Arduino IoT Remote) envoie la vidéo à la carte en WebSocket chiffré, appairé par un code secret à 6 chiffres affiché dans un QR code.
2. La brique `video_objectdetection` applique le modèle (`ei-model-1019311-1`, voir `app.yaml`) avec un seuil de confiance réglable depuis l'interface (0,5 par défaut).
3. `send_message_to_romi` convertit l'étiquette détectée en code (`VG`, `VD`, `AV`, `FR`).
4. Anti-rebond : un nouveau code est envoyé immédiatement ; le même code répété n'est renvoyé qu'une fois toutes les 5 détections.
5. Le code est transmis au microcontrôleur par `Bridge.call("ai_command", code)`.
6. `ai_command` met à jour l'état (braquage, marche) et envoie les commandes au ROMI par la liaison série reliée au module HC-05 (38 400 bauds).

## Tâches temps réel (Zephyr)
Sur Zephyr, un numéro de priorité plus petit signifie une priorité plus haute.

| Tâche | Période | Priorité | Rôle |
|---|---|---|---|
| `tacheEcoute` | 75 ms | 5 | Lit la liaison série, reconstitue les lignes, décode la réponse des télémètres (id `60103`, trois valeurs hexadécimales : gauche, centre, droite) et moyenne 5 mesures |
| `alerte_sonore` | 200 ms | 5 | Si une distance moyenne est < 20 cm et que le robot avance : envoie `*K2;*B100;` (alerte d'obstacle, le robot s'arrête) ; envoie `*B0;` quand l'obstacle disparaît |
| `tachePing` | 100 ms | 6 | Demande les distances (trame `00060103`) ; envoie `*L1;` au démarrage après 3 s |
| `update_volant` | 100 ms | 7 | Envoie `*V<angle>;` quand l'angle change |
| `cligno_gauche` | 500 ms | 7 | Fait clignoter à gauche si braquage < −19 |
| `cligno_droite` | 500 ms | 7 | Fait clignoter à droite si braquage > 19 |

Un mutex (`bluetooth_mutex`) sérialise les écritures sur la liaison Bluetooth pour que les messages de plusieurs tâches ne s'entremêlent pas. Chaque tâche a une pile de 1 Ko.

La distance est rafraîchie environ toutes les 0,5 s (5 mesures moyennées à 100 ms d'intervalle) : c'est le compromis entre stabilité de la mesure et réactivité de l'alerte.

## Choix de conception et alternatives écartées
- **Bluetooth classique via un HC-05** : le module Bluetooth natif de l'UNO Q fonctionne en BLE, incompatible avec le HC-05 du ROMI. Un module HC-05 supplémentaire, configuré en **maître** et piloté en série par la carte, fait le lien.
- **Traitement local (Edge AI)** : la détection s'exécute sur la carte, ce qui limite la consommation et les ressources externes.
- **Trois rôles de tâches** (documentation du projet) : sécurité (haute priorité : lecture du bus CAN et télémètres), pilotage (priorité moyenne : traduction des ordres en commandes du robot), périphériques (basse priorité : clignotants et signaux sonores).
- **Quatre gestes statiques**, un par commande (avancer, freiner, braquer à gauche, braquer à droite), faciles à distinguer pour un modèle de détection d'objets.
