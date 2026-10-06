# Installation et mise en route

## Matériel
- Arduino UNO Q
- Robot ROMI (module HC-05 intégré, Bluetooth classique)
- Un module **HC-05 supplémentaire**, configuré en **maître**, relié à la carte
- Smartphone ou tablette avec l'app **Arduino IoT Remote**
- Réseau Wi-Fi commun à la carte et au smartphone (un routeur 5 GHz est recommandé)

## Câblage du module HC-05 sur l'UNO Q
| Module HC-05 | Arduino UNO Q |
|---|---|
| VCC | 5 V |
| GND | GND |
| TX | RX (D0) |
| RX | TX (D1) |

Appairer le module au bon robot (code par défaut : `1234`) **avant** de lancer le projet dans App Lab. Liaison série à 38 400 bauds. Selon la version d'App Lab, il peut falloir utiliser `Serial` ou `Serial1` dans le code.

## Mise en route pas à pas
1. Allumer **un seul** ROMI, non connecté à autre chose (sinon la connexion est aléatoire). Allumer la carte UNO Q et brancher le HC-05. Sa LED passe d'un clignotement rapide à lent quand la connexion est établie, et celle du ROMI devient fixe.
2. Lancer App Lab, importer le projet de test « Bluetooth Com Test » et le lancer. Les feux de détresse du robot s'activent quand la communication fonctionne.
3. Importer le projet « Control ROMI with Smartphone Camera » (ce dépôt). Vérifier que tu es connecté à ton compte Arduino et à un réseau Wi-Fi.
4. Dans la brique **Video Object Detection**, onglet « AI models », cliquer sur **Train new AI model**, puis se connecter à Edge Impulse avec le même compte Arduino.
5. Dans ton espace personnel Edge Impulse, ouvrir le projet de détection de gestes du ROMI (modèle « Hand Gesture Detection for ROMI », 4 classes), puis lancer « Retrain model » (jusqu'à 30 minutes ou plus).
6. Retourner dans App Lab, onglet « AI models » : le modèle cloné est prêt à être téléchargé. Le télécharger et le sélectionner comme modèle du projet.
7. Lancer le projet : une page web s'ouvre avec un QR code (sinon `<nom-de-la-carte>.local:7000`).
8. Sur le smartphone, installer **IoT Remote**, se connecter au même réseau que la carte, scanner le QR code (ou saisir le code numérique affiché dessous), puis se connecter à la carte.
9. Poser le robot sur la piste d'essai et le smartphone sur un support pour qu'il voie les mains.

## Dépannage
| Problème | Cause probable | Solution |
|---|---|---|
| Robot immobile | Port série mal défini | Tester `Serial` ou `Serial1` |
| Latence vidéo | Wi-Fi instable | Utiliser un routeur 5 GHz |
| Le robot s'arrête brutalement | Obstacle détecté | Vérifier le bus CAN : la requête `60103 0` doit renvoyer les distances |
| « Undefined reference to setup » | Erreur de structure | Vérifier que le fichier principal contient `void setup()` et `void loop()` |
| Impossible de connecter le smartphone par QR code | Réseau | Même Wi-Fi que la carte, ou saisir le code numérique sous le QR code |
| Les formes détectées ne correspondent pas | Mauvais modèle ou mise à jour | Cocher « Hand Gesture Detection for ROMI » dans « AI models » ; sinon reflasher la carte (suppression des données) et recommencer sans mettre à jour le firmware |
