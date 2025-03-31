#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Pour std::stod
#include "fluidecomplexe.h"  

double L_x = 150e-10;                 // Longueur de la boîte en x (en m)
double L_z = 150e-10;                 // Longueur de la boîte en z (en m)
double delta_t = 5e-14;               // Pas de temps pour la discrétisation (en s)
double kappa = 1.0;                   // Coefficient de compressibilité isotherme
double tau_P = 1e-12;                 // Temps de réponse du barostat (en s)
double tau_T = 1e-12;                 // Temps de réponse du thermostat (en s)
double r_c = 30e-10;                  // Rayon de coupure des interactions (en m)

int main(int argc, char* argv[]) {
    if (argc != 5) {  // Vérifier qu'on a bien 4 arguments (fichier + température + pression + nombre d'evolutions)
        std::cerr << "Usage: " << argv[0] << " <fichier_initialisation> <temperature> <pression> <N_evolutions>" << std::endl;
        return 1;
    }

    // Récupérer le fichier d'initialisation
    std::string fichier_ini = argv[1];
    // Récupérer la température et la convertir en double
    double T = std::stod(argv[2]);
    // Récupérer la pression et la convertir en double
    double P = std::stod(argv[3]); // en Pa.m et ODG : 1e-3 à 1e-1
    // Récupérer le nombre d'evolutions et le convertir en int
    int N_evolutions = std::stoi(argv[4]);

    // Vérifier que la température est valide
    if (T <= 0) {
        std::cerr << "Erreur : La température doit être positive." << std::endl;
        return 1;
    }
    // Vérifier que la température est valide
    if (P <= 0) {
        std::cerr << "Erreur : La pression doit être positive." << std::endl;
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

    std::cout << "T = " << T << " K et P = " << P << " Pa.m\n";
    std::string positionsCSV = "positions_";
    std::string vitessesCSV = "vitesses_";
    fluide.initialisation(T);
    for (int i = 0; i < N_evolutions; i++) {
        fluide.evoluer(T, P);
        if ( i%10 == 0 ){
            fluide.exporterPositionsNormaliseesCSV("positions_" + std::to_string(i) + ".csv");
        }
        if ( i%500 == 0 ){
            fluide.exporterVitessesCSV("vitesses_" + std::to_string(i) + ".csv");
        }
    }

    std::cout << "Simulation de " << N_evolutions << " pas de temps (" << delta_t << " s) terminée avec T = " << T << " K et P = " << P << " Pa.m  via " << fichier_ini << "." << std::endl;

    return 0;
}
