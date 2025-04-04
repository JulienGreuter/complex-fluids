#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Pour std::stod
#include "fluidecomplexe.h"

double L_x = 150e-10;                  // Longueur de la boîte en x (en m)
double L_z = 150e-10;                 // Longueur de la boîte en z (en m)
double delta_t = 5e-14;               // Pas de temps pour la discrétisation (en s)
double kappa = 1.;                    // Coefficient de compressibilité isotherme
double tau_P = 1e-12;                  // Temps de réponse du barostat (en s)
double tau_T = 1e-12;                  // Temps de réponse du thermostat (en s)
double r_c = 50e-10;                 // Rayon de coupure des interactions (en m)

int main(int argc, char* argv[]) {
    if (argc != 6) {  // Vérifier qu'on a bien 5 arguments (fichier + température + pression + nombre de tranches + nombre d'evolutions)
        std::cerr << "Usage: " << argv[0] << " <fichier_initialisation> <temperature> <pression> <N_tranches> <N_evolutions>" << std::endl;
        return 1;
    }

    // Récupérer le fichier d'initialisation
    std::string fichier_ini = argv[1];
    // Récupérer la température et la convertir en double
    double T_fin = std::stod(argv[2]);
    // Récupérer la pression et la convertir en double
    double P = std::stod(argv[3]); // en Pa.m et ODG : 1e-3 à 1e-1
    // Récupérer le nombre d'iterations et le convertir en int
    int N_tranches = std::stoi(argv[4]);
    // Récupérer le nombre d'evolutions et le convertir en int
    int N_evolutions = std::stoi(argv[5]);

    // Vérifier que la température est valide
    if (T_fin <= 0) {
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

    double Delta_z; // epaisseur des tranches de fluide utilisé pour calculer les pressions

    // Initialiser un objet de type FluideComplexe
    FluideComplexe fluide = FluideComplexe(L_x, L_z, delta_t, kappa, tau_P, tau_T, r_c, fichier_ini);

    // Initialisation du fluide
    double T_ini = 1, T;
    fluide.initialisation(T_ini);

    for (int i = 0; i < N_evolutions; i++){
        if (i<500){T = T_ini + (T_fin - T_ini) * i /500;} //rampe de temperature
    
        L_z = fluide.getL_z();
        r_c = fluide.getR_c();
        Delta_z = L_z / 100.0;

        std::ofstream fichier_csv("pressions_" + std::to_string(i) + ".csv");
        if (!fichier_csv) {
            std::cerr << "Erreur : Impossible d'ouvrir le fichier pressions_" << i << ".csv" << std::endl;
            return 1;
        }
        std::cout << "Ecriture du fichier pressions_" <<  i << ".csv...\n";
        fichier_csv << "# fichier d'initialisation : " << fichier_ini 
                    << "  T = " << T << std::endl;
        fichier_csv << "P_N,P_T,z_k\n";  // En-tête du fichier CSV
        
        // Calcul du tenseur de pression à chaque tranche z_k
        for (double z_k = -1/2.0; z_k <= 1/2.0; z_k += 1.0 / N_tranches) {  
            // Calcul des tenseurs de pression P_N et P_T
            double P_N = fluide.calculer_tenseur_pression(1, 1, Delta_z, z_k * L_z);
            double P_T = fluide.calculer_tenseur_pression(0, 0, Delta_z, z_k * L_z);

            // Écrire les résultats dans le fichier CSV
            fichier_csv << P_N << "," << P_T << "," << z_k << "\n";
        }
        
        // Faire évoluer le fluide pour 1 pas de temps entre chaques mesures des tenseurs de pression 
        fluide.evoluer(T, P);  
        
        fluide.exporterPositionsNormaliseesCSV("positions_" + std::to_string(i) + ".csv");
        if ( i%50 == 0 ){
            fluide.exporterVitessesCSV("vitesses_" + std::to_string(i) + ".csv");
        }
    }
    std::cout << "Simulation terminée et fichiers CSV générés.\n";
    return 0;
}