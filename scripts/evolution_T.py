import argparse
import pandas as pd
import matplotlib.pyplot as plt

# Parser les arguments de la ligne de commande
parser = argparse.ArgumentParser(description="Tracer l'évolution de la température avec échelle log.")
parser.add_argument('--input', type=str, required=True, help="Fichier de données contenant les températures.")
parser.add_argument('--delta_t', type=float, default=1e-7, help="Intervalle de temps entre chaque point (en secondes).")
args = parser.parse_args()

# Lire le fichier et extraire les températures
df = pd.read_csv(args.input, header=None, skiprows=2, names=["Temperature"])
temperatures = df["Temperature"].tolist()

# Calculer le temps pour chaque pas de température
temps = [i * args.delta_t for i in range(len(temperatures))]

# Tracer la température en fonction du temps avec échelle logarithmique
plt.plot(temps, temperatures, '.', label="Température (K)")
plt.xlabel('Temps (s)')
plt.ylabel('Température (K)')
plt.yscale('log')  # Mettre l'échelle en logarithmique
plt.title(f'Évolution de la Température (delta_t = {args.delta_t:.1e})')
plt.legend()
plt.grid(True)
plt.show()