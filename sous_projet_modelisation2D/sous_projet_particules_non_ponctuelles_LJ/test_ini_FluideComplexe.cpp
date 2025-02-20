#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Pour std::stod
#include "fluidecomplexe.h"  

double L_x = 50e-10;                  // Longueur de la boîte en x (en m)
double L_z = 100e-10;                 // Longueur de la boîte en z (en m)
double delta_t = 1e-8;               // Pas de temps pour la discrétisation (en s)
double kappa = 1.;                    // Coefficient de compressibilité isotherme
double tau_P = 1e-6;                  // Temps de réponse du barostat (en s)
double tau_T = 1e-6;                  // Temps de réponse du thermostat (en s)
double r_c = 4.0e-10;                 // Rayon de coupure des interactions (en m)

int main(int argc, char* argv[]) {
    if (argc != 3) {  // Vérifier qu'on a bien 2 arguments (fichier + température)
        std::cerr << "Usage: " << argv[0] << " <fichier_initialisation> <temperature>" << std::endl;
        return 1;
    }

    // Récupérer le fichier d'initialisation
    std::string fichier_ini = argv[1];

    // Récupérer la température et la convertir en double
    double T = std::stod(argv[2]);

    // Vérifier que la température est valide
    if (T <= 0) {
        std::cerr << "Erreur : La température doit être positive." << std::endl;
        return 1;
    }

    // Ouverture du fichier d'initialisation
    std::ifstream fichier(fichier_ini);
    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << fichier_ini << std::endl;
        return 1;
    }

    // Initialiser un objet de type FluideComplexe
    FluideComplexe fluide = FluideComplexe(L_x, L_z, delta_t, kappa, tau_P, tau_T, r_c, fichier_ini);

    // Initialisation et évolution du fluide
    double P = 1e6;  // Pression fixée
    std::cout << "T = " << T << " K et P = " << P << " Pa\n";
    std::string positionsCSV = "positions_";
    std::string vitessesCSV = "vitesses_";
    fluide.initialisation(T);
    for (int i = 0; i < 10000; i++) {
        fluide.evoluer(T, P);
        fluide.exporterPositionsCSV(positionsCSV + std::to_string(i) + ".csv");
        fluide.exporterVitessesCSV(vitessesCSV + std::to_string(i) + ".csv");

    }

    // Export des résultats
    //fluide.exporterPositionsCSV("positions_suivantes.csv");
    //fluide.exporterVitessesCSV("vitesses_suivantes.csv");

    std::cout << "Simulation terminée avec T = " << T << " K et fichier " << fichier_ini << "." << std::endl;

    return 0;
}