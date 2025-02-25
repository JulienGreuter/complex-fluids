#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Pour std::stod
#include "../../../sous_projet_modelisation2D/vec2.h"

// Fonction pour appliquer les conditions périodiques correctement
double periodic_boundary(double coord, double L) {
    return coord - L * std::round(coord / L);
}

int main(int argc, char* argv[]){

    double x = std::stod(argv[1]);
    double z = std::stod(argv[2]);

    Vec2 position = Vec2(x,z);
    position.afficher();
    bool modifie = false;
    double L_x = 50.0;
    double L_z = 100.0;

    // Correction de la position avec conditions périodiques
    position.x = periodic_boundary(position.x, L_x);
    position.z = periodic_boundary(position.z, L_z);

    

    position.afficher();
    std::cout << "modifié : " << modifie << std::endl;
    return 0;
}