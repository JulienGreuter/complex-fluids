# Utilisation des exécutables et analyse des résultats

Lors de l'utilisation des exécutables, il est recommandé de rediriger la sortie vers un fichier `run.txt`. Ceci permet de vérifier le bon déroulement de la simulation.

## Exécution des simulations

Après avoir exécuté :

```bash
$ make
```

Deux possibilités s'offrent à vous :

### 1. Utilisation de `export_data`

```bash
$ export_data <fichier_initialisation> <temperature> <pression> <N_evolutions> > run.txt
```

Cela génère les fichiers `positions_i.csv`, `vitesses_i.csv` et `ensemble_i.csv`, permettant d'analyser la simulation.

### 2. Utilisation de `test_FluideComplexe`

```bash
$ test_FluideComplexe <fichier_initialisation> <temperature> <pression> <N_evolutions> > run.txt
```

Cela génère uniquement les fichiers `positions_i.csv` et `vitesses_i.csv` pour une analyse plus sobre de la simulation.

## Précautions

Il est impératif de renseigner un fichier d'initialisation compatible avec les attributs du fluide complexe décrits dans `export_data.cpp` ou `test_FluideComplexe.cpp`, notamment :

- Dimensions de la boîte
- Pas de temps
- Autres paramètres nécessaires pour assurer la cohérence de la simulation

### Exemples de fichiers d'initialisation

Un exemple complet est disponible pour aider à la compréhension du format :
- [`figures/fichier_initialisation/exemple_fichier_ini.txt`](figures/fichier_initialisation/exemple_fichier_ini.txt)

Un exemple fonctionnel simple est également fourni :
- [`figures/fichier_initialisation/fichier_ini_Ar_D2_petit.txt`](figures/fichier_initialisation/fichier_ini_Ar_D2_petit.txt)

Cet exemple correspond à une simulation d'argon liquide en interface avec un gaz (non simulé, mais manifesté via le barostat) avec :
- Température : **40 K** (proposition de valeur)
- Pression : **3e-3 Pa.m** (proposition de valeur)
- **660 particules simulées** permettant d'atteindre plusieurs milliers de pas de temps rapidement.

Deux autres exemples fonctionnels existent avec plus de particules et une géométrie différente :
- [`figures/fichier_initialisation/fichier_ini_Ar_D2.txt`](figures/fichier_initialisation/fichier_ini_Ar_D2.txt)
- [`figures/fichier_initialisation/fichier_ini_Ar_DFM.txt`](figures/fichier_initialisation/fichier_ini_Ar_DFM.txt)
- Dans ces fichiers, les valeurs `L_x` et `L_z` doivent être modifiées dans `export_data.cpp` ou `test_FluideComplexe.cpp`.

## Analyse des résultats

### Suivi de l'évolution de la température

```bash
$ grep "T =" run.txt | awk '{print $3}' > temperature.csv
```

On peut adapter cette commande pour suivre **Pxx**, **Pzz**, le nombre d'applications des **conditions périodiques**, etc.

### Format des lignes utiles

``` run.txt
423 interactions en moyenne sur les particules de l'ensemble
Modification de : 0 particules      # Conditions périodiques
conteur_operations = 5200           # Barostat local rapide
lambda_moyen_P = 1                  # Taille de la boîte de simulation
N = 24 particules traitées          # Barostat local rapide
Pxx = 0.00293277 Pa.m
Pzz = 0.00314078 Pa.m
T = 40.3824 K
lambda_T = 0.999763                 # Thermostat
```

### Utilisation du script Python `evolution.py`

```bash
$ python scripts/evolution.py --input PATHS/temperature.csv --title T
```

## Vérification des résultats

### Vérification de la distribution des vitesses (`vitesses_i.csv`)

```bash
$ python scripts/plot_vitesses.py --input PATHS/vitesses_ini.csv --T 40
```

### Vérification du réseau et des positions des particules (`reseau_i.csv` ou `positions_i.csv`)

```bash
$ python scripts/plot_reseau2D.py --input PATHS/positions_ini.csv
```

## Animation des positions des particules

Pour animer un ensemble de fichiers `positions_i.csv` :

```bash
$ python scripts/anim.py --input-folder PATHS/folder --skip-frames 50 --fps 10 --tracer 10
```

Cette commande est adaptée pour `i` variant de 1 en 1 sur plusieurs milliers de fichiers et particules.

## Analyse de la pression dans le fluide (`ensemble_i.csv`)

Différentes options permettent d'obtenir des cartes, des animations et des grandeurs moyennées en fonction de l'axe de la boîte.

```bash
$ python scripts/plot_map_pression.py --input-folder PATHS/folder --mean --P --diff --grid 100 --neighbors 1
```
```bash
$ python scripts/plot_map_pression.py --input-folder PATHS/folder --anim 10 --fps 10 --P --grid 100 --neighbors 1
```
```bash
$ python scripts/plot_map_pression.py --input-folder PATHS/folder --flat --P --diff --P-N --P-T --grid 100 --neighbors 1
```

## Optimisation et sous-échantillonnage des fichiers

La rapidité du programme dépend de la puissance de la machine et de la taille/précision des données. Pour limiter l'utilisation des ressources, il est possible de traiter un sous-ensemble des fichiers avec `split_files.sh`.

Exemple de copie des fichiers `ensemble_i.csv` de `i = 0` à `1000` dans un dossier `0_1000/` :

```bash
bash ~/PATHS/scripts/split_files.sh ensemble 0 1000
```
