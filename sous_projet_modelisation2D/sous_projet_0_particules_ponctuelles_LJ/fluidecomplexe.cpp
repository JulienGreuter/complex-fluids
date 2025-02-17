#include <algorithm>  // Pour std::sort
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include "fluidecomplexe.h"



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

    return r_ij * factor;                         // Retourne la force de Lennard-Jones sous forme de Vec2
}



// Constructeur

FluideComplexe::FluideComplexe(double L_x, double L_z, double delta_t, double kappa, double tau_P, double tau_T, double r_c, const std::string fichier_nom) 
: L_x(L_x), L_z(L_z), delta_t(delta_t), kappa(kappa), tau_P(tau_P), tau_T(tau_T), r_c(r_c), fichier_nom(fichier_nom) {

    // Initialiser la table des domaines
    domaines = {
        {"D1",  std::make_tuple(-L_x / 2, L_x / 2, -L_z / 2, L_z / 2)},
        {"D2",  std::make_tuple(-L_x / 2, L_x / 2, -L_z / 4, L_z / 4)},
        {"D31", std::make_tuple(-L_x / 2, L_x / 2, -L_z / 2, -L_z / 4)},
        {"D32", std::make_tuple(-L_x / 2, L_x / 2, -L_z / 4, L_z / 4)},
        {"D33", std::make_tuple(-L_x / 2, L_x / 2, L_z / 4, L_z / 2)}
    };
}

void FluideComplexe::initialisation_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire) {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Appel de initialisation_domaine()\n";
    std::cout << "domaine :" << domaine << std::endl;
    if (domaines.find(domaine) == domaines.end()) {
        std::cerr << "Erreur : Domaine non reconnu : " << domaine << std::endl;
        return;
    }
    std::cout << "Correspond à : ";
    auto [xmin, xmax, zmin, zmax] = domaines[domaine];
    std::cout << "xmin = " << xmin << " | xmax = " << xmax << " | zmin = " << zmin << " | zmax = " << zmax << std::endl;

    if (vecteur_intermediaire.empty()) {
        std::cerr << "Erreur : Aucun ensemble de particules fourni pour le domaine " << domaine << std::endl;
        return;
    }

    double taille_case = vecteur_intermediaire[0].taille > 0 ? vecteur_intermediaire[0].taille : vecteur_intermediaire[0].d;
    int ordre_subdivision = 0;
    std::cout << "taille_case = " << taille_case << std::endl;
    Reseau reseau(xmin, xmax, zmin, zmax, taille_case);

    for (auto& ensemble : vecteur_intermediaire) {
        double taille_ensemble = ensemble.taille > 0 ? ensemble.taille : ensemble.d;
        std::cout << "taille_ensemble = " << taille_ensemble << std::endl;
        while (taille_ensemble < taille_case / 2.0) {
            std::cout << "  Subdivision du reseau...\n";
            
            reseau.subdiviser(ordre_subdivision);
            
            taille_case /= 2.0;
            ordre_subdivision += 1;
        }
        std::cout << "  Remplissage du reseau...\n";
        for (int i = 0; i < ensemble.N; ++i) {
            Case* case_libre = reseau.tirerCaseLibre();
            if (!case_libre) {
                std::cerr << "Erreur : Plus de cases libres disponibles pour le domaine " << domaine << std::endl;
                return;
            }
            ensemble.positions.push_back(Vec2(case_libre->getX(), case_libre->getZ()));
            reseau.retirerCaseLibre(case_libre);
        }
    }
    // visualisation du reseau 
    reseau.afficher_details();
    reseau.exporterCSV(domaine);
    for (auto& ensemble : vecteur_intermediaire) {
        ensemble.initialiserVitesses(T);
    }
    std::cout << "Fin de l'initialisation du domaine " << domaine << std::endl;
    std::cout << "--------------------------------------------------\n";
}

void FluideComplexe::traiter_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire) {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Appel de traiter_domaine()\n";
    if (vecteur_intermediaire.empty()) return; // Éviter un traitement inutile

    // Trier les particules en fonction de leur taille ou d si taille = 0
    std::cout << "Trie du vecteur intermediaire...\n";
    std::sort(vecteur_intermediaire.begin(), vecteur_intermediaire.end(), [](const Particules& a, const Particules& b) {
        return (a.taille > 0 ? a.taille : a.d) > (b.taille > 0 ? b.taille : b.d);
    });
    // Initialiser le domaine avec les particules triées
    initialisation_domaine(T, domaine, vecteur_intermediaire);
    // Ajouter les particules traitées à l'attribut principal
    particules.insert(particules.end(), vecteur_intermediaire.begin(), vecteur_intermediaire.end());
    // Vider le vecteur pour le prochain domaine
    vecteur_intermediaire.clear();
    std::cout << "Fin de traiter_domaine()\n";
    std::cout << "--------------------------------------------------\n";
}

void FluideComplexe::initialisation(double T) { 
    std::cout << "Initialisation...\n";
    std::string ligne;
    std::vector<Particules> vecteur_intermediaire;
    std::string dernier_domaine = "";
    std::cout << "Lecture du fichier d'initialisation...\n";
    std::ifstream fichier_initialisation(fichier_nom);

    while (std::getline(fichier_initialisation, ligne)) {
        if (ligne.empty() || ligne[0] == '#') continue;

        int N;
        double E_0, d, masse, taille, charge;
        std::string id_ini;
        std::istringstream iss(ligne);

        if (!(iss >> N >> E_0 >> d >> masse >> taille >> charge >> id_ini)) {
            std::cerr << "Erreur : Format de ligne incorrect : " << ligne << std::endl;
            continue;
        }
        std::cout << "Instanciation d'un ensemble de particules...\n";
        Particules ensemble(N, E_0, d, masse, taille, charge);
        std::cout << "N = " << N << " | E_0 = " << E_0 << " | d = " << d << " | masse = " << masse << " | taille = " <<taille << " | charge = " << charge << " | domaine : " << id_ini << std::endl;

        if (id_ini != dernier_domaine && !dernier_domaine.empty()) {
            traiter_domaine(T, dernier_domaine, vecteur_intermediaire);
        }

        dernier_domaine = id_ini;
        vecteur_intermediaire.push_back(ensemble);
    }

    // Traiter le dernier domaine après la lecture du fichier
    traiter_domaine(T, dernier_domaine, vecteur_intermediaire);
    std::cout << "Fin de L'initialisation\n";
}

// Méthode pour calculer les forces d'interactions entre les particules
void FluideComplexe::calculer_forces() {
    // Nettoyer le vecteur des forces d'interaction avant de recalculer
    forces_interactions.clear();

    // Vecteurs de translation pour appliquer les conditions périodiques
    Vec2 ux(1.0, 0.0);  // Unité en x
    Vec2 uz(0.0, 1.0);  // Unité en z

    std::vector<Vec2> vecteurs_periodiques = {
        Vec2(0.0, 0.0),                    // Vecteur nul (pas de translation)
        ux * L_x ,                         // Translation +L_x en x
        uz * L_z ,                         // Translation +L_z en z
        ux * -L_x ,                        // Translation -L_x en x
        uz * -L_z ,                        // Translation -L_z en z
        ux * L_x + uz * - L_z ,            // Translation +L_x et -L_z
        ux * L_x  + uz * L_z ,             // Translation +L_x et +L_z
        ux * -L_x  + uz* L_z ,             // Translation -L_x et +L_z
        ux * -L_x + uz * - L_z             // Translation -L_x et -L_z
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
    