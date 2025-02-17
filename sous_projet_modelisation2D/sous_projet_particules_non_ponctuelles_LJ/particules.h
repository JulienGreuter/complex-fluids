#ifndef PARTICULES_H
#define PARTICULES_H

#include <vector>
#include "vec2.h"  // Inclut la structure Vec2 représentant un vecteur 2D (x, z)

class FluideComplexe;

class Particules {
private:
    // Attributs
    int N;                        // Nombre de particules
    double E_0;                   // Paramètre du potentiel de Lennard-Jones
    double d;                     // Paramètre du potentiel de Lennard-Jones
    double masse;                 // Masse des particules
    double taille;                // Taille des particules (initialisée à 0 pour des particules ponctuelles)
    double charge;                // Charge électrique des particules (initialisée à 0)
    std::vector<Vec2> positions;  // Positions des particules
    std::vector<Vec2> vitesses;   // Vitesses des particules

    friend class FluideComplexe;  // FluideComplexe peut accéder directement aux attributs privés

public:
    // Constructeur
    Particules(int N, double E_0, double d, double masse, double taille = 0.0, double charge = 0.0); // Pour l'utilisation dans FluideComplexe, positions et vitesses sont initialisés plus tard

    // Méthode pour initialiser les vitesses en fonction de la température T
    void initialiserVitesses(double T);

    // Méthode pour appliquer les conditions périodiques (dépendant de L_x et L_z)
    void appliquerConditionsPeriodiques(double L_x, double L_z);

    // Setter pour positions et vitesses dans le cas d'une utilisation de la classe Particules en dehors de la classe FluideComplexe
    void setPositions(const std::vector<Vec2>& newPositions);
    void setVitesses(const std::vector<Vec2>& newVitesses);

    // Getter pour accéder aux attributs privés
    const std::vector<Vec2>& getPositions() const;
    const std::vector<Vec2>& getVitesses() const;
};

#endif // PARTICULES_H
