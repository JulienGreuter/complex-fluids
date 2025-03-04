import argparse
import os
import numpy as np
import matplotlib.pyplot as plt
import glob
import itertools
import matplotlib.cm as cm

parser = argparse.ArgumentParser(description="Tracer les pressions à partir des fichiers pressions_i.csv dans un dossier.")
parser.add_argument("--folder", type=str, help="Chemin du dossier contenant les fichiers pressions_i.csv")
parser.add_argument("--mean", action="store_true", help="Afficher la moyenne de tous les fichiers")
parser.add_argument("--integrale", action="store_true", help="Afficher l'integrale de la moyenne'")
parser.add_argument("--i", type=int, help="numero du fichier à afficher specifiquement")
args = parser.parse_args()

def lire_fichier(fichier):
    with open(fichier, 'r') as f:
        lignes = f.readlines()
    
    # Trouver l'index où commencent les données
    debut_donnees = next(i for i, ligne in enumerate(lignes) if not ligne.startswith("#"))
    
    # Charger les données en ignorant la première ligne d'en-tête
    data = np.loadtxt(lignes[debut_donnees+1:], delimiter=",")
    P_N, P_T, z_k = data[:, 0], data[:, 1], data[:, 2]
    
    return P_N, P_T, z_k

def tracer_graphique(dossier, mean):
    fichiers = sorted(glob.glob(os.path.join(dossier, "pressions_*.csv")))
    if not fichiers:
        print("Aucun fichier trouvé dans le dossier spécifié.")
        return
    
    nb_fichiers = len(fichiers)
    cmap_red = cm.Reds(np.linspace(0.4, 1, nb_fichiers))
    cmap_green = cm.Greens(np.linspace(0.4, 1, nb_fichiers))
    cmap_blue = cm.Blues(np.linspace(0.4, 1, nb_fichiers))
    
    plt.figure(figsize=(10, 6))
    
    all_P_N = []
    all_P_T = []
    all_z_k = []
    
    for idx, fichier in enumerate(fichiers):
        i = int(os.path.splitext(os.path.basename(fichier))[0].split("_")[-1])  # Extraction de i
        P_N, P_T, z_k = lire_fichier(fichier)
        
        all_P_N.append(P_N)
        all_P_T.append(P_T)
        all_z_k.append(z_k)
        
        delta_P = P_N - P_T
        
        #plt.plot(z_k, P_N, color=cmap_red[idx], linestyle="-", alpha = 0.2)
        #plt.plot(z_k, P_T, color=cmap_green[idx], linestyle="--", alpha = 0.2)
        plt.plot(z_k, delta_P, color=cmap_blue[idx], linestyle="-.", alpha = 0.2)

        if args.i == i :
            plt.plot(z_k, P_N, color=cmap_red[idx], linestyle="-", alpha = 0.9)
            plt.plot(z_k, P_T, color=cmap_green[idx], linestyle="-", alpha = 0.9)
            plt.plot(z_k, delta_P, color=cmap_blue[idx], linestyle="-", alpha = 0.9, linewidth=3)
    
    if mean:
        P_N_mean = np.mean(all_P_N, axis=0)
        P_T_mean = np.mean(all_P_T, axis=0)
        z_k_mean = np.mean(all_z_k, axis=0)
        delta_P_mean = P_N_mean - P_T_mean
        
        
        plt.plot(z_k_mean, P_N_mean, color="darkred", linestyle="-", linewidth=2, label="P_N moyen")
        plt.plot(z_k_mean, P_T_mean, color="darkgreen", linestyle="--", linewidth=2, label="P_T moyen")
        plt.plot(z_k_mean, delta_P_mean, color="darkblue", linestyle="-.", linewidth=2, label="P_N - P_T moyen")

        if args.integrale :
            integrale_delta_P_mean = np.cumsum(delta_P_mean) 
            plt.plot(z_k_mean, integrale_delta_P_mean, color="yellow", linestyle="-", linewidth=4, label="cumsum de P_N - P_T moyen")
    

    
    plt.xlabel("z_k")
    plt.ylabel("Pression")
    plt.title("Évolution des pression en fonction de z_k")
    plt.legend()
    plt.grid()
    plt.show()

if __name__ == "__main__":
    tracer_graphique(args.folder, args.mean)