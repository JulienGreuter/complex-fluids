import argparse
import numpy as np
import os
import pandas as pd
from scipy.interpolate import griddata
from scipy.ndimage import uniform_filter
import matplotlib.pyplot as plt
from matplotlib.colors import TwoSlopeNorm
from scipy.ndimage import convolve
from scipy.stats import binned_statistic_2d
import matplotlib.animation as animation
import multiprocessing
import matplotlib
matplotlib.use('Agg')  # Utilise le backend non interactif pour plus de rapidité


def pad_with_periodicity(coords, neighbors, grid_size):
    """Ajoute un padding périodique aux coordonnées x et z."""
    padding = neighbors  # Taille du padding
    # On étend les coordonnées en ajoutant un padding à gauche et à droite (périodique)
    coords_padded = np.pad(coords, pad_width=padding, mode='wrap')
    return coords_padded

def compute_local_sum(grid, grid_size, neighbors):
    """Calcule la somme des valeurs contenues dans une surface autour de chaque point,
    en prenant en compte la périodicité."""
    
    # Taille du noyau de convolution (carré de (2*neighbors + 1) x (2*neighbors + 1))
    kernel_size = 2 * neighbors + 1
    kernel = np.ones((kernel_size, kernel_size))  # Noyau rempli de 1
    area = (kernel_size/grid_size)**2
    # Convolution périodique pour sommer les valeurs locales
    local_sum = convolve(grid, kernel, mode='wrap')  # 'wrap' assure la périodicité
    # Division par la surface considérée
    local_pressure = local_sum / area

    return local_pressure  # On retourne la pression locale

def sum_on_meshgrid_scipy(data, quantity, grid_size):
    """Calcule la somme des valeurs dans chaque case du maillage régulier."""
    
    # Définition des bords des cases (bins) → Doit être de taille `grid_size+1`
    x_bins = np.linspace(-0.5, 0.5, grid_size + 1)  
    z_bins = np.linspace(-0.5, 0.5, grid_size + 1)

    # Calcul de la somme des valeurs dans chaque case
    sum_grid, _, _, _ = binned_statistic_2d(
        data["x"], data["z"], data[quantity], 
        statistic='sum', bins=[x_bins, z_bins]
    )

    # Re-créer un meshgrid de même taille que sum_grid (grid_size x grid_size)
    mesh_x, mesh_z = np.meshgrid(
        (x_bins[:-1] + x_bins[1:]) / 2,  # Centres des cases en x
        (z_bins[:-1] + z_bins[1:]) / 2   # Centres des cases en z
    )

    return mesh_x, mesh_z, sum_grid

def compute_meshgrid(data, quantity, grid_size, neighbors):
    mesh_x, mesh_z, sum_grid = sum_on_meshgrid_scipy(data, quantity, grid_size)
    local_pressure = compute_local_sum(sum_grid, grid_size, neighbors)
    return mesh_x, mesh_z, local_pressure

def compute_average_on_grid(files,folder, grid_size, neighbors):
    """Calculer la moyenne des valeurs sur le meshgrid pour tous les fichiers."""
    
    # Création du meshgrid pour la grille globale
    x_vals = np.linspace(-0.5, 0.5, grid_size)
    z_vals = np.linspace(-0.5, 0.5, grid_size)
    mesh_x, mesh_z = np.meshgrid(x_vals, z_vals)

    # Initialisation des sommes des quantités
    sum_P_N = np.zeros_like(mesh_x)
    sum_P_T = np.zeros_like(mesh_x)
    count = 0

    # Processus pour chaque fichier
    for filename in files:
        filepath = os.path.join(folder, filename)
        print(f"Traitement de {filename}...")

        # Chargement des données du fichier
        data = pd.read_csv(filepath)

        # Interpolation des valeurs sur le meshgrid global
        _,_,grid_N = sum_on_meshgrid_scipy(data, "mvz2", grid_size)
        _,_,grid_T = sum_on_meshgrid_scipy(data, "mvx2", grid_size)

        # Mise à jour des sommes
        sum_P_N += grid_N
        sum_P_T += grid_T

        count += 1

    # Calcul de la moyenne sur tous les fichiers
    P_N = compute_local_sum(sum_P_N / count, grid_size, neighbors)
    P_T = compute_local_sum(sum_P_T / count, grid_size, neighbors)
    
    return mesh_x, mesh_z, P_N, P_T

def plot_meshgrid(mesh_x, mesh_z, grid, title, log, save, filename):
    """Affiche le meshgrid avec une colormap qui distingue négatif et positif."""
    plt.figure(figsize=(6, 5))

    if log:
        vmin, vmax = grid.min(), grid.max()
        vcenter = 0
        print(f"vmin: {vmin}, vmax: {vmax}, vcenter: {vcenter}")

        # Si toutes les valeurs sont identiques, ne pas utiliser TwoSlopeNorm
        if vmin == vmax:
            norm = None  
        else:
            if vmin >= vcenter:  # S'assure que vmin < vcenter
                vmin = vcenter - (vmax - vcenter) / 2  # Décale vmin vers les négatifs
            norm = TwoSlopeNorm(vmin=vmin, vcenter=vcenter, vmax=vmax)

        plt.pcolormesh(mesh_x, mesh_z, grid, shading='auto', cmap='coolwarm', norm=norm)

    else:
        plt.pcolormesh(mesh_x, mesh_z, grid, shading='auto', cmap='inferno')

    plt.colorbar(label=title)
    plt.xlabel("x")
    plt.ylabel("z")
    plt.title(title)
    if save:
        plt.savefig(filename, dpi=300)
    

def plot_scatter(data, title, save, filename):
    """Affiche un scatter plot des données brutes."""
    plt.figure(figsize=(6, 5))
    plt.scatter(data['x'], data['z'], c=data[title], cmap='inferno', alpha=0.7)
    plt.colorbar(label=title)
    plt.xlabel("x")
    plt.ylabel("z")
    plt.title(f"Scatter Plot - {title}")
    if save:
        plt.savefig(filename, dpi=300)
    

"""ANIMATION"""
def create_animation(averages, mesh_x, mesh_z, quantity, folder, fps=5):
    """Crée l'animation des résultats en utilisant FuncAnimation."""
    filename ="animation.mp4"
    # Trouver les min et max globaux sur l'ensemble de toutes les moyennes (toutes les frames)
    global_min = np.min([np.min(avg) for avg in averages])
    global_max = np.max([np.max(avg) for avg in averages])
    if global_min == 0 :
        global_min = - global_max / 2

    # Créer la figure et l'axe pour l'animation
    fig, ax = plt.subplots(figsize=(6, 5))

    # Définir la normalisation avec les min et max globaux
    norm = TwoSlopeNorm(vmin=global_min, vcenter=0, vmax=global_max)

    # Créer une liste de toutes les images à l'avance
    images = []
    for avg in averages:
        img = ax.pcolormesh(mesh_x, mesh_z, avg, shading='auto', cmap='coolwarm', norm=norm)
        images.append([img])

    # Créer l'animation avec les images pré-calculées
    ani = animation.ArtistAnimation(fig, images, interval=1000/fps, blit=True)

    # Sauvegarder l'animation
    ani.save(folder + filename, writer="ffmpeg", fps=fps, codec='h264')
    print(f"Animation sauvegardée sous {folder + filename}")

def process_chunk(chunk, folder, grid_size, neighbors, quantity):
    """Calculer la moyenne pour un chunk de fichiers."""
    sum_grid = np.zeros((grid_size, grid_size))
    count = 0

    for filename in chunk:
        filepath = os.path.join(folder, filename)
        data = pd.read_csv(filepath)

        if quantity == "P_N":
            _, _, grid = compute_meshgrid(data, "mvz2", grid_size, neighbors)
        elif quantity == "P_T":
            _, _, grid = compute_meshgrid(data, "mvx2", grid_size, neighbors)
        elif quantity == "P":
            _, _, grid_N = compute_meshgrid(data, "mvz2", grid_size, neighbors)
            _, _, grid_T = compute_meshgrid(data, "mvx2", grid_size, neighbors)
            grid = (grid_N + grid_T) / 2
        elif quantity == "diff":
            _, _, grid_N = compute_meshgrid(data, "mvz2", grid_size, neighbors)
            _, _, grid_T = compute_meshgrid(data, "mvx2", grid_size, neighbors)
            grid = grid_N - grid_T
        else:
            raise ValueError(f"Quantité non valide : {quantity}")

        sum_grid += grid
        count += 1

    avg_grid = sum_grid / count
    return avg_grid

def compute_average_in_chunks(files, folder, grid_size, neighbors, chunk_size, quantity):
    """Calculer la moyenne de chaque tranche de fichiers en fonction de la grandeur spécifiée."""
    # Diviser les fichiers en morceaux de taille chunk_size
    chunks = [files[i:i + chunk_size] for i in range(0, len(files), chunk_size)]
    averages = []

    with multiprocessing.Pool() as pool:
        results = pool.starmap(process_chunk, [(chunk, folder, grid_size, neighbors, quantity) for chunk in chunks])

    return results

def main():
    parser = argparse.ArgumentParser(description="Analyse des données de simulation")
    
    parser.add_argument("--folder", required=True, help="Dossier contenant les fichiers ensemble_i.csv")
    parser.add_argument("--i", type=int, help="Afficher les résultats pour le fichier i")
    parser.add_argument("--mean", action="store_true", help="Afficher la moyenne de tous les fichiers")
    parser.add_argument("--neighbors", type=int, default=2, help="Nombre de voisins à prendre en compte")
    parser.add_argument("--grid", type=int, default=100, help="Taille du meshgrid")
    parser.add_argument("--log", action="store_true", help="Afficher en variations log (accepte valeurs positives)")
    parser.add_argument("--raw", action="store_true", help="Affiche le scatter plot des données brutes")
    parser.add_argument("--save", action="store_true", help="Sauvegarde le plot généré sous un fichier image")
    parser.add_argument("--anim", type=int, help="Nombre de fichiers à traiter pour chaque tranche (animation)")
    parser.add_argument("--fps", type=int, default=5, help="Frames par seconde pour l'animation")

    
    parser.add_argument("--P-N", action="store_true", help="Affiche P_N (zz)")
    parser.add_argument("--P-T", action="store_true", help="Affiche P_T (xx)")
    parser.add_argument("--diff", action="store_true", help="Affiche P_N - P_T")
    parser.add_argument("--P", action="store_true", help="Affiche P")

    args = parser.parse_args()
    
    # Vérifier la validité du dossier
    if not os.path.isdir(args.folder):
        print(f"Erreur : le dossier '{args.folder}' n'existe pas.")
        return

    files = sorted([f for f in os.listdir(args.folder) if f.startswith("ensemble_") and f.endswith(".csv")])
    
    if args.i is not None:
        # Cas où l'on affiche un fichier spécifique
        filename = f"ensemble_{args.i}.csv"
        filepath = os.path.join(args.folder, filename)

        if not os.path.exists(filepath):
            print(f"Erreur : fichier '{filename}' introuvable.")
            return

        # Chargement des données du fichier
        data = pd.read_csv(filepath)
        print(f"Traitement du fichier : {filename}")

        if args.P_N:
            mesh_x, mesh_z, grid = compute_meshgrid(data, "mvz2", args.grid, args.neighbors)
            plot_meshgrid(mesh_x, mesh_z, grid, r"P_N (zz)", args.log, args.save, f"P_N_{args.i}.png")
            if args.raw:
                plot_scatter(data, "mvz2", args.save, f"scatter_{args.i}.png")

        if args.P_T:
            mesh_x, mesh_z, grid = compute_meshgrid(data, "mvx2", args.grid, args.neighbors)
            plot_meshgrid(mesh_x, mesh_z, grid, r"P_T (xx)", args.log, args.save, f"P_T_{args.i}.png")
            if args.raw:
                plot_scatter(data, "mvx2", args.save, f"scatter_{args.i}.png")

        if args.diff:
            mesh_x, mesh_z, grid_n = compute_meshgrid(data, "mvz2", args.grid, args.neighbors)
            _, _, grid_t = compute_meshgrid(data, "mvx2", args.grid, args.neighbors)
            plot_meshgrid(mesh_x, mesh_z, grid_n - grid_t, r"P_N - P_T", args.log, args.save, f"diff_{args.i}.png")
        
        if args.P:
            mesh_x, mesh_z, grid_n = compute_meshgrid(data, "mvz2", args.grid, args.neighbors)
            _, _, grid_t = compute_meshgrid(data, "mvx2", args.grid, args.neighbors)
            plot_meshgrid(mesh_x, mesh_z, (grid_n + grid_t)/2, r"(P_N + P_T)/2", args.log, args.save, f"P_{args.i}.png")

    if args.mean:
        # Calcul de la moyenne sur tous les fichiers du dossier
        print("Calcul de la moyenne sur tous les fichiers...")
        mesh_x, mesh_z, avg_P_N, avg_P_T = compute_average_on_grid(files, args.folder, args.grid, args.neighbors)

        # Affichage des résultats
        if args.P_N:
            plot_meshgrid(mesh_x, mesh_z, avg_P_N, r"Moyenne P_N (zz)", args.log, args.save, f"mean_P_N.png")

        if args.P_T:
            plot_meshgrid(mesh_x, mesh_z, avg_P_T, r"Moyenne P_T (xx)", args.log, args.save, f"mean_P_T.png")

        if args.diff:
            plot_meshgrid(mesh_x, mesh_z, avg_P_N - avg_P_T, r"P_N - P_T", args.log, args.save, f"mean_diff.png")
        
        if args.P:
            plot_meshgrid(mesh_x, mesh_z, (avg_P_N + avg_P_T)/2, r"(P_N + P_T)/2", args.log, args.save, f"P_{args.i}.png")

    if args.anim:

        if args.P_N:
            quantity = "P_N"
        elif args.P_T:
            quantity = "P_T"
        elif args.P:
            quantity = "P"
        elif args.diff:
            quantity = "diff"
        else:
            raise ValueError("Aucune grandeur spécifiée. Utilisez --P-N, --P-T, --P, ou --diff.")

        # Si --anim est fourni, créer l'animation sur les tranches
        print(f"Calcul de la moyenne par tranche de {args.anim} fichiers...")
        averages = compute_average_in_chunks(files, args.folder, args.grid, args.neighbors, args.anim, quantity)

        # Créer l'animation
        mesh_x, mesh_z, _ = compute_meshgrid(pd.read_csv(os.path.join(args.folder, files[0])), "mvx2", args.grid, args.neighbors)
        create_animation(averages, mesh_x, mesh_z, quantity, args.folder, args.fps)
        return

if __name__ == "__main__":
    main()
    plt.show()