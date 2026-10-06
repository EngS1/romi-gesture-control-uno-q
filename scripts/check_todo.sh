#!/usr/bin/env bash
# À lancer avant de publier : liste ce qui reste à compléter ou à confirmer.
grep -rnE "À COMPLÉTER|À CONFIRMER|À VÉRIFIER" --exclude-dir=.git --exclude-dir=assets --exclude=check_todo.sh . || echo "Rien à compléter."
