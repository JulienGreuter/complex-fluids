#!/bin/bash

# Paramètres
NOM_BASE="test_reseau"  # Nom de base des fichiers
NB_TESTS=25              # Nombre de tests

# Répertoires de travail
SCRIPT_DIR=$(dirname "$(realpath "$0")")  # Chemin absolu du répertoire contenant le script
BASE_DIR=$(dirname "$SCRIPT_DIR")         # Répertoire parent (racine du projet)

# Dossiers de sortie (chemins absolus)
CSV_DIR="$BASE_DIR/figures/raw_data"
IMG_DIR="$BASE_DIR/figures/visualisations"
mkdir -p "$CSV_DIR" "$IMG_DIR"  # Créer les dossiers s'ils n'existent pas

# Générer les fichiers CSV
echo "Génération des réseaux..."
for i in $(seq 1 $NB_TESTS); do
    CSV_FILE="$CSV_DIR/${NOM_BASE}_${i}.csv"

    echo "Création de $CSV_FILE..."
    "$BASE_DIR/tests/export_reseau.exe" "$CSV_FILE"
done

# Générer les images correspondantes
echo "Génération des visualisations..."
for i in $(seq 1 $NB_TESTS); do
    CSV_FILE="$CSV_DIR/${NOM_BASE}_${i}.csv"
    IMG_FILE="$IMG_DIR/${NOM_BASE}_${i}.png"

    echo "Traitement de $CSV_FILE..."
    python "$BASE_DIR/scripts/plot_reseau2D.py" --input "$CSV_FILE" --output "$IMG_FILE"
done

echo "Tous les tests sont terminés !"