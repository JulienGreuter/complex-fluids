# **Modélisation Numérique des Fluides Complexes**  

## **Présentation du projet**  

Ce projet vise à modéliser numériquement les fluides complexes en utilisant une approche de dynamique moléculaire. L'objectif est d'étudier les interfaces entre phases (liquide-gaz, liquide-liquide, etc.) en simulant les interactions intermoléculaires via un potentiel de Lennard-Jones.  

Le code est écrit en **C++** pour les simulations et l'exportation des données, tandis que **Python** est utilisé pour l'analyse et la visualisation des résultats. L'architecture du projet permet une évolution future avec des outils modulaires pour explorer diverses propriétés physiques, notamment la pression interfaciale et la tension de surface.  

---

## **Organisation du projet**  

```plaintext
.
├── Makefile
├── README.md
├── bin/
│   └── sous_projet_modelisation2D/
│       └── sous_projet_particules_non_ponctuelles_LJ/
│           ├── export_data
│           └── test_FluideComplexe
├── build/
│   └── sous_projet_modelisation2D/
│       └── sous_projet_particules_non_ponctuelles_LJ/
│           ├── case.o
│           ├── export_data.o
│           ├── fluidecomplexe.o
│           ├── particules.o
│           ├── reseau.o
│           └── test_FluideComplexe.o
├── docs/                                   *(Documentation du projet)*
│   ├── architecture.md
│   ├── figures_discussions.md
│   ├── sous_projet_discussions.md
│   ├── GIT.md                              *(pour prendre en main GIT)*
│   ├── WSL.md                              *(pour utiliser linux sur une machine windows)*
│   └── utilisations.md                     *(pour un guide détaillé sur l'utilisation)*
├── figures/  *(Fichiers d'initialisation et visualisations)*
│   ├── datas/
|   ├── runs/                               *(pour effectuer des tests, dossier non suivie par GIT)*
│   ├── fichier_initialisation/
│   └── visualisations/
├── scripts/                                *(Scripts Python & Shell pour l'analyse et le post-traitement)*
│   ├── anim.py
│   ├── evolution.py
│   ├── force_LJ.py
│   ├── plot_map_pression.py
│   ├── plot_reseau2D.py
│   ├── plot_vitesses.py
│   ├── split_files.sh
│   └── test_auto_Reseau.sh
├── sous_projet_modelisation2D/             *(Code source de la simulation)*
│   ├── sous_projet_particules_non_ponctuelles_LJ/
│   │   ├── README.md                       *(Explication du sous projet)*
│   │   ├── case.cpp
│   │   ├── case.h
│   │   ├── export_data.cpp
│   │   ├── fluidecomplexe.cpp
│   │   ├── fluidecomplexe.h
│   │   ├── particules.cpp
│   │   ├── particules.h
│   │   ├── reseau.cpp
│   │   ├── reseau.h
│   │   └── test_FluideComplexe.cpp
│   └── vec2.h
└── tests/                                  *(Fichiers de test)*
```

---

## **Installation et Compilation**  

### **1. Compilation du projet**  

```bash
make
```

pour un guide sur l'utilisation du projet : voir [`docs/utilisations.md`](docs/utilisations.md)

### **2. Installation des dépendances Python**

Il est recommandé de créer un environnement virtuel pour ce projet. Voici comment procéder :

1. Créez un environnement virtuel :
    ```
    python -m venv mon_env
    ```

2. Activez l'environnement virtuel :
    ```bash
    $ . PATHS/mon_env/bin/activate
    ```

3. Installez les dépendances Python requises :
    ```
    (mon_env) $ pip install numpy pandas scipy matplotlib argparse
    ```

Cela installera toutes les bibliothèques nécessaires pour exécuter les programmes Python de ce projet.


## **Évolution du Projet**  

Les axes d’amélioration envisagés incluent :  
- L’optimisation des performances de la simulation.  
- L’extension à des modèles de particules plus complexe.  
- L’ajout de nouvelles métriques d’analyse.  
- Passage en 3D

---

## **Contributeurs et Contact**  

Merci de contacter les développeurs via les *issues* sur GitHub pour toute question ou suggestion.

Des espaces de discussions sont également disponibles dans le répertoire `docs/` :

- **architecture.md** : Discussion sur l'architecture du projet.
- **figures_discussions.md** : Discussion sur les besoins et les méthodes pour obtenir des figures.
- **sous_projet_discussions.md** : Discussion sur la nature des sous-projets.

N'hésitez pas à participer et à échanger vos idées pour améliorer ce projet !
