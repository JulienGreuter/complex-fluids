#ifndef RESEAU_H
#define RESEAU_H

#include "case.h"  // Inclut la définition de la classe Case
#include <vector>
#include <iostream>

class Reseau {
public:
    // Limites du domaine
    double xmin, xmax, zmin, zmax;
    // Taille des cases dans le réseau
    double taille_case;
    // Dimensions du réseau en termes de cases
    int n_lignes, n_colonnes;
    // Matrice 2D représentant les cases (parent et enfants)
    std::vector<std::vector<Case>> cases;

    // Ensemble de toutes les cases libres (initialement toutes les cases)
    std::vector<Case*> cases_libres;

    // Constructeur
    Reseau(double xmin, double xmax, double zmin, double zmax, double taille_case);

    // Méthode pour tirer une case libre aléatoire
    Case* tirerCaseLibre();

    // Méthode pour subdiviser une case libre en 4 enfants
    void subdiviserCase(Case* case_a_subdiviser);

    // Méthode pour retirer une case de l'ensemble des cases libres
    void retirerCaseLibre(Case* case_a_retirer);

    // Méthode pour ajouter les enfants d'une case parent à cases_libres
    void ajouterEnfantsCasesLibres(Case* case_parent);
};

#endif // RESEAU_H
