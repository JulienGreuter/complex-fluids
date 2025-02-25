#include "../../../sous_projet_modelisation2D/sous_projet_particules_non_ponctuelles/reseau.h"
#include <iostream>
#include <string>
#include <functional> // Pour std::function
#include <fstream>    // Pour std::ofstream

Reseau genererReseau( double xmax, double zmax, int N ) {
    double taille_case = xmax/3.0;
    std::cout << "\nInstanciation du reseau...\n";
    Reseau reseau(-xmax, xmax, -zmax, zmax, taille_case);

    // Subdiviser quelques cases aléatoirement
    std::cout << "Subdivision du reseau...\n";
    for (int i = 0; i < N; ++i) {
        Case* case_tiree = reseau.tirerCaseLibre();
        if (case_tiree) {
            reseau.subdiviserCase(case_tiree);
        }
    }
    reseau.afficher_details();

    return reseau;  // Retourne le réseau généré
}

void exporterReseauCSV(const Reseau& reseau, const std::string& filename) {
    std::cout << "Exportation du reseau sous CSV...\n";
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Erreur : Impossible d'écrire dans " << filename << std::endl;
        return;
    }

    file << "x,z,taille,ordre\n";  // En-tête CSV

    // Fonction récursive pour exporter toutes les cases
    std::function<void(const Case&)> exporterCase = [&](const Case& c) {
        file << c.getX() << "," << c.getZ() << "," << c.getTaille() << "," << c.getOrdreSubdivision() << "\n";

        for (const auto& enfant : c.getEnfants()) {
            exporterCase(*enfant);
        }
    };

    // Parcourir toutes les cases parents et exporter
    for (const auto& row : reseau.getCases()) {
        for (const auto& cell : row) {
            exporterCase(*cell);
        }
    }

    file.close();
    std::cout << "Exportation terminée : " << filename << std::endl;
}

int main(int argc, char* argv[]) {
    // Vérifier que le nom du fichier est fourni
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <nom_fichier.csv>" << std::endl;
        return 1;
    }

    std::string nom_fichier = argv[1];  // Nom du fichier passé en argument
    int nb_subdivisions = 10;  // Nombre fixe de subdivisions (modifiable ici)

    // Générer le réseau et exporter en CSV
    Reseau reseau = genererReseau(60.0, 60.0, nb_subdivisions);
    exporterReseauCSV(reseau, nom_fichier);

    std::cout << "Fichier généré : " << nom_fichier << " (" << nb_subdivisions << " subdivisions)" << std::endl;
    return 0;
}