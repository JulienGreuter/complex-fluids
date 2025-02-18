import argparse
import matplotlib.pyplot as plt
import numpy as np
import re

# Constante de Boltzmann en J/K
k_B = 1.38e-23

# Configuration d'argparse
parser = argparse.ArgumentParser(description="Visualisation de la distribution des vitesses des particules")
parser.add_argument("--input", required=True, help="Nom du fichier contenant les vitesses")
parser.add_argument("--output", help="Nom du fichier image à sauvegarder (optionnel)", default=None)
args = parser.parse_args()

# Lecture du fichier et séparation des ensembles
ensembles = []
parametres = []
with open(args.input, "r") as f:
    lignes = f.readlines()

    current_ensemble = []
    current_params = None

    for ligne in lignes:
        ligne = ligne.strip()
        if ligne.startswith("# Ensemble de particules :"):
            if current_ensemble:
                ensembles.append(np.array(current_ensemble))  # Ajouter l'ensemble précédent
                parametres.append(current_params)
                current_ensemble = []

            # Extraction des paramètres
            match = re.search(r"N = (\d+) \| d = ([\d\.e-]+) \| E_0 = ([\d\.e-]+) \| taille = ([\d\.e-]+) \| masse = ([\d\.e-]+)", ligne)
            if match:
                N, d, E_0, taille, masse = map(float, match.groups())
                current_params = {
                    "N": int(N),
                    "d": d,
                    "E_0": E_0,
                    "taille": taille,
                    "masse": masse
                }

        elif ligne and not ligne.startswith("#"):
            try:
                vx, vz = map(float, ligne.split(","))
                current_ensemble.append((vx, vz))
            except ValueError:
                continue

    if current_ensemble:
        ensembles.append(np.array(current_ensemble))
        parametres.append(current_params)

# Création des subplots
fig, axes = plt.subplots(len(ensembles), 2, figsize=(6, 2 * len(ensembles)))

if len(ensembles) == 1:
    axes = [axes]  # S'assurer que axes est une liste même pour un seul ensemble

for i, (data, params) in enumerate(zip(ensembles, parametres)):
    vx = data[:, 0]
    vz = data[:, 1]
    vitesses = np.sqrt(vx**2 + vz**2)

    # Nuage de points v_x vs v_z
    ax1 = axes[i][0]
    ax1.scatter(vx, vz, alpha=0.5, s=5, label=f"N={params['N']}, d={params['d']}, E₀={params['E_0']}, taille={params['taille']}, masse={params['masse']}")
    ax1.set_xlabel("v_x", fontsize=8)
    ax1.set_ylabel("v_z", fontsize=8)
    ax1.set_title("Nuage de points", fontsize=9)
    ax1.legend(loc="upper right", fontsize=7, frameon=False)  # Légende discrète
    ax1.grid(True)

    # Histogramme des vitesses |v|
    ax2 = axes[i][1]
    bins = np.linspace(0, vitesses.max(), 30)
    ax2.hist(vitesses, bins=bins, density=True, alpha=0.7, color="blue", label="Données")

    # Ajout de la courbe théorique de Maxwell-Boltzmann en 2D
    m = params["masse"]
    T = 300  # Température fixée à 300K

    v_theorique = np.linspace(0, vitesses.max(), 100)
    P_v = (m / (k_B * T)) * v_theorique * np.exp(-m * v_theorique**2 / (2 * k_B * T))
    ax2.plot(v_theorique, P_v, color="red", linestyle="--", label="Théorique (Maxwell-Boltzmann)")

    ax2.set_xlabel("|v|", fontsize=8)
    ax2.set_ylabel("Densité", fontsize=8)
    ax2.set_title("Distribution de Maxwell-Boltzmann", fontsize=9)
    ax2.legend(loc="upper right", fontsize=7, frameon=False)
    ax2.grid(True)

# Titre global plus discret
plt.suptitle("Distributions de vitesse des particules", fontsize=10)

# Ajuster l'espacement
plt.tight_layout(pad=1.5)

# Sauvegarde ou affichage
if args.output:
    plt.savefig(args.output, dpi=300)
    print(f"Image sauvegardée sous {args.output}")
else:
    plt.show()