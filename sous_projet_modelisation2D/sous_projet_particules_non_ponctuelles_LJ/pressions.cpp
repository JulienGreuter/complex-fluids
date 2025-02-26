#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Pour std::stod
#include "fluidecomplexe.h"

double L_x = 200e-10;                  // Longueur de la boîte en x (en m)
double L_z = 800e-10;                 // Longueur de la boîte en z (en m)
double delta_t = 5e-14;               // Pas de temps pour la discrétisation (en s)
double kappa = 1.;                    // Coefficient de compressibilité isotherme
double tau_P = 1e-15;                  // Temps de réponse du barostat (en s)
double tau_T = 1e-15;                  // Temps de réponse du thermostat (en s)
double r_c = 50e-10;                 // Rayon de coupure des interactions (en m)

int main(int argc, char* argv[]) {
    if (argc != 4) {  // Vérifier qu'on a bien 3 arguments (fichier + température + nombre de tranches)
        std::cerr << "Usage: " << argv[0] << " <fichier_initialisation> <temperature> <nombre_tranches>" << std::endl;
        return 1;
    }

    // Récupérer le fichier d'initialisation
    std::string fichier_ini = argv[1];

    // Récupérer la température et la convertir en double
    double T = std::stod(argv[2]);

    // Récupérer le nombre d'iterations et le convertir en int
    int N_tranches = std::stoi(argv[3]);

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

    double P = 1e-2; // en Pa.m
    double Delta_z = L_z / 30.0; // epaisseur des tranches de fluide utilisé pour calculer les pressions

    // Initialiser un objet de type FluideComplexe
    FluideComplexe fluide = FluideComplexe(L_x, L_z, delta_t, kappa, tau_P, tau_T, r_c, fichier_ini);

    // Initialisation du fluide
    fluide.initialisation(T);

    // Faire évoluer le fluide pendant 1000 pas de temps pour le mettre dans un etats d'equillibre
    // Ou partir directement d'un etats via des fichiers CSV positions.csv et vitesses.csv // methode à implementer dans FluideComplexe
    for (int i = 0; i < 1000; i++) {
        fluide.evoluer(T, P);
        fluide.exporterPositionsCSV("positions_" + std::to_string(i) + ".csv");  
    }
    std::cout << "Fluide complexe à l'equillibre.\n";
    // Calcul du tenseur de pression et exportation dans des fichiers CSV (300)
    for (int i = 0; i < 300; i++){
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
        for (double z_k = -L_z/2; z_k <= L_z/2; z_k += L_z / N_tranches) {  
            // Calcul des tenseurs de pression P_N et P_T
            double P_N = fluide.calculer_tenseur_pression(1, 1, Delta_z, z_k);
            double P_T = fluide.calculer_tenseur_pression(0, 0, Delta_z, z_k);

            // Écrire les résultats dans le fichier CSV
            fichier_csv << P_N << "," << P_T << "," << z_k << "\n";
        }
        
        // Faire évoluer le fluide pour 10 pas de temps entre chaques mesures des tenseurs de pression 
        for (int i = 0; i < 10; i++) {
            fluide.evoluer(T, P);  
        }
    }
    std::cout << "Simulation terminée et fichiers CSV générés.\n";
    return 0;
}