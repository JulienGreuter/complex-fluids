#!/bin/bash

# Vérification du bon nombre d'arguments
if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <type> <start> <end>"
  exit 1
fi

# Assignation des variables
TYPE=$1
START=$2
END=$3

# Création du dossier cible
TARGET_DIR="${START}_${END}"
mkdir -p "$TARGET_DIR"

# Copie des fichiers dans la plage donnée
for i in $(seq $START $END); do
  FILENAME="${TYPE}_${i}.csv"
  if [ -f "$FILENAME" ]; then
    cp "$FILENAME" "$TARGET_DIR/"
    
  else
    echo "Fichier non trouvé: $FILENAME"
  fi
done

echo "Fichiers ${TYPE}_${START} à ${TYPE}_${END} copiés dans $TARGET_DIR/"