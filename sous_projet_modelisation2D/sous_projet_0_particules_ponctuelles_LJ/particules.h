#ifndef PARTICULES_H
#define PARTICULES_H

#include <vector>
#include "vec2.h"  // Inclut la structure Vec2 représentant un vecteur 2D (x, z)

class Particules {
public:
    // Attributs
    int N;                        // Nombre de particules
    double E_0;                   // Paramètre du potentiel de Lennard-Jones
    double d;                     // Paramètre du potentiel de Lennard-Jones
    double charge;                // Charge électrique des particules (initialisée à 0)
    std::vector<Vec2> positions;  // Positions des particules
    std::vector<Vec2> vitesses;   // Vitesses des particules
    double taille;                // Taille des particules (initialisée à 0 pour des particules ponctuelles)
    double masse;                 // Masse des particules

    // Constructeur
    Particules(int N, double E_0, double d, double charge = 0.0, double taille = 0.0, double masse);

    // Méthode pour initialiser les vitesses en fonction de la température T
    void initialiserVitesses(double T);

    // Méthode pour appliquer les conditions périodiques (dépendant de L_x et L_z)
    void appliquerConditionsPeriodiques(double L_x, double L_z);
};

#endif // PARTICULES_H
