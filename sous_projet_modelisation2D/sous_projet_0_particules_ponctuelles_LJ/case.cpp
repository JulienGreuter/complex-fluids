#include "case.h"

// Constructeur
Case::Case(double x, double z, double taille_case, int ordre_subdivision, bool est_libre)
    : x(x), z(z), ordre_subdivision(ordre_subdivision), taille_case(taille_case), est_libre(est_libre) {}

// Destructeur : libère la mémoire des enfants pour éviter les fuites mémoire
Case::~Case() {
    for (auto enfant : enfants) {
        delete enfant;  // Libère chaque enfant alloué dynamiquement
    }
    enfants.clear();  // Vide le vecteur d'enfants
}

// Méthode pour subdiviser une case en 4 enfants, mais uniquement si la case est libre
void Case::subdiviser() {
    if (est_libre) {  // Vérifie si la case est libre avant de subdiviser
        // Libérer les anciens enfants s'il y en a déjà          //INUTILE ?
        for (auto enfant : enfants) {
            delete enfant;
        }
        enfants.clear();

        // Créer 4 enfants avec une taille de case réduite de moitié
        double nouvelle_taille = taille_case / 2.0;

        enfants.push_back(new Case(x - nouvelle_taille / 2, z - nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));
        enfants.push_back(new Case(x + nouvelle_taille / 2, z - nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));
        enfants.push_back(new Case(x - nouvelle_taille / 2, z + nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));
        enfants.push_back(new Case(x + nouvelle_taille / 2, z + nouvelle_taille / 2, nouvelle_taille, ordre_subdivision + 1));

        // La case devient un parent après la subdivision
        
    }
}

// Méthode pour vérifier si la case est une case parent
bool Case::estParent() const {
    return !enfants.empty();
}

// Méthode pour vérifier si la case est libre
bool Case::estLibre() const {
    return est_libre;
}

// Méthode pour marquer la case comme occupée
void Case::marquerOccupee() {
    est_libre = false;
}
