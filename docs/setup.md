# Installation

## Matériel
- Arduino UNO Q
- Robot ROMI avec module Bluetooth classique HC-05 relié aux broches série de la carte (À COMPLÉTER : schéma de câblage)
- Smartphone iOS ou Android avec l'app **Arduino IoT Remote**
- Réseau Wi-Fi commun à la carte et au smartphone

## Logiciel
1. Importer ce dossier dans **Arduino App Lab**.
2. Vérifier dans `app.yaml` que le modèle `ei-model-1019311-1` est disponible (À COMPLÉTER : comment le récupérer ou le régénérer sous Edge Impulse).
3. Lancer l'application : l'interface s'ouvre dans le navigateur, sinon à `<nom-de-la-carte>.local:7000`.

## Appairage du smartphone
1. Dans Arduino IoT Remote : Devices, puis « Stream phone camera to UNO Q ».
2. Scanner le QR code affiché dans l'interface.
3. Le flux vidéo apparaît dans l'interface.

## Appairage Bluetooth du robot
À COMPLÉTER : appairage du HC-05 (nom, code PIN, mode de communication), vitesse série 38 400 bauds.

## Vérification rapide
1. Faire le geste « Avancer » devant la caméra : le robot avance.
2. Faire « Freiner » : le robot s'arrête.
3. « Volant Gauche » : le curseur de l'interface se déplace et le clignotant gauche s'active après deux pas.
