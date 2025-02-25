#include "../../../sous_projet_modelisation2D/sous_projet_particules_non_ponctuelles/reseau.h"
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
    reseau.afficher();

    return reseau;  // Retourne le réseau généré
}

void testSubdivisionMultiple( int N ){
    std::cout << "\nInstanciation du reseau...\n";
    Reseau reseau(0.0, 55.0, 0.0, 50.0, 10.0);
    std::cout << "Affichage du reseau...\n";
    reseau.afficher();

    std::cout << "\n=== Test subdivision multiple N=" << N <<" ===\n";
    for (int it = 0; it < N ; ++it){
        std::cout << "Tirage d'une case libre...\n";
        Case* case_tiree = reseau.tirerCaseLibre();
        if (case_tiree) {
        case_tiree->afficher();
        std::cout << "Subdivision de la case tiree...\n";
        reseau.subdiviserCase(case_tiree);
        }
        else { 
            std::cout << "Aucune case libre tirée\n";    
        }
    }
    std::cout << "\nAffichage du reseau...\n";
    reseau.afficher();
    std::cout << "\nAffichage details...\n";
    reseau.afficher_details();
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



int main() {

    std::cout << "\n=== Test classe Reseau ===\n";

    testSubdivisionMultiple( 20 );

    Reseau reseau = genererReseau(60.0, 60.0, 10);
    //exporterReseauCSV(reseau, "test_reseau.csv");

    std::cout << "\n=== Fin des tests de la classe Reseau ===\n";
    return 0;
}