import pandas as pd
import numpy as np

# Charger les données en ignorant les lignes de commentaires
df = pd.read_csv("merged_vitesses.csv", comment='#', header=None, names=["vx", "vz"])

# Calcul de la vitesse scalaire v = sqrt(vx^2 + vz^2)
df["v2"] = df["vx"]**2 + df["vz"]**2
df["v"] = np.sqrt(df["v2"])
# Calcul des statistiques
min_v = df["v"].min()
max_v = df["v"].max()
mean_v = df["v"].mean()
sum_v = df["v2"].sum()

# Affichage des résultats
print(f"Min v  : {min_v}")
print(f"Max v  : {max_v}")
print(f"Moyenne v : {mean_v}")
print(f"Somme v2 : {sum_v}")