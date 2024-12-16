#include "fluidecomplexe.h"
#include "reseau.h"
#include "vec2.h"
#include <algorithm>  // Pour std::sort
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <iostream>

// Constante de Boltzmann (en unités SI : J/K)
const double K_B = 1.380649e-23;

// Fonction pour convertir une chaîne de caractères représentant un domaine en ses limites (xmin, xmax, zmin, zmax)
std::unordered_map<std::string, std::tuple<double, double, double, double>> domaines = {
    {"D1",  std::make_tuple(-L_x / 2, L_x / 2, -L_z / 2, L_z / 2)},
    {"D2",  std::make_tuple(-L_x / 2, L_x / 2, -L_z / 4, L_z / 4)},
    {"D31", std::make_tuple(-L_x / 2, L_x / 2, -L_z / 2, -L_z / 4)},
    {"D32", std::make_tuple(-L_x / 2, L_x / 2, -L_z / 4, L_z / 4)},
    {"D33", std::make_tuple(-L_x / 2, L_x / 2, L_z / 4, L_z / 2)}
};

// Fonction pour calculer la force de Lennard-Jones entre deux particules
Vec2 force_LJ(double E_0i, double E_0j, double di, double dj, Vec2 r_ij) {
    double E_0 = (E_0i + E_0j) / 2.0;             // Paramètre d'énergie moyenne
    double d = std::sqrt(di * dj);                // Paramètre de distance moyenne
    double distance = r_ij.norme();               // Norme de r_ij (distance entre les particules)

    // Évite la division par zéro ou des interactions à très courte distance  //ne devrait pas arriver
    if (distance == 0.0) {
        return Vec2(0.0, 0.0);
    }

    double A = d / distance;                      // Rapport d / distance
    double factor = 24 * E_0 * (2 * std::pow(A, 13) - std::pow(A, 7)) / (d * distance);

    return factor * r_ij;                         // Retourne la force de Lennard-Jones sous forme de Vec2
}

void initialisation_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire) {
    // Obtenir les limites du domaine
    if (domaines.find(domaine) == domaines.end()) {
        std::cerr << "Erreur : Domaine non reconnu." << std::endl;
        return;
    }
    auto [xmin, xmax, zmin, zmax] = domaines[domaine];

    // Déterminer la taille initiale des cases du réseau
    double taille_case = vecteur_intermediaire[0].taille > 0 ? vecteur_intermediaire[0].taille : vecteur_intermediaire[0].d;

    // Créer le réseau pour le domaine spécifié
    Reseau reseau(xmin, xmax, zmin, zmax, taille_case);

    for (size_t k = 0; k < vecteur_intermediaire.size(); ++k) {
        auto& ensemble = vecteur_intermediaire[k];

        // Vérifier si une subdivision est nécessaire avant de placer cet ensemble
        double taille_ensemble = ensemble.taille > 0 ? ensemble.taille : ensemble.d;
        while (taille_ensemble < taille_case / 2.0) {
            // Subdiviser toutes les cases libres
            for (auto* case_libre : reseau.cases_libres) {
                reseau.subdiviserCase(case_libre);
            }

            // Mettre à jour la taille des cases après subdivision
            taille_case /= 2.0;
        }

        // Initialiser les positions des particules de l'ensemble courant
        for (int i = 0; i < ensemble.N; ++i) {
            Case* case_libre = reseau.tirerCaseLibre();
            if (!case_libre) {
                std::cerr << "Erreur : Plus de cases libres disponibles pour le domaine " << domaine << std::endl;
                return;
            }

            ensemble.positions.push_back(Vec2(case_libre->x, case_libre->z));
        }

        // Vérifier si le prochain ensemble nécessite une subdivision
        if (k + 1 < vecteur_intermediaire.size()) {
            double prochaine_taille = vecteur_intermediaire[k + 1].taille;
            double prochain_d = vecteur_intermediaire[k + 1].d;

            if ((prochaine_taille > 0 && prochaine_taille < taille_case / 2.0) || 
                (prochaine_taille == 0 && prochain_d < taille_case / 2.0)) {
                
                // Subdiviser toutes les cases libres
                for (auto* case_libre : reseau.cases_libres) {
                    reseau.subdiviserCase(case_libre);
                }

                // Mettre à jour la taille des cases après subdivision
                taille_case /= 2.0;
            }
        }
    }

    // Initialiser les vitesses des particules en fonction de la température T
    for (auto& ensemble : vecteur_intermediaire) {
        ensemble.initialiserVitesses(T);
    }
}







// Constructeur

// Méthode d'initialisation des positions et vitesses
void FluideComplexe::initialisation(std::ifstream& fichier) {
    std::string ligne;
    std::vector<Particules> vecteur_intermediaire;

    while (std::getline(fichier, ligne)) {
        // Ignorer les commentaires et les lignes vides
        if (ligne.empty() || ligne[0] == '#') continue;

        // Lire les attributs des particules depuis la ligne
        int N;
        double E_0, d, charge, taille, masse;
        std::string id_ini;

        std::istringstream iss(ligne);
        if (!(iss >> N >> E_0 >> d >> charge >> taille >> masse >> id_ini)) {
            std::cerr << "Erreur : Format de ligne incorrect : " << ligne << std::endl;
            continue;
        }

    // Créer une instance de Particules avec les paramètres lus
    Particules ensemble(N, E_0, d, charge, taille, masse);

    // Ajouter l'ensemble au vecteur intermédiaire
    vecteur_intermediaire.push_back(ensemble);

        // Si la fin du fichier ou une ligne vide est atteinte, procéder à l'initialisation du domaine
        if (fichier.eof() || fichier.peek() == '\n') {
            // Trier le vecteur par taille (ou par d si taille == 0)
            std::sort(vecteur_intermediaire.begin(), vecteur_intermediaire.end(), [](const Particules& a, const Particules& b) {
                return a.taille > 0 && b.taille > 0 ? a.taille < b.taille : a.d < b.d;
            });

            // Appeler la fonction d'initialisation pour le domaine spécifié
            initialisation_domaine(T, id_ini, vecteur_intermediaire);

            // Ajouter les ensembles initialisés à l'attribut `particules`
            particules.insert(particules.end(), vecteur_intermediaire.begin(), vecteur_intermediaire.end());

            // Vider le vecteur intermédiaire pour le prochain domaine
            vecteur_intermediaire.clear();
        }
    }
}

// Méthode pour calculer les forces d'interactions entre les particules
void FluideComplexe::calculer_forces() {
    // Nettoyer le vecteur des forces d'interaction avant de recalculer
    forces_interactions.clear();

    // Vecteurs de translation pour appliquer les conditions périodiques
    Vec2 ux(1.0, 0.0);  // Unité en x
    Vec2 uz(0.0, 1.0);  // Unité en z

    std::vector<Vec2> vecteurs_periodiques = {
        Vec2(0.0, 0.0),                   // Vecteur nul (pas de translation)
        L_x * ux,                         // Translation +L_x en x
        L_z * uz,                         // Translation +L_z en z
        -L_x * ux,                        // Translation -L_x en x
        -L_z * uz,                        // Translation -L_z en z
        L_x * ux - L_z * uz,              // Translation +L_x et -L_z
        L_x * ux + L_z * uz,              // Translation +L_x et +L_z
        -L_x * ux + L_z * uz,             // Translation -L_x et +L_z
        -L_x * ux - L_z * uz              // Translation -L_x et -L_z
    };

    // Boucle sur chaque ensemble de particules i
    for (auto& ensemble_i : particules) {
        // Boucle sur chaque position i dans l'ensemble de particules i
        for (size_t i = 0; i < ensemble_i.positions.size(); ++i) {
            Vec2 position_i = ensemble_i.positions[i];

            // Initialiser la force totale subie par la particule i
            Vec2 force_totale(0.0, 0.0);

            // Boucle sur chaque ensemble de particules j
            for (auto& ensemble_j : particules) {
                // Boucle sur chaque position j dans l'ensemble de particules j
                for (size_t j = 0; j < ensemble_j.positions.size(); ++j) {
                    Vec2 position_j = ensemble_j.positions[j];

                    // Évite le calcul de la force avec soi-même
                    if (position_i.x == position_j.x && position_i.z == position_j.z) {
                        continue;
                    }

                    // Initialiser la force d'interaction f_ij
                    Vec2 f_ij(0.0, 0.0);

                    /*Regarde d'abord si la particule_j est assez proche dans la boite de simulation, 
                    puis dans les 8 boites adjacentes. Dans le cas ou r_c << L_x, L_z on sort de la boucle directement 
                    la plupart du temps ( la particules j interragissant est souvent dans la boite principale : vecteur = (0,0)).
                    Il faut dans tout les cas que r_c < L_x, L_z pour eviter des interactions d'une particules avec elle même 
                    dans la boite voisine, et 4r_c < L_x, L_z implique qu'il existe un interieur. 
                    Toutes particules se situant dans l'interieur de la boite, interagissent exclusivement avec des particules 
                    de la boites principales par construction*/
                    for (const auto& vecteur : vecteurs_periodiques) {
                        Vec2 r_ij = position_j - position_i + vecteur;  // Distance avec conditions périodiques

                        // Vérifier si la distance est inférieure au rayon de coupure r_c
                        if (r_ij.norme() < r_c) {
                            f_ij = force_LJ(ensemble_i.E_0, ensemble_j.E_0, ensemble_i.d, ensemble_j.d, r_ij);
                            break;  // Par construction (r_c correctement choisi), une particule i n'interagit qu'avec une des 9 particules j 
                        }
                    }

                    // Ajouter la force d'interaction f_ij à la force totale
                    force_totale += f_ij;
                }
            }

            // Ajouter la force totale pour cette particule au vecteur des forces d'interactions
            forces_interactions.push_back(force_totale);
        }
    }
}


// Méthode pour mettre à jour les positions des particules

// Méthode pour mettre à jour les vitesses des particules
    
// Méthode pour appliquer les conditions périodiques
    
// Méthode pour appliquer le thermostat en fonction de la température T
    
// Méthode pour appliquer le barostat en fonction de la pression P

// Méthode pour calculer la température du fluide  

// Méthode pour calculer le tenseur de pression sur une tranche donnée

// Méthode pour faire évoluer le système vers l'état suivant
    