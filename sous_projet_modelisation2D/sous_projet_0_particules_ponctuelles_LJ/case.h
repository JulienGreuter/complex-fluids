#ifndef CASE_H
#define CASE_H

#include <vector>
#include <iostream>

class Case {
public:
    // Attributs publics
    double x, z;                // Position du centre de la case
    int ordre_subdivision;      // Ordre de subdivision de la case
    std::vector<Case*> enfants; // Enfants si c'est une case parent
    double taille_case;         // Taille de la case en fonction de son ordre de subdivision
    bool est_libre;             // Si la case est libre ou occupée

    // Constructeur
    Case(double x, double z, double taille_case, int ordre_subdivision = 0, bool est_libre = true);

    // Destructeur pour libérer la mémoire des enfants
    ~Case();

    // Méthode pour subdiviser une case en 4 enfants
    void subdiviser();

    // Méthode pour vérifier si la case est une case parent
    bool estParent() const;

    // Méthode pour vérifier si la case est libre
    bool estLibre() const;

    // Méthode pour marquer la case comme occupée
    void marquerOccupee();
};

#endif // CASE_H
