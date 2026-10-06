# Protocole de commandes avec le robot ROMI

Les messages sont du texte ASCII envoyé par liaison série (module HC-05, 38 400 bauds). Les commandes commencent par `*` et se terminent par `;`. Ce tableau est relevé dans le code (`sketch/sketch.ino`).

## Commandes envoyées au robot
| Message | Émetteur dans le code | Signification |
|---|---|---|
| `*G10;` | `ai_command("AV")` | Avancer |
| `*G0;` | `ai_command("FR")` | Arrêt |
| `*V<n>;` | `update_volant` | Angle de braquage, de −100 à +100 |
| `*C1;` / `*C2;` / `*C0;` | clignotants | Clignotant droit / gauche / éteint |
| `*K2;*B100;` puis `*B0;` | `alerte_sonore` | Commandes qui font envoyer des messages CAN au robot : alerte d'obstacle, puis fin d'alerte |
| `*L1;` | `tachePing` | Commande qui fait envoyer un message CAN au robot, émise au démarrage |
| `00060103` | `tachePing` | Requête d'identifiant CAN `0x60103`, sans données : demande des distances |

## Réponse des télémètres
Une ligne contenant `60103` suivie de trois valeurs hexadécimales : distances gauche, centre et droite. Une valeur à 0 est traitée comme la distance maximale (100), c'est-à-dire sans obstacle dans la portée. Le guide de dépannage du projet indique que la requête `60103 0` doit renvoyer les distances.

## Commandes internes (Python vers microcontrôleur)
| Code | Effet |
|---|---|
| `VG` | braquage −10 (limité à −100) |
| `VD` | braquage +10 (limité à +100) |
| `AV` | avancer |
| `FR` | arrêter |
