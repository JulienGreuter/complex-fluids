#include <iostream>
#include <vector>
#include "vec2.h"

int main() {
    // Création de deux vecteurs
    Vec2 v1(3.0, 4.0);
    Vec2 v2(1.0, 2.0);

    // Test de l'affichage
    std::cout << "Vecteur v1: ";
    v1.afficher();

    std::cout << "Vecteur v2: ";
    v2.afficher();

    // Test de l'addition
    Vec2 v3 = v1 + v2;
    std::cout << "v1 + v2 = ";
    v3.afficher();

    // Test de la soustraction
    Vec2 v4 = v1 - v2;
    std::cout << "v1 - v2 = ";
    v4.afficher();

    // Test de la multiplication par un scalaire à droite ( ne marche pas pour multiplication à gauche )
    Vec2 v5 = v1 * 2.0;
    std::cout << "v1 * 2 = ";
    v5.afficher();


    // Test de la norme
    std::cout << "Norme de v1: " << v1.norme() << std::endl;

    // Test des opérateurs d'affectation += et -=
    v1 += v2;
    std::cout << "Après v1 += v2, v1 = ";
    v1.afficher();

    v1 -= v2;
    std::cout << "Après v1 -= v2, v1 = ";
    v1.afficher();

    // test vecteurs peridiques
    // Vecteurs de translation pour appliquer les conditions périodiques
    Vec2 ux(1.0, 0.0);  // Unité en x
    Vec2 uz(0.0, 1.0);  // Unité en z

    double L_x = 2.0, L_z = 3.0 ;

    std::vector<Vec2> vecteurs_periodiques = {
        Vec2(0.0, 0.0),                   // Vecteur nul (pas de translation)
        ux * L_x ,                         // Translation +L_x en x
        uz * L_z ,                         // Translation +L_z en z
        ux * -L_x ,                        // Translation -L_x en x
        uz * -L_z ,                        // Translation -L_z en z
        ux * L_x + uz * - L_z ,              // Translation +L_x et -L_z
        ux * L_x  + uz * L_z ,              // Translation +L_x et +L_z
        ux * -L_x  + uz* L_z ,             // Translation -L_x et +L_z
        ux * -L_x + uz * - L_z              // Translation -L_x et -L_z
    };

    std::cout << "ux = " ; ux.afficher();
    std::cout << "uz = " ; uz.afficher();

    for (auto v : vecteurs_periodiques){
        v.afficher();
    }

    return 0;
}