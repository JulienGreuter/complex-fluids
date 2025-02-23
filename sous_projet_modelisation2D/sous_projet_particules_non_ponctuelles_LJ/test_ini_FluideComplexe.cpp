#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Pour std::stod
#include "fluidecomplexe.h"  

double L_x = 50e-10;                  // Longueur de la boîte en x (en m)
double L_z = 100e-10;                 // Longueur de la boîte en z (en m)
double delta_t = 1e-8;               // Pas de temps pour la discrétisation (en s)
double kappa = 1.;                    // Coefficient de compressibilité isotherme
double tau_P = 1e-8;                  // Temps de réponse du barostat (en s)
double tau_T = 1e-8;                  // Temps de réponse du thermostat (en s)
double r_c = 10e-10;                 // Rayon de coupure des interactions (en m)

int main(int argc, char* argv[]) {
    if (argc != 4) {  // Vérifier qu'on a bien 3 arguments (fichier + température + nombre d'iterations)
        std::cerr << "Usage: " << argv[0] << " <fichier_initialisation> <temperature> <nombre_iterations>" << std::endl;
        return 1;
    }

    // Récupérer le fichier d'initialisation
    std::string fichier_ini = argv[1];

    // Récupérer la température et la convertir en double
    double T = std::stod(argv[2]);

    // Récupérer le nombre d'iterations et le convertir en int
    double N_it = std::stod(argv[3]);

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
    double P = 1e7;  // Pression fixée
    std::cout << "T = " << T << " K et P = " << P << " Pa\n";
    std::string positionsCSV = "positions_";
    std::string vitessesCSV = "vitesses_";
    fluide.initialisation(T);
    for (int i = 0; i < N_it; i++) {
        fluide.evoluer(T, P);
        fluide.exporterPositionsCSV(positionsCSV + std::to_string(i) + ".csv");
        fluide.exporterVitessesCSV(vitessesCSV + std::to_string(i) + ".csv");

    }

    std::cout << "Simulation de " << N_it << " pas de temps (" << delta_t << " s) terminée avec T = " << T << " K et fichier " << fichier_ini << "." << std::endl;

    return 0;
}