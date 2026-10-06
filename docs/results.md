# Jeu de données, modèle et comportement observé

## Jeu de données et modèle
- **4 classes** de gestes statiques de la main : un doigt levé (`Avancer`), poing fermé (`Freiner`), main ouverte (`Volant Droit`), deux doigts en V (`Volant Gauche`).
- **30 images par geste**, collectées et utilisées pour l'entraînement sous Edge Impulse par mon binôme, Orphée DJIKPE (méthodologie du projet : collecte d'images, définition des paramètres du modèle, entraînement, test et validation).
- Le modèle est ensuite téléchargé dans App Lab et exécuté **localement sur l'UNO Q** (détection d'objets sur le flux vidéo du smartphone, seuil de confiance réglable depuis l'interface, 0,5 par défaut).

## Démonstration
La [vidéo de démonstration](https://1drv.ms/f/c/f9c8d123e6cbe736/IgAnTCKELjs4Sq8H9bSpwonzAW95COt1dy-BWJ-6Cf7Aa8U?e=BeVqbb) montre la chaîne complète : geste, détection, commande Bluetooth, réaction du robot sur la piste d'essai.

## Comportement observé en essais
- **Latence vidéo** : sensible à la qualité du Wi-Fi ; un routeur 5 GHz stabilise le flux.
- **Appairage Bluetooth** : un seul ROMI allumé et non connecté à la fois, sinon la connexion est aléatoire.
- **Sécurité** : le robot s'arrête lorsque les télémètres détectent un obstacle à moins de 20 cm pendant qu'il avance.
- **Mises à jour d'App Lab** : après une mise à jour du firmware, le modèle pouvait ne plus correspondre ; un reflashage de la carte puis une réinstallation sans mise à jour du firmware rétablit le fonctionnement.

## Limites du système
- Gestes **statiques** uniquement, reconnus image par image ; le braquage évolue par pas de 10 à chaque commande confirmée.
- Jeu de données de 30 images par geste : la robustesse dépend de l'éclairage, de l'arrière-plan et de la personne devant la caméra.
