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
parser.add_argument('--skip-frames', type=int, default=1, help="Sauter un fichier sur N pour accélérer")
parser.add_argument('--lines', type=int, default=None, help="Nombre de lignes à lire par fichier CSV (par défaut toutes les lignes)")
parser.add_argument('--x-min', type=float, default=None, help="Coordonnée x minimale de la boîte de filtrage")
parser.add_argument('--x-max', type=float, default=None, help="Coordonnée x maximale de la boîte de filtrage")
parser.add_argument('--z-min', type=float, default=None, help="Coordonnée z minimale de la boîte de filtrage")
parser.add_argument('--z-max', type=float, default=None, help="Coordonnée z maximale de la boîte de filtrage")
args = parser.parse_args()

folder = args.input_folder
trail_length = args.trail_length
fps = args.fps


# Fonction pour charger les fichiers CSV
def load_positions(folder, num_lines=None, x_min=None, x_max=None, z_min=None, z_max=None):
    files = [f for f in os.listdir(folder) if f.endswith('.csv') and f.startswith('positions_') and f != 'positions_ini.csv']
    files.sort(key=lambda f: int(''.join(filter(str.isdigit, f))))  # Tri basé sur le numéro du fichier
    
    files = files[::args.skip_frames]  # Lire un fichier sur N
    positions = []

    for file in files:
        file_path = os.path.join(folder, file)
        
        # Lire un nombre limité de lignes si num_lines est spécifié, sinon lire tout le fichier
        if num_lines is not None:
            data = np.loadtxt(file_path, delimiter=',', skiprows=1, max_rows=num_lines)
        else:
            data = np.loadtxt(file_path, delimiter=',', skiprows=1)

        # Sélectionner les points dans la boîte définie
        if x_min is not None and x_max is not None and z_min is not None and z_max is not None:
            mask = (data[:, 0] >= x_min) & (data[:, 0] <= x_max) & (data[:, 1] >= z_min) & (data[:, 1] <= z_max)
            data = data[mask]

        positions.append(data[:, :2])  # Garder seulement x et z
    return positions

# Charger avec filtrage
positions = load_positions(folder, num_lines=args.lines, x_min=args.x_min, x_max=args.x_max, z_min=args.z_min, z_max=args.z_max)


# Déterminer les bornes de l'animation
if args.x_min is not None and args.x_max is not None and args.z_min is not None and args.z_max is not None:
    x_min=args.x_min
    x_max=args.x_max 
    z_min=args.z_min 
    z_max=args.z_max
else :
    x_min = min([pos[:, 0].min() for pos in positions])
    x_max = max([pos[:, 0].max() for pos in positions])
    z_min = min([pos[:, 1].min() for pos in positions])
    z_max = max([pos[:, 1].max() for pos in positions])

# Créer la figure
fig, ax = plt.subplots()
ax.set_xlim(x_min, x_max)
ax.set_ylim(z_min, z_max)
scatters = [ax.scatter([], [], s=50, color = 'gray', alpha=0.3) for i in range(trail_length)]

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
ani = FuncAnimation(fig, update, frames=len(positions), init_func=init, blit=False, interval=1000//fps)

# Sauvegarde ou affichage
ani.save(os.path.join(folder, 'animation.mp4'), writer='ffmpeg', fps=fps)
plt.show()