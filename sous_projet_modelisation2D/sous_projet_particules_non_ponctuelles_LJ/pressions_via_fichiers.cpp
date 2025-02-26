#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Pour std::stod
#include <limits>
#include <sstream>
#include "fluidecomplexe.h"

double L_x = 800e-10; // Longueur de la boîte en x (m)
double L_z = 800e-10; // Longueur de la boîte en z (m)
double delta_t = 5e-14; // Pas de temps (s)
double kappa = 1.;
double tau_P = 1e-15;
double tau_T = 1e-15;
double r_c = 50e-10;

int main(int argc, char* argv[]) {
    if (argc != 5) { // Vérifier qu'on a bien 4 arguments
        std::cerr << "Usage: " << argv[0] << " <fichier_positions> <fichier_vitesses> <temperature> <nombre_tranches>" << std::endl;
        return 1;
    }

    std::string fichier_positions = argv[1];
    std::string fichier_vitesses = argv[2];
    double T = std::stod(argv[3]);
    int N_tranches = std::stoi(argv[4]);

    if (T <= 0) {
        std::cerr << "Erreur : La température doit être positive." << std::endl;
        return 1;
    }

    double max_x = std::numeric_limits<double>::lowest();
    double min_x = std::numeric_limits<double>::max();
    double max_z = std::numeric_limits<double>::lowest();
    double min_z = std::numeric_limits<double>::max();

    std::ifstream fichier(fichier_positions);
    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << fichier_positions << std::endl;
        return 1;
    }

    std::string ligne;
    std::getline(fichier, ligne); // Ignorer l'en-tête
    while (std::getline(fichier, ligne)) {
        if (ligne.empty() || ligne[0] == '#') continue;
        
        double x, z, taille;
        char temp;
        std::istringstream iss(ligne);
        if (!(iss >> x >> temp >> z >> temp >> taille)) continue;
        
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (z < min_z) min_z = z;
        if (z > max_z) max_z = z;
    }

    fichier.close();

    if (min_x < -L_x / 2 || max_x > L_x / 2 || min_z < -L_z / 2 || max_z > L_z / 2) {
        std::cerr << "Erreur : Certaines positions dépassent les limites initiales !" << std::endl;
        std::cerr << "Max X : " << max_x << " | Min X : " << min_x << " (Limite : [-" << L_x / 2 << ", " << L_x / 2 << "])" << std::endl;
        std::cerr << "Max Z : " << max_z << " | Min Z : " << min_z << " (Limite : [-" << L_z / 2 << ", " << L_z / 2 << "])" << std::endl;
        return 1;
    }

    double P = 1e-2;
    double Delta_z = L_z / 30.0;

    FluideComplexe fluide(L_x, L_z, delta_t, kappa, tau_P, tau_T, r_c, ""); // pas besoin de fichier d'initialisation
    fluide.initialisationViaCSV(fichier_positions, fichier_vitesses);

    std::cout << "Fluide complexe à l'équilibre.\n";

    for (int i = 0; i < 300; i++) {
        std::ofstream fichier_csv("pressions_" + std::to_string(i) + ".csv");
        if (!fichier_csv) {
            std::cerr << "Erreur : Impossible d'ouvrir le fichier pressions_" << i << ".csv" << std::endl;
            return 1;
        }
        std::cout << "Écriture du fichier pressions_" << i << ".csv...\n";
        fichier_csv << "P_N,P_T,z_k\n";
        
        for (double z_k = -L_z/2; z_k <= L_z/2; z_k += L_z / N_tranches) {
            double P_N = fluide.calculer_tenseur_pression(1, 1, Delta_z, z_k);
            double P_T = fluide.calculer_tenseur_pression(0, 0, Delta_z, z_k);
            fichier_csv << P_N << "," << P_T << "," << z_k << "\n";
        }
        
        for (int i = 0; i < 10; i++) {
            fluide.evoluer(T, P);
        }
    }
    std::cout << "Simulation terminée et fichiers CSV générés.\n";
    return 0;
}
