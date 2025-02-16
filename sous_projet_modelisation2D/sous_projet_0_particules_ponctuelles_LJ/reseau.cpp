#include "reseau.h"
#include <random>  // Pour std::random_device, std::mt19937 et std::uniform_int_distribution
#include <algorithm> // Pour std::find
#include <map>  // Pour stocker les statistiques par ordre

// Constructeur
Reseau::Reseau(double xmin, double xmax, double zmin, double zmax, double taille_case)
    : xmin(xmin), xmax(xmax), zmin(zmin), zmax(zmax), taille_case(taille_case) {
    
    // Calcul du nombre de cases en x et z
    n_lignes = static_cast<int>((xmax - xmin) / taille_case);
    n_colonnes = static_cast<int>((zmax - zmin) / taille_case);

    // Ajustement pour centrer les cases
    double delta_x = xmax - xmin - n_lignes * taille_case;
    double delta_z = zmax - zmin - n_colonnes * taille_case;
    double decalage_x = delta_x / 2.0;
    double decalage_z = delta_z / 2.0;

    // Initialisation du réseau de cases
    cases.resize(n_lignes);
    for (int i = 0; i < n_lignes; ++i) {
        cases[i].resize(n_colonnes);
        for (int j = 0; j < n_colonnes; ++j) {
            double pos_x = xmin + i * taille_case + decalage_x + taille_case / 2.0;
            double pos_z = zmin + j * taille_case + decalage_z + taille_case / 2.0;

            // Création de la case avec unique_ptr
            cases[i][j] = std::make_unique<Case>(pos_x, pos_z, taille_case);
            cases_libres.push_back(cases[i][j].get());  // Ajout à la liste des cases libres
        }
    }
}

// Tirer une case libre aléatoire
Case* Reseau::tirerCaseLibre() {
    if (cases_libres.empty()) {
        std::cerr << "Erreur : Aucune case libre !\n";
        return nullptr;  // Aucune case libre
    }

    // Génèrateur de nombres aléatoires
    std::random_device rd;           // Fournit une source aléatoire basée sur le matériel (si disponible)
    std::mt19937 gen(rd());          // Générateur Mersenne Twister, initialisé avec la graine aléatoire
    // Distribution uniforme pour obtenir un index valide dans le tableau
    std::uniform_int_distribution<> dis(0, cases_libres.size() - 1); // Distribution uniforme entre 0 et cases_libres.size() - 1
    // Tirer un nombre aléatoire pour l'index
    int index = dis(gen);  // Utilise le générateur pour obtenir un index valide
    Case* case_choisie = cases_libres[index];

    return case_choisie;
}

// Subdiviser une case libre en 4 enfants
void Reseau::subdiviserCase(Case* case_a_subdiviser) {
    if (case_a_subdiviser != nullptr && case_a_subdiviser->estLibre()) {
        case_a_subdiviser->subdiviser();
        ajouterEnfantsCasesLibres(case_a_subdiviser);
    }
    else {
        std::cerr << "Erreur : subdivision impossible ( nullptr ou case occupée ou case déjà subdivisée )\n";
        case_a_subdiviser->afficher();
    }
}

// Methode pour subdiviser toutres les cases libres d'un ordre de subdivisation donné
void Reseau::subdiviser(int ordre) {
    // Créer une copie temporaire des cases à subdiviser
    std::vector<Case*> cases_a_subdiviser;
    
    for (auto* c : cases_libres) {  // Utilisation de *c pour éviter les erreurs
        if (c && ordre == c->getOrdreSubdivision()) {  
            cases_a_subdiviser.push_back(c);
        }
    }

    // On subdivise en dehors de la boucle d'itération sur cases_libres
    for (auto* c : cases_a_subdiviser) {
        subdiviserCase(c);
    }
}

// Retirer une case de l'ensemble des cases libres
void Reseau::retirerCaseLibre(Case* case_a_retirer) {
    auto it = std::find(cases_libres.begin(), cases_libres.end(), case_a_retirer);
    if (it != cases_libres.end()) {
        cases_libres.erase(it);
        case_a_retirer->marquerOccupee();
    }
    else {
        std::cerr << "Erreur : la case n'est pas trouvée dans cases_libres";
        case_a_retirer->afficher();
    }
}

// Ajouter les enfants d'une case parent à cases_libres
void Reseau::ajouterEnfantsCasesLibres(Case* case_parent) {
    for (const auto& enfant : case_parent->getEnfants()) {
        cases_libres.push_back(enfant.get());  // Ajoute les enfants en tant que pointeurs bruts
    }
    retirerCaseLibre(case_parent);
}

// Afficher le réseau
void Reseau::afficher() const {
    std::cout << "=== Reseau ===" << std::endl;
    std::cout << "Taille: " << n_lignes << "x" << n_colonnes << std::endl;
    std::cout << "Cases libres: " << cases_libres.size() << std::endl;
    for (const auto& row : cases) {
        for (const auto& cell : row) {
            cell->afficher();
        }
    }
}

void Reseau::afficher_details() const {
    std::map<int, std::tuple<int, int, int>> stats;  
    // {ordre : (cases libres, cases occupées, cases occupées non parentes)}

    // Fonction récursive pour parcourir les cases et remplir les stats
    std::function<void(const Case&)> compterCases = [&](const Case& c) {
        int ordre = c.getOrdreSubdivision();
        if (stats.find(ordre) == stats.end()) {
            stats[ordre] = {0, 0, 0};  // Initialisation à (0, 0, 0)
        }

        auto& [libres, occupees, occupees_non_parents] = stats[ordre];

        if (c.estLibre()) {
            libres++;  // Case libre
        } else {
            occupees++;  // Case occupée
            if (!c.estParent()) {
                occupees_non_parents++;  // Case occupée non parente
            }
        }

        for (const auto& enfant : c.getEnfants()) {
            compterCases(*enfant);
        }
    };

    // Parcourir toutes les cases racines du réseau
    for (const auto& row : cases) {
        for (const auto& cell : row) {
            compterCases(*cell);
        }
    }

    // Affichage des statistiques
    std::cout << "=== Statistiques du Réseau ===\n";
    std::cout << "Réseau (xmin=" << xmin << ", xmax=" << xmax 
              << ", zmin=" << zmin << ", zmax=" << zmax 
              << ", taille_case=" << taille_case << ")\n";

    for (const auto& [ordre, counts] : stats) {
        int libres, occupees, occupees_non_parents;
        std::tie(libres, occupees, occupees_non_parents) = counts;

        std::cout << std::string(ordre * 2, ' ')  // Indentation
                  << "├── Ordre " << ordre << " : " 
                  << (libres + occupees) << " cases "
                  << "(Libres: " << libres 
                  << " | Occupées: " << occupees 
                  << " | Occupées non parentes: " << occupees_non_parents << ")\n";
    }
}