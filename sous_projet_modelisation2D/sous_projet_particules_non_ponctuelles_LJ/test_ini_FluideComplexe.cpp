#include <iostream>
#include <fstream>
#include <string>
#include "fluidecomplexe.h"  

double L_x = 50.;                  // Longueur de la boîte en x
double L_z = 100.;                 // Longueur de la boîte en z
double delta_t = 1e-8;             // Pas de temps pour la discrétisation
double kappa = 1.;                 // Coefficient de compressibilité isotherme
double tau_P = 1.;                 // Temps de réponse du barostat
double tau_T = 1.;                 // Temps de réponse du thermostat
double r_c = 10.;                  // Rayon de coupure des interactions

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <fichier_initialisation>" << std::endl;
        return 1;
    }

    // Récupérer le fichier d'initialisation passé en argument
    std::string fichier_ini = argv[1];

    // Ouverture du fichier d'initialisation
    std::ifstream fichier(fichier_ini);
    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << fichier_ini << std::endl;
        return 1;
    }

    // Initialiser un objet de type FluideComplexe
    FluideComplexe fluide = FluideComplexe(L_x, L_z, delta_t, kappa, tau_P, tau_T, r_c, fichier_ini);

    // Appeler la méthode d'initialisation avec une température T 
    double T = 300.0; // Exemple de température
    fluide.initialisation(T);

    std::cout << "Initialisation du fluide avec le fichier " << fichier_ini << " terminée." << std::endl;

    return 0;
}