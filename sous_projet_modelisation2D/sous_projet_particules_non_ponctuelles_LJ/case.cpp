#include "case.h"

// Constructeur
Case::Case(double x, double z, double taille_case, int ordre_subdivision, bool est_libre)
    : x(x), z(z), ordre_subdivision(ordre_subdivision), taille_case(taille_case), est_libre(est_libre) {}

// Méthode pour subdiviser une case en 4 enfants
void Case::subdiviser() {
    if (!enfants.empty()) {
        std::cerr << "Erreur : la case est déjà subdivisée !\n";
        return;
    }

    double nouvelle_taille = taille_case / 2.0;

    enfants.push_back(std::make_unique<Case>(x - nouvelle_taille / 2, z - nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));
    enfants.push_back(std::make_unique<Case>(x + nouvelle_taille / 2, z - nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));
    enfants.push_back(std::make_unique<Case>(x - nouvelle_taille / 2, z + nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));
    enfants.push_back(std::make_unique<Case>(x + nouvelle_taille / 2, z + nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));
}

// Affichage des informations de la case
void Case::afficher() const {
    std::cout << "Case (" << x << ", " << z << "), Taille: " << taille_case
              << ", Ordre: " << ordre_subdivision
              << ", " << (est_libre ? "Libre" : "Occupée") << "\n";
    if (estParent()) {
        std::cout << "Cette case a " << enfants.size() << " enfants.\n";
    }
}
