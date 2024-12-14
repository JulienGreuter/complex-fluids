#ifndef RESEAU_H
#define RESEAU_H

#include "case.h"  // Inclut la définition de la classe Case
#include <vector>
#include <iostream>

class Réseau {
public:
    // Limites du domaine
    double xmin, xmax, zmin, zmax;
    // Taille des cases dans le réseau
    double taille_case;
    // Dimensions du réseau en termes de cases
    int n_lignes, n_colonnes;
    // Matrice 2D représentant les cases (parent et enfants)
    std::vector<std::vector<Case>> cases;

    // Constructeur
    Réseau(double xmin, double xmax, double zmin, double zmax, double taille_case);

    // Méthode pour tirer une case libre aléatoire
    Case* tirerCaseLibre();

    // Méthode pour subdiviser une case libre en 4 enfants
    void subdiviserCase(Case* case_a_subdiviser);

    // Méthode pour libérer la mémoire des cases occupées après subdivision
    void libererCasesOccupees();
};

#endif // RESEAU_H