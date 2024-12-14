#include "reseau.h"
#include <cstdlib>  // Pour rand()

// Constructeur pour initialiser le réseau
Réseau::Réseau(double xmin, double xmax, double zmin, double zmax, double taille_case)
    : xmin(xmin), xmax(xmax), zmin(zmin), zmax(zmax), taille_case(taille_case) {

    // Calcul du nombre de cases en x et z
    n_lignes = static_cast<int>((xmax - xmin) / taille_case);
    n_colonnes = static_cast<int>((zmax - zmin) / taille_case);

    // Décalage pour centrer les cases dans le domaine
    double delta_x = xmax - xmin - n_lignes * taille_case;
    double delta_z = zmax - zmin - n_colonnes * taille_case;
    double decalage_x = delta_x / 2.0;
    double decalage_z = delta_z / 2.0;

    // Initialiser le réseau de cases
    cases.resize(n_lignes, std::vector<Case>(n_colonnes, Case(0, 0, taille_case)));

    // Positionner les cases avec les coordonnées du centre de chaque case
    for (int i = 0; i < n_lignes; ++i) {
        for (int j = 0; j < n_colonnes; ++j) {
            double pos_x = xmin + i * taille_case + decalage_x + taille_case / 2.0;
            double pos_z = zmin + j * taille_case + decalage_z + taille_case / 2.0;
            cases[i][j] = Case(pos_x, pos_z, taille_case);
        }
    }
}

// Méthode pour tirer une case libre aléatoire
Case* Réseau::tirerCaseLibre() {
    std::vector<Case*> cases_libres;
    for (int i = 0; i < n_lignes; ++i) {
        for (int j = 0; j < n_colonnes; ++j) {
            if (cases[i][j].estLibre()) {
                // Si la case est libre et n'a pas été subdivisée, on l'ajoute à la liste
                cases_libres.push_back(&cases[i][j]);
            } else if (cases[i][j].estParent()) {
                // Si la case est un parent, tirer une case parmi ses enfants
                for (auto enfant : cases[i][j].enfants) {
                    if (enfant->estLibre()) {
                        cases_libres.push_back(enfant);
                    }
                }
            }
        }
    }

    if (!cases_libres.empty()) {
        int index = rand() % cases_libres.size();
        Case* case_choisie = cases_libres[index];
        // Marquer la case comme occupée
        case_choisie->est_libre = false;
        return case_choisie;
    } else {
        return nullptr;  // Aucune case libre disponible
    }
}

// Méthode pour subdiviser une case libre en 4 enfants
void Réseau::subdiviserCase(Case* case_a_subdiviser) {
    if (case_a_subdiviser != nullptr && case_a_subdiviser->estLibre()) {
        case_a_subdiviser->subdiviser();
    }
}

// Méthode pour libérer la mémoire des cases occupées après subdivision
void Réseau::libererCasesOccupees() {
    for (int i = 0; i < n_lignes; ++i) {
        for (int j = 0; j < n_colonnes; ++j) {
            if (!cases[i][j].estLibre()) {
                // Si la case est occupée, libérer la mémoire de ses enfants (si subdivisée)
                if (cases[i][j].estParent()) {
                    for (auto enfant : cases[i][j].enfants) {
                        delete enfant;  // Libération de la mémoire des enfants
                    }
                    cases[i][j].enfants.clear();  // Effacer le vecteur d'enfants
                }
            }
        }
    }
}