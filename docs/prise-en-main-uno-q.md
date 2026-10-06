# Prise en main de l'Arduino UNO Q

Synthèse personnelle écrite pendant le projet, d'après la documentation officielle d'Arduino.

## Outils de développement
**Arduino App Lab** est la plateforme par défaut de la carte : elle combine dans un même projet des sketchs Arduino classiques, des scripts Python et des applications Linux, avec des « Bricks » (briques de code prêtes à l'emploi) et des modèles d'IA préconfigurés. Après installation, la carte est détectée automatiquement quand l'animation du logo sur la matrice de LED s'arrête.

**Arduino IDE** : il faut installer le noyau « Arduino UNO Q Zephyr » (Outils, Carte, Gestionnaire de cartes). Limite : seul le microcontrôleur peut être programmé depuis l'IDE.

## Premiers essais
1. **Blink** : exemple « Blink LED » d'App Lab, puis RUN : la LED 3 clignote.
2. **LED du côté Linux** : depuis le terminal d'App Lab :
```bash
echo 1 | tee /sys/class/leds/red:user/brightness   # allumer
echo 0 | tee /sys/class/leds/red:user/brightness   # éteindre
```

## Communication entre le microprocesseur (Linux) et le microcontrôleur : le Bridge
La carte associe un processeur Linux (Python) et un microcontrôleur (Zephyr, C++). Le **Bridge** est un pont RPC entre les deux.

| Côté | Fonction | Rôle |
|---|---|---|
| MCU (C++) | `Bridge.begin()` | Initialise le pont, à appeler dans `setup()` |
| MCU (C++) | `Bridge.call(méthode, args...)` | Appelle une fonction Python et attend un résultat |
| MCU (C++) | `Bridge.notify(méthode, args...)` | Appel asynchrone sans réponse (messages > 256 octets ignorés) |
| MCU (C++) | `Bridge.provide(nom, fonction)` | Expose une fonction du MCU à Linux ; elle s'exécute dans le thread RPC prioritaire et doit être courte et thread-safe |
| MCU (C++) | `Bridge.provide_safe(nom, fonction)` | Idem, mais exécutée dans `loop()` : à préférer si la fonction utilise l'API Arduino (`digitalWrite`, `Serial`) |
| Linux (Python) | `Bridge.call(nom, *args)` | Appelle une fonction du MCU et bloque jusqu'au résultat (limite 256 octets) |
| Linux (Python) | `Bridge.notify(nom, *args)` | Appel sans attente de réponse |
| Linux (Python) | `Bridge.provide(nom, fonction)` | Expose une fonction Python au MCU |

**Piège à éviter :** ne pas appeler `Bridge.call()` ni `Monitor.print()` à l'intérieur d'une fonction enregistrée avec `Bridge.provide()` : lancer une communication pendant qu'on répond à une autre peut bloquer le système.

Dans ce projet, Python appelle `Bridge.call("ai_command", code)` et le firmware expose `ai_command` avec `Bridge.provide`.
