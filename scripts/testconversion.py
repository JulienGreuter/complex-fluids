import re
import csv
import argparse


# Configuration d'argparse
parser = argparse.ArgumentParser(description="Visualisation d'un réseau à partir d'un fluideComplexe")
parser.add_argument("--input", required=True, help="Nom du fichier les cases")

args = parser.parse_args()

# Charger le fichier 
with open(args.input, "r") as f:
    output_text = f.read()
# Regex pour extraire les informations des lignes "Case"
pattern = re.compile(r"Case \((-?\d+(\.\d+)?), (-?\d+(\.\d+)?)\), Taille: (\d+(\.\d+)?), Ordre: (\d+), Occupée")

# Liste pour stocker les données extraites
data = [("x", "z", "taille")]  # En-tête du CSV #, "ordre"

# Recherche et extraction des données
for match in pattern.finditer(output_text):
    x, _, z, _, taille, _, ordre = match.groups() #, _, ordre
    data.append((x, z, taille)) #, ordre

# Écriture dans un fichier CSV
csv_filename = "particules.csv"
with open(csv_filename, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerows(data)

print(f"Extraction terminée ! Les données ont été enregistrées dans '{csv_filename}'.")