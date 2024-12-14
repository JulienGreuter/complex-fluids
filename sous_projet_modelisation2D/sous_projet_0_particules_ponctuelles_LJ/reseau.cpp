#include "reseau.h"
#include <cstdlib>  // Pour rand()

// Constructeur pour initialiser le réseau
Reseau::Reseau(double xmin, double xmax, double zmin, double zmax, double taille_case)
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
            cases_libres.push_back(&cases[i][j]);  // Ajouter toutes les cases initialement à cases_libres
        }
    }
}

// Méthode pour tirer une case libre aléatoire
Case* Reseau::tirerCaseLibre() {
    if (cases_libres.empty()) {
        return nullptr;  // Si aucune case libre, on retourne nullptr
    }

    int index = rand() % cases_libres.size();
    Case* case_choisie = cases_libres[index];

    // Marquer la case choisie comme occupée
    case_choisie->est_libre = false;

    // Retirer la case choisie de l'ensemble des cases libres
    retirerCaseLibre(case_choisie);

    return case_choisie;
}

// Méthode pour subdiviser une case libre en 4 enfants
void Reseau::subdiviserCase(Case* case_a_subdiviser) {
    if (case_a_subdiviser != nullptr && case_a_subdiviser->estLibre()) {
        case_a_subdiviser->subdiviser();  // Subdiviser la case

        // Ajouter les enfants de la case parent à l'ensemble des cases libres
        ajouterEnfantsCasesLibres(case_a_subdiviser);
    }
}

// Méthode pour retirer une case de l'ensemble des cases libres
void Reseau::retirerCaseLibre(Case* case_a_retirer) {
    auto it = std::find(cases_libres.begin(), cases_libres.end(), case_a_retirer);
    if (it != cases_libres.end()) {
        cases_libres.erase(it);
    }
}

// Méthode pour ajouter les enfants d'une case parent à cases_libres
void Reseau::ajouterEnfantsCasesLibres(Case* case_parent) {
    for (auto enfant : case_parent->enfants) {
        cases_libres.push_back(enfant);
    }
    // Retirer la case parent de l'ensemble des cases libres
    retirerCaseLibre(case_parent);
}
