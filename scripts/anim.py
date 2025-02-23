import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import argparse

# Parser les arguments de la ligne de commande
parser = argparse.ArgumentParser(description="Génère une animation d'un ensemble de particules avec traînée à partir d'un dossier de fichiers CSV positions_i.csv")
parser.add_argument('--input-folder', type=str, required=True, help="Chemin du dossier contenant les fichiers CSV")
parser.add_argument('--trail-length', type=int, default=1, help="Nombre de frames affichées pour la traînée")
parser.add_argument('--fps', type=int, default=3, help="Images par seconde de l'animation")
parser.add_argument('--lines', type=int, default=None, help="Nombre de lignes à lire par fichier CSV (par défaut toutes les lignes)")
args = parser.parse_args()

folder = args.input_folder
trail_length = args.trail_length
fps = args.fps

# Fonction pour charger les fichiers CSV
def load_positions(folder, num_lines=None):
    files = [f for f in os.listdir(folder) if f.endswith('.csv') and f.startswith('positions_') and f != 'positions_ini.csv']
    files.sort(key=lambda f: int(''.join(filter(str.isdigit, f))))  # Tri basé sur le numéro du fichier
    positions = []

    for file in files:
        file_path = os.path.join(folder, file)
        
        # Lire un nombre limité de lignes si num_lines est spécifié, sinon lire tout le fichier
        if num_lines is not None:
            data = np.loadtxt(file_path, delimiter=',', skiprows=1, max_rows=num_lines)  # Limiter le nombre de lignes lues
        else:
            data = np.loadtxt(file_path, delimiter=',', skiprows=1)  # Lire tout le fichier
        
        positions.append(data[:, :2])  # Garder seulement les coordonnées x et z
    return positions

positions = load_positions(folder, num_lines=args.lines)

# Déterminer les bornes de l'animation
x_min = min([pos[:, 0].min() for pos in positions])
x_max = max([pos[:, 0].max() for pos in positions])
z_min = min([pos[:, 1].min() for pos in positions])
z_max = max([pos[:, 1].max() for pos in positions])

# Créer la figure
fig, ax = plt.subplots()
ax.set_xlim(x_min, x_max)
ax.set_ylim(z_min, z_max)
scatters = [ax.scatter([], [], s=5, color = 'gray', alpha=0.3) for i in range(trail_length)]

# Fonction d'initialisation
def init():
    for scatter in scatters:
        scatter.set_offsets(np.empty((0, 2)))  # Utiliser un tableau vide 2D avec 0 lignes et 2 colonnes
    return scatters

# Fonction d'animation
def update(frame):
    for i in range(trail_length):
        if frame - i >= 0:
            scatters[i].set_offsets(positions[frame - i])
    return scatters

# Création de l'animation
ani = FuncAnimation(fig, update, frames=len(positions), init_func=init, blit=True, interval=1000//fps)

# Sauvegarde ou affichage
ani.save(os.path.join(folder, 'animation.mp4'), writer='ffmpeg', fps=fps)
plt.show()