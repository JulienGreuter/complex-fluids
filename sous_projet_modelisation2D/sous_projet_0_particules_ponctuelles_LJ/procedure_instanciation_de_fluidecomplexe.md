# 📋 Récapitulatif de la procédure d'instanciation de FluideComplexe
Voici un résumé structuré et clair de la démarche d'instanciation d'un objet `FluideComplexe` en utilisant un fichier texte `fichier_initialisation.txt` pour configurer les ensembles de particules.

# **1. Structure du fichier `fichier_initialisation.txt`**
Le fichier contiendra les informations nécessaires pour créer chaque ensemble de particules, notamment :

- Paramètres du potentiel de Lennard-Jones :
    - `E_0` (**énergie du potentiel**)
    - `d` (**distance caractéristique du potentiel**)
- **Charge** électrique (`charge`)
- **Taille** des particules (`taille`)
- **Nombre** de particules (`N`)
- **Identifiant d'initialisation** (`id_ini`) pour attribuer un domaine à chaque ensemble de particules.

Exemple de contenu de `fichier_initialisation.txt` :
```
# Format : N E_0 d charge taille id_ini
100 1.0 0.01  0.0 0.0 D31
250 1.2 0.015 0.0 0.0 D32
500 1.0 0.01  0.0 0.0 D32
100 1.0 0.01  0.0 0.0 D33
```
Explication de l'exemple :
```
la boite de simulation est separé en 3 tranches adjacentes D31,D32,D33.
On a deux types de particules dans la boite :
    type A : E_0 = 1.0, d = 0.01,  charge = 0.0, taille = 0.0
    type B : E_0 = 1.2, d = 0.015, charge = 0.0, taille = 0.0

    les particules de type A sont presentent dans toute la boite :
        100 dans D31
        500 dans D32
        100 dans D33

    les particules de type B sont presentent au centre la boite :
        250 dans D32
cette situation initiale pourrait correspondre à une interface liquide/vapeur,
où le liquide serait composé des particules A et B, et la vapeurs des particules A.
```
# **2. Domaines disponibles pour l'initialisation**
Les domaines possibles sont prédéfinis avec les limites suivantes :

- Domaine `D1` :

`xmax = L_x/2`, `xmin = -L_x/2`
`zmax = L_z/2`, `zmin = -L_z/2`

- Domaine `D2` :

`xmax = L_x/2`, `xmin = -L_x/2`
`zmax = L_z/4`, `zmin = -L_z/4`

- Domaines `D31`, `D32`, `D33` (utilisables simultanément) :

    - `D31` : `xmax = L_x/2`, `xmin = -L_x/2`, `zmax = -L_z/4`, `zmin = -L_z/2`
    - `D32` : `xmax = L_x/2`, `xmin = -L_x/2`, `zmax = L_z/4`, `zmin = -L_z/4`
    - `D33` : `xmax = L_x/2`, `xmin = -L_x/2`, `zmax = L_z/2`, `zmin = L_z/4`

Les domaines `D31`, `D32`, et `D33` permettent d'initialiser plusieurs ensembles de particules simultanément dans differents domaines sans chevauchement.

# **3. Étapes pour instancier `FluideComplexe`**
- ### Lecture du fichier `fichier_initialisation.txt` :
    Lire les paramètres de chaque ensemble de particules et stocker les informations dans des objets de type `Particules`.

- ### Tri des ensembles par taille croissante :
    Pour chaque domaine attribué, trier les ensembles de particules par taille (`Particules.taille`). **Le tri est facultatif si toutes les tailles sont égales** (par exemple, pour des particules ponctuelles avec `taille = 0`).

- ### Initialisation des positions :
    Utiliser la méthode d'initialisation de la classe `FluideComplexe` pour assigner des positions aux particules en utilisant la classe `Reseau` pour gérer l'agencement des particules dans **le domaine**.

- ### Ajout des ensembles de particules à l'attribut particules de `FluideComplexe` :
    Une fois les positions initialisées, ajouter les ensembles de particules au vecteur `particules` de `FluideComplexe`.

- ### Répétition du processus pour chaque domaine attribué :
    Pour chaque domaine (`D31`, `D32`, `D33`), construire le vecteur des ensembles de particules correspondant, initialiser leurs positions, puis passer au domaine suivant. Sinon une seule étape est necessaire pour `D1` et `D2`.

# **4. Modularité et automatisation**
Pour automatiser l'instanciation de plusieurs objets `FluideComplexe` avec des paramètres `E_0` et `d` variables, il faut :

- ### Créer plusieurs fichiers d'initialisation (`fichier_initialisation_1.txt`, `fichier_initialisation_2.txt`, etc.), chacun contenant des valeurs différentes de **E_0** et **d** ( via un script Bash ).

- ### Utiliser un **script Bash** pour parcourir ces fichiers et lancer le programme de simulation avec chacun d'eux.

    - Exemple de script Bash : run_simulations.sh
    ```
    #!/bin/bash

    # Boucle sur tous les fichiers d'initialisation dans le répertoire courant

    for file in fichier_initialisation_*.txt; do
    echo "Lancement de la simulation avec le fichier $file"
    ./programme_executable $file
    done
    ```
    - Pour rendre le script exécutable :
    ```
    chmod +x run_simulations.sh
    ```
- ### Passer le nom du fichier d'initialisation en argument du programme C++ :
    - Utilisez argparse ou std::ifstream pour lire le fichier spécifié depuis la ligne de commande.

    - Exemple de code pour le main :
```
#include "fluidecomplexe.h"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <fichier_initialisation.txt>" << std::endl;
        return 1;
    }

    std::string fichier_initialisation = argv[1];
    std::ifstream fichier(fichier_initialisation);

    if (!fichier.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << fichier_initialisation << std::endl;
        return 1;
    }

    // Exemple de lecture et traitement des données
    double L_x = 10.0;
    double L_z = 30.0;
    double delta_t = 0.001;
    double kappa = 1.0;
    double tau_P = 1.0;
    double tau_T = 1.0;
    double r_c = 1.0;

    FluideComplexe fluide(L_x, L_z, delta_t, kappa, tau_P, tau_T, r_c);

    // Appeler une méthode pour initialiser les particules à partir du fichier
    fluide.initialisation(fichier);

    fichier.close();

    // Lancer l'évolution du fluide
    fluide.evoluer(300.0, 1.0);  // Exemple : température = 300 K, pression = 1 atm

    return 0;
}
```

# Points à vérifier
- ## Validation des domaines :
    Vérifiez que les domaines assignés ne se chevauchent pas lors de l'initialisation des positions.

- ## Gestion des erreurs :
    Ajoutez des vérifications pour s'assurer que les fichiers d'initialisation sont corrects et que les domaines spécifiés existent bien (D31, D32, D33).

- ## Performance :
    Optimisez le tri des ensembles et l'initialisation des positions pour éviter les redondances et garantir une exécution efficace.

# Conclusion
Cette approche modulaire permet de :

- Facilement **configurer** différents ensembles de particules **via** un **fichier texte**.
- **Automatiser l'instanciation** de plusieurs objets `FluideComplexe` avec un **script Bash**.
- **Observer l'influence des paramètres** `E_0` et `d` sur le comportement du fluide en générant **différents fichiers d'initialisation**.