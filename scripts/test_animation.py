import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import argparse
# Parser les arguments de la ligne de commande
parser = argparse.ArgumentParser(description="genere animation d'un ensemble de particules à partir d'un dossiers de fichiers csv particules_i.csv")
parser.add_argument('--input-folder', type=str, required=True, help="chemin du dossier")

args = parser.parse_args()


# Dossier contenant les fichiers CSV
folder = args.input

# Fonction pour charger les fichiers CSV
def load_positions(folder):
    files = [f for f in os.listdir(folder) if f.endswith('.csv') and f.startswith('positions_')]
    files.sort()  # On trie les fichiers pour qu'ils soient traités dans l'ordre chronologique
    positions = []

    for file in files:
        file_path = os.path.join(folder, file)
        data = np.loadtxt(file_path, delimiter=',', skiprows=1)  # Charger les données en ignorant la première ligne de commentaire
        positions.append(data[:, :2])  # Garder seulement les coordonnées x et z
    return positions

# Charger les positions des particules
positions = load_positions(folder)

# Déterminer les bornes de l'animation
x_min = min([pos[:, 0].min() for pos in positions])
x_max = max([pos[:, 0].max() for pos in positions])
z_min = min([pos[:, 1].min() for pos in positions])
z_max = max([pos[:, 1].max() for pos in positions])

# Créer la figure de l'animation
fig, ax = plt.subplots()
ax.set_xlim(x_min, x_max)
ax.set_ylim(z_min, z_max)

# Créer un scatter plot pour les particules
scatter = ax.scatter([], [], s=5)

# Fonction d'initialisation pour l'animation
def init():
    # Initialiser avec un tableau vide de forme correcte
    scatter.set_offsets(np.array([[], []]).T)  # np.array([[], []]).T pour obtenir une forme correcte
    return scatter,

# Fonction d'animation pour mettre à jour les particules à chaque frame
def update(frame):
    scatter.set_offsets(positions[frame])  # Mettre à jour les positions des particules
    return scatter,

# Créer l'animation
ani = FuncAnimation(fig, update, frames=len(positions), init_func=init, blit=True, interval=100)

# Sauvegarder l'animation en fichier .mp4 ou afficher à l'écran
ani.save(floder+'/animation.mp4', writer='ffmpeg', fps=30)  # ou vous pouvez utiliser plt.show() pour afficher l'animation

plt.show()  # Pour afficher l'animation à l'écran si vous ne voulez pas
