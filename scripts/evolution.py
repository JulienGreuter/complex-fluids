import argparse
import pandas as pd
import matplotlib.pyplot as plt

# Parser les arguments de la ligne de commande
parser = argparse.ArgumentParser(description="Tracer l'évolution d'une données avec échelle log.")
parser.add_argument('--input', type=str, required=True, help="Fichier de données.")
parser.add_argument('--delta_t', type=float, default=5e-14, help="Intervalle de temps entre chaque point (en secondes).")
parser.add_argument('--title', type=str, default = "X", help="Noms des données.")
args = parser.parse_args()

# Lire le fichier et extraire les températures
df = pd.read_csv(args.input, header=None, skiprows=2, names=["args.title"])
data = df["args.title"].tolist()

# Calculer le temps pour chaque pas de température
temps = [i * args.delta_t for i in range(len(data))]

# Tracer la température en fonction du temps avec échelle logarithmique
plt.plot(temps, data, '.', label= args.title)
plt.xlabel('Temps (s)')
plt.ylabel(args.title)
plt.yscale('log')  # Mettre l'échelle en logarithmique
plt.title('Évolution de ' + args.title + f' (delta_t = {args.delta_t:.1e})')
plt.legend()
plt.grid(True)
plt.show()