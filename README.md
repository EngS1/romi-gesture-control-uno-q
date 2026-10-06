# Pilotage gestuel d'un robot ROMI avec l'Arduino UNO Q (Edge AI)

> Un robot mobile **ROMI** piloté par gestes : la caméra d'un smartphone envoie son flux à une carte **Arduino UNO Q** qui exécute un modèle de détection **en local** ; les gestes reconnus sont traduits en commandes envoyées au robot en **Bluetooth classique (HC-05)**, par un firmware **Zephyr RTOS** multitâche.

**Statut :** prototype fonctionnel · **Cadre :** Projet 2A 2025-2026, ENSISA (Université de Haute-Alsace), encadré par Didier BRESCH, réalisé en binôme avec Orphée DJIKPE · **Vidéo :** [Voir la démonstration](https://1drv.ms/f/c/f9c8d123e6cbe736/IgAnTCKELjs4Sq8H9bSpwonzAW95COt1dy-BWJ-6Cf7Aa8U?e=BeVqbb)

## Ce que fait le projet

Reconnaissance de **gestes statiques de la main** (4 classes) :

| Étiquette du modèle | Geste | Commande | Effet sur le robot |
|---|---|---|---|
| `Avancer` | un doigt levé | `AV` | le robot avance |
| `Freiner` | poing fermé | `FR` | le robot s'arrête |
| `Volant Droit` | main ouverte | `VD` | braquage +10 (max +100), clignotant droit au-delà de +19 |
| `Volant Gauche` | signe V (deux doigts) | `VG` | braquage −10 (min −100), clignotant gauche au-delà de −19 |

Objectifs du projet (poster) : prise en main de l'Arduino UNO Q et d'App Lab, test de ses performances (ports, consommation, puissance de calcul, IA) et pilotage du ROMI par Bluetooth avec une caméra connectée et une IA de reconnaissance gestuelle. Approche « low-tech » : traitement local, matériel simple et réutilisable, carte à environ 45 €.

En parallèle, le firmware interroge les télémètres du robot (10 fois par seconde) et déclenche l'alerte d'obstacle du robot (commande `*K2;*B100;`, arrêt du ROMI) dès qu'un obstacle est détecté à moins de **20 cm** pendant qu'il avance. Une interface web affiche le flux, les détections récentes, le seuil de confiance et la position du volant.

## Architecture

```mermaid
flowchart LR
    P["Smartphone<br/>app Arduino IoT Remote"] -- "flux vidéo (WebSocket chiffré)" --> L["UNO Q, côté Linux<br/>Python, détection d'objets"]
    L -- "appel RPC ai_command" --> M["UNO Q, côté microcontrôleur<br/>Zephyr RTOS, C++"]
    M -- "Bluetooth classique HC-05" --> R["Robot ROMI"]
    R -- "distances (trame CAN 0x60103)" --> M
    L -- "interface web" --> W["Navigateur<br/>flux, détections, volant"]
```

Détail des tâches temps réel et du protocole : [`docs/architecture.md`](docs/architecture.md) et [`docs/protocol.md`](docs/protocol.md).

## Poster du projet

![Poster du projet 2A](docs/images/poster.jpg)

## Ce qui est de moi, ce qui vient d'Arduino

Ce projet a été réalisé **en binôme** (avec Orphée DJIKPE). Ce dépôt part de l'exemple officiel **« Detect Hands on Smartphone Camera »** d'Arduino App Lab (licence MPL-2.0), dont il reprend l'interface web et le pipeline de détection. **Ma contribution** (tout sauf l'entraînement du modèle) :

- **Firmware Zephyr** (`sketch/sketch.ino`) : 6 tâches temps réel (clignotants, braquage, interrogation des télémètres, écoute et décodage des trames, alerte sonore), mutex sur la liaison Bluetooth, protocole de commandes du ROMI.
- **Logique de pilotage** (`python/main.py`, fonction `send_message_to_romi`) : traduction des détections en commandes, anti-rebond par répétition, suivi de l'angle de volant.
- **Interface** : curseur « Volant » synchronisé avec la commande réelle.
- **Modèle de détection** : réalisé par mon binôme **Orphée DJIKPE** sous **Edge Impulse** (jeu de **30 images par geste**, 4 classes). Je l'ai intégré à la carte et au pilotage du robot.
- **Intégration et essais** sur le robot ROMI (liaison Bluetooth, commandes CAN, télémètres).

Détail des attributions et licences tierces : [`NOTICE.md`](NOTICE.md).

## Démonstration

La [vidéo de démonstration](https://1drv.ms/f/c/f9c8d123e6cbe736/IgAnTCKELjs4Sq8H9bSpwonzAW95COt1dy-BWJ-6Cf7Aa8U?e=BeVqbb) montre le système en fonctionnement : gestes devant la caméra du smartphone, détection sur la carte UNO Q et réaction du robot ROMI sur la piste d'essai.

Jeu de données, modèle et comportement observé : [`docs/results.md`](docs/results.md).

## Structure du dépôt

```
.
├── app.yaml            Description de l'application App Lab (briques, modèle)
├── python/main.py      Côté Linux : détection → commandes
├── sketch/             Côté microcontrôleur : firmware Zephyr (C++)
├── assets/             Interface web (HTML/JS/CSS)
├── docs/               Architecture, protocole, installation, prise en main de l'UNO Q, résultats
└── .github/workflows/  Intégration continue
```

La racine reste celle d'une application App Lab pour pouvoir être importée telle quelle.

## Installation

Voir [`docs/setup.md`](docs/setup.md). En bref : importer le dossier dans Arduino App Lab, lancer l'application, scanner le QR code avec l'app Arduino IoT Remote, appairer le module HC-05 avec le robot.

## Difficultés rencontrées

- **Bluetooth** : le Bluetooth natif de l'UNO Q est en basse consommation (BLE), alors que le module HC-05 du ROMI est en Bluetooth classique. Il a fallu ajouter un module HC-05 configuré en maître et commandé par liaison série.
- **Connexion aléatoire** si plusieurs ROMI sont allumés en même temps : un seul robot allumé et non connecté pendant l'appairage.
- **Latence vidéo** avec un Wi-Fi instable : un routeur 5 GHz règle le problème.
- **Mises à jour** : après une mise à jour, les formes détectées ne correspondaient plus à l'application ; il a fallu reflasher la carte (suppression des données) puis refaire l'installation sans mettre à jour le firmware.
- **Port série** : selon la version d'App Lab, il faut utiliser `Serial` ou `Serial1`.

## Évolutions possibles

- Protéger par mutex ou file de messages les variables partagées entre tâches (`volant`, `avancer`, distances).
- Élargir le jeu de données (30 images par geste aujourd'hui) : plus de personnes, d'éclairages et d'arrière-plans pour gagner en robustesse.
- Extraire la logique de traduction détection → commande pour la tester sans carte.

## Auteur

**Daouda SYLLA** — élève-ingénieur Automatique et Systèmes Embarqués, ENSISA
[GitHub](https://github.com/EngS1) · [LinkedIn](https://linkedin.com/in/sylla-daouda)

En binôme avec **Orphée DJIKPE**. Encadrant : **Didier BRESCH** (ENSISA, Université de Haute-Alsace).

## Licence

[MPL-2.0](LICENSE), comme l'exemple Arduino dont ce projet est dérivé. Voir [`NOTICE.md`](NOTICE.md).
