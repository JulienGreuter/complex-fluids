import argparse
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.cm as cm
import matplotlib.colors as mcolors

# Configuration d'argparse
parser = argparse.ArgumentParser(description="Visualisation d'un réseau à partir d'un fichier CSV")
parser.add_argument("--input", required=True, help="Nom du fichier CSV contenant le réseau")
parser.add_argument("--output", help="Nom du fichier image à sauvegarder (optionnel)", default=None)
args = parser.parse_args()

# Charger le fichier CSV
df = pd.read_csv(args.input)

# Définir la colormap et normaliser les couleurs en fonction de l'ordre de subdivision
ordre_min, ordre_max = df["ordre"].min(), df["ordre"].max()
norm = mcolors.Normalize(vmin=ordre_min, vmax=ordre_max)
cmap = cm.viridis  # Palette de couleurs

# Créer la figure et l'axe
fig, ax = plt.subplots(figsize=(8, 8))

# Tracer les cases avec des couleurs selon l'ordre de subdivision
for _, row in df.iterrows():
    x, z, taille, ordre = row["x"], row["z"], row["taille"], row["ordre"]
    color = cmap(norm(ordre))  # Couleur normalisée
    rect = plt.Rectangle((x - taille / 2, z - taille / 2), taille, taille, 
                         edgecolor="black", facecolor=color, linewidth=1)
    ax.add_patch(rect)

# Ajouter la colorbar associée à l'ordre de subdivision
sm = cm.ScalarMappable(cmap=cmap, norm=norm)
sm.set_array([])  # Nécessaire pour éviter l'erreur
cbar = plt.colorbar(sm, ax=ax, label="Ordre de subdivision")

# Définir les bornes du graphique
ax.set_xlim(df["x"].min() - 5, df["x"].max() + 5)
ax.set_ylim(df["z"].min() - 5, df["z"].max() + 5)
ax.set_aspect("equal")
ax.set_title("Visualisation du Réseau")
ax.set_xlabel("x")
ax.set_ylabel("z")
ax.grid(True)

# Sauvegarde ou affichage
if args.output:
    plt.savefig(args.output, dpi=300)
    print(f"Image sauvegardée sous {args.output}")
else:
    plt.show()