#include "fluidecomplexe.h"
#include <cmath>
#include <iostream>

// Constante de Boltzmann (en unités SI : J/K)
const double K_B = 1.380649e-23;

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









// Constructeur

// Méthode d'initialisation des positions et vitesses
    
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
    