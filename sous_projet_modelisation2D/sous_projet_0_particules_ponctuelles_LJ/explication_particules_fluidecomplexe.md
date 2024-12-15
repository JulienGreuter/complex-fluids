# 📝 **Explication des attributs et méthodes**
- ## **Classe `Particules`**
    Une instance de cette classe represente un ensemble de N particules de même nature.
    
    ## **Attributs :**

    - `N` : Nombre de particules.
    - `E_0` et `d` : Paramètres du potentiel de Lennard-Jones.
    - `charge` : Charge électrique des particules (initialisée à 0 par défaut).
    - `positions` : Vecteur des positions des particules.
    - `vitesses` : Vecteur des vitesses des particules.
    - `taille` : Taille des particules (initialisée à 0 pour des particules ponctuelles).
    
    ## **Méthodes :**

    - `Constructeur` : Initialise les paramètres des particules.
    - `initialiserVitesses(double T)` : Initialise les vitesses en fonction de la température T.
    - `appliquerConditionsPeriodiques(double L_x, double L_z)` : Applique les conditions périodiques en fonction des dimensions L_x et L_z de la boîte.

- ## **Classe `FluideComplexe`**
    
    ## **Attributs :**

    - `L_x` et `L_z` : Dimensions de la boîte de simulation.
    - `delta_t` : Pas de temps pour la discrétisation.
    - `kappa` : Coefficient de compressibilité isotherme.
    - `tau_P` et `tau_T` : Paramètres de réponse du barostat et du thermostat.
    - `r_c` : Rayon de coupure pour les interactions.
    - `particules` : Vecteur contenant plusieurs ensembles de particules.
    - `forces_interactions` : Vecteur des forces d'interactions correspondant aux ensembles de particules.
    
    ## **Méthodes :**

    - `Constructeur` : Initialise les paramètres de la boîte de simulation et des régulateurs.
    - `initialisation(double T, Reseau& reseau)` : Initialise les positions et vitesses des particules en utilisant les classes Reseau et Case.
    - `calculer_forces()` : Calcule les forces d'interactions entre les particules.
        📝 Explication du code

        - ### 1. Initialisation :
            - `forces_interactions.clear()` : Nettoie le vecteur des forces avant le recalcul.
            - Définition des vecteurs de translation périodiques.
        - ### 2. Boucles imbriquées :
            - Parcourt chaque ensemble de particules (`ensemble_i`) et chaque position (`position_i`).
            - Pour chaque particule `i`, parcourt tous les autres ensembles de particules (`ensemble_j`) et leurs positions (`position_j`).
        - ### 3. Conditions périodiques :
            - Applique les translations périodiques à la distance `r_ij`.
            - Calcule la force `f_ij` uniquement **si** la **norme de r_ij** est **inférieure** au **rayon de coupure** `r_c`.
        - ### 4. Stockage des forces :
            - La force totale sur chaque particule est ajoutée au vecteur forces_interactions.
    - `mettre_a_jour_positions()` : Met à jour les positions des particules en fonction des vitesses et des forces.
    - `mettre_a_jour_vitesses()` : Met à jour les vitesses des particules en fonction des forces.
    - `appliquer_conditions_periodiques()` : Applique les conditions périodiques à toutes les particules.
    - `appliquer_thermostat(double T)` : Modifie les vitesses des particules pour ajuster la température.
    - `appliquer_barostat(double P)` : Modifie les positions et vitesses des particules pour ajuster la pression.
    - `calculer_temperature()` : Calcule la température actuelle du fluide complexe.
    - `calculer_tenseur_pression(int alpha, int beta, double Delta_z, double z_k)` : Calcule la composante du tenseur de pression pour une tranche donnée. `(1,1) = zz`, `(0,0) = xx`, etc...
    - `evoluer(double T, double P)` : Met à jour l'état du fluide (positions, vitesses et forces) pour un pas de temps.

- ## Dépendances et compléments

    - `vec2.h` : Contient la définition de la structure `Vec2` représentant **un vecteur 2D (x, z)** avec des **surcharges des opérateurs** `+`, `-`, `*`, `+=`, `-=` et une méthode `norme()`.
    - `reseau.h` et `case.h` : Utilisés pour gérer l'initialisation des positions des particules dans la boîte de simulation.
    - La fonction `force_LJ` calcule la force de Lennard-Jones entre deux particules :

        ### Paramètres :
        - `E_0i`, `E_0j` : Paramètres d'énergie pour les particules `i` et `j`.
        - `di`, `dj` : Paramètres de distance pour les particules `i` et `j`.
        - `r_ij` : Vecteur de distance entre les particules `i` et `j`.
        ### Calculs :
        - **Moyenne** des paramètres d'énergie et de distance.
        - Calcul du facteur de la force en fonction de la distance.
        - **Retourne** la force sous forme de `Vec2`.
