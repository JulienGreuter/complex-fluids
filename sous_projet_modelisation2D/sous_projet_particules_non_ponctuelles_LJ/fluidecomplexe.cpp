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

// Méthode d'initialisation des positions et vitesses d'un domaine du fluide complexe
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
    reseau.afficher_details(); // decommenter c.afficher() dans reseau.cpp si besoin;
    //reseau.exporterCSV(domaine);
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

// Méthode d'initialisation des positions et vitesses
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
    exporterPositionsCSV("positions_ini.csv");
    exporterVitessesCSV("vitesses_ini.csv");
    // Initialisation de forces_interactions
    calculer_forces();
}

// Méthode pour calculer les forces d'interactions entre les particules
void FluideComplexe::calculer_forces() {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Appel de calculer_forces()\n";
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
        std::cout << "Calcul des forces d'interaction d'un ensemble de particules...\n";
        // Boucle sur chaque position i dans l'ensemble de particules i
        for (size_t i = 0; i < ensemble_i.positions.size(); ++i) {
            int k = 0;
            Vec2 position_i = ensemble_i.positions[i];

            // Initialiser la force totale subie par la particule i
            Vec2 force_totale(0.0, 0.0);

            // Boucle sur chaque ensemble de particules j
            for (auto& ensemble_j : particules) {
                // Boucle sur chaque position j dans l'ensemble de particules j
                for (size_t j = 0; j < ensemble_j.positions.size(); ++j) {
                    Vec2 position_j = ensemble_j.positions[j];

                    // Évite le calcul de la force avec soi-même
                    if ( (position_i-position_j).norme() < 1e-15 ) {
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
                            k += 1;
                            f_ij = force_LJ(ensemble_i.E_0, ensemble_j.E_0, ensemble_i.d, ensemble_j.d, r_ij);
                            break;  // Par construction (r_c correctement choisi), une particule i n'interagit qu'avec une des 9 particules j 
                        }
                    }

                    // Ajouter la force d'interaction f_ij à la force totale
                    force_totale += f_ij;
                }
            }
            //std::cout << k << " particules intergissant sur la particule i : fi = "; force_totale.afficher();
            // Ajouter la force totale pour cette particule au vecteur des forces d'interactions
            forces_interactions.push_back(force_totale);
        }
    }
    std::cout << "Fin de calculer_forces()\n";
    std::cout << "--------------------------------------------------\n";
}

// Méthode pour mettre à jour les positions des particules
void FluideComplexe::mettre_a_jour_positions(double P) {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Mise à jour des positions des particules...\n";
    std::cout << "Calcul des positions des particules à l'instant suivant...\n";

    size_t index = 0; // Indice pour parcourir forces_interactions

    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        for (size_t i = 0; i < ensemble.positions.size(); ++i) {
            // Calcul des nouvelles positions
            ensemble.positions[i] += ensemble.vitesses[i] * delta_t  
                                  + forces_interactions[index] * (std::pow(delta_t, 2) / (2.0 * ensemble.masse));

            // Incrémentation de l'index pour correspondre à la bonne force
            index++;
        }
    }
    // Application des conditions periodiques
    appliquer_conditions_periodiques();
    // Aplication du barostat pour tenir compte de la pression fixée P, modifie les positions, L_x et L_z
    //appliquer_barostat(P);
    double Pxx = calculer_tenseur_pression(0,0,L_z,0);
    double Pzz = calculer_tenseur_pression(1,1,L_z,0);
    std::cout << "Pxx = " << Pxx << " Pa\n";
    std::cout << "Pzz = " << Pzz << " Pa\n";

    std::cout << "Fin de mise à jour des positions.\n";
    std::cout << "--------------------------------------------------\n";
}

// Méthode pour mettre à jour les vitesses des particules
void FluideComplexe::mettre_a_jour_vitesses(double T, const std::vector<Vec2>& forces_interactions_precedentes) {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Mise à jour des vitesses des particules...\n";
    std::cout << "Calcul des vitesses des particules à l'instant suivant...\n";

    size_t index = 0; // Indice pour parcourir forces_interactions

    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        for (size_t i = 0; i < ensemble.vitesses.size(); ++i) {
            // Calcul des nouvelles vitesses
            ensemble.vitesses[i] +=  (forces_interactions_precedentes[index] + forces_interactions[index])* (delta_t / (2.0 * ensemble.masse));

            // Incrémentation de l'index pour correspondre à la bonne force
            index++;
        }
    }
    // Aplication du thermostat pour tenir compte de la temperature fixée T, modifie les vitesses
    appliquer_thermostat(T);

    std::cout << "Fin de mise à jour des vitesses.\n";
    std::cout << "--------------------------------------------------\n";
}
// Méthode pour appliquer les conditions périodiques
void FluideComplexe::appliquer_conditions_periodiques() {
    std::cout << "Application des conditions périodiques...\n";
    int k = 0;
    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        for (auto& position : ensemble.positions) {
            bool modifie = false;
            // Correction de x si nécessaire
            if (position.x > L_x / 2.0 || position.x < -L_x / 2.0) {
                position.x = std::fmod(position.x + L_x / 2.0, L_x) - L_x / 2.0;
                modifie = true;
            }
            // Correction de z si nécessaire
            if (position.z > L_z / 2.0 || position.z < -L_z / 2.0) {
                position.z = std::fmod(position.z + L_z / 2.0, L_z) - L_z / 2.0;
                modifie = true;
            }
    
            // Incrémente k uniquement si une modification a eu lieu
            if (modifie) {
                k += 1;
            }
        }
    }
    std::cout << "Modification de : " << k << " particules\n";
}

// Méthode pour appliquer le thermostat en fonction de la température T
void FluideComplexe::appliquer_thermostat(double T) {
    std::cout << "Calcul de T...\n";
    double T_mes = calculer_temperature();
    std::cout << "T = " << T_mes << " K\n";
    double lambda = std::sqrt( 1 + delta_t * (T/T_mes -1) / tau_T );
    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        for (auto& vitesse : ensemble.vitesses) {
            vitesse = vitesse * lambda;
        }
    }
    std::cout << "Thermostat appliqué\n";
}

// Méthode pour appliquer le barostat en fonction de la pression P
void FluideComplexe::appliquer_barostat(double P) {
    std::cout << "Calcul de P...\n";
    double P_mes = (calculer_tenseur_pression(0,0,L_z,0) + calculer_tenseur_pression(1,1,L_z,0)) / 2.0;
    std::cout << "P = " << P_mes << " Pa\n";
    double lambda = 1 - kappa * delta_t * (P - P_mes) / tau_P;
    L_x = std::pow(lambda, 1/3.0) * L_x;
    L_z = std::pow(lambda, 1/3.0) * L_z;

    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        for (auto& position : ensemble.positions) {
            position = position * std::pow(lambda, 1/3.0);
        }
    }
    std::cout << "Barostat appliqué.\n";
}

// Méthode pour calculer la température du fluide  
double FluideComplexe::calculer_temperature() const {
    double T_mes = 0;
    int N_tot = 0; // nombre totale de particules
    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        N_tot += ensemble.N;
        for (auto& vitesse : ensemble.vitesses) {
            T_mes += ensemble.masse * std::pow(vitesse.norme(), 2); // 2 -> 3 pour essayer de tenir compte du passage 3D -> 2D
        }
    }
    T_mes /= (2 * N_tot * K_B);
    return T_mes;
}

// Méthode pour calculer le tenseur de pression sur une tranche donnée
double FluideComplexe::calculer_tenseur_pression(int alpha, int beta, double Delta_z, double z_k) const {
    // Delta_z : largeur de la tranche | z_k milieu de la tranche
    // alpha = 0 -> x | alpha = 1 -> z | beta = 0 -> x | beta = 1 -> z
    double P_mes = 0;
    int index =0;
    // A IMPLEMENTER : version provisoire Pxx et Pzz sur totalité du fluide + formule incorrecte
    if (alpha ==0 && beta ==0) {
        // Parcours de chaque ensemble de particules
        for (auto& ensemble : particules) {
            for (size_t i = 0; i < ensemble.vitesses.size(); ++i) {
              P_mes += ( ensemble.masse * ensemble.vitesses[i].x * ensemble.vitesses[i].x
                    + forces_interactions[index].x * ensemble.positions[i].x ) / (L_x * L_z * ensemble.d); // on divise par d pour essayer de tenir compte du passage 3D -> 2D

                // Incrémentation de l'index pour correspondre à la bonne force
                index++;
            }
        }
    }
    if (alpha ==1 && beta ==1) {
        // Parcours de chaque ensemble de particules
        for (auto& ensemble : particules) {
            for (size_t i = 0; i < ensemble.vitesses.size(); ++i) {
              P_mes += ( ensemble.masse * ensemble.vitesses[i].z * ensemble.vitesses[i].z 
                    + forces_interactions[index].z * ensemble.positions[i].z ) / (L_x * L_z * ensemble.d); // on divise par d pour essayer de tenir compte du passage 3D -> 2D

                // Incrémentation de l'index pour correspondre à la bonne force
                index++;
            }
        }
    }
    return P_mes;
}

// Méthode pour faire évoluer le système vers l'état suivant
void FluideComplexe::evoluer(double T, double P) {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Evolution du fluide complexe...\n";
    std::vector<Vec2> forces_interactions_precedentes = forces_interactions;  // Forces d'interactions des particules à t
    mettre_a_jour_positions(P);
    calculer_forces();
    mettre_a_jour_vitesses(T, forces_interactions_precedentes);
    std::cout << "Fin de l'evolution du fluide complexe.\n";
    std::cout << "--------------------------------------------------\n";
}

// Méthode pour exporter les positions des particules sous CSV
void FluideComplexe::exporterPositionsCSV(const std::string& fileCSV) const {
    std::cout << "Exportation des positions des particules...\n";
    std::ofstream fichier(fileCSV);
    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << fileCSV << std::endl;
        return;
    }
    fichier << "x,z,taille\n";
    for (const auto& ensemble : particules) {
        if (ensemble.positions.empty()) continue; // Ignorer les ensembles vides
        
        // Écriture de l'en-tête
        fichier << "# Ensemble de particules : N = " << ensemble.N
                << " | d = " << ensemble.d
                << " | E_0 = " << ensemble.E_0
                << " | taille = " << ensemble.taille 
                << " | masse = " << ensemble.masse << "\n";
        
        double taille_particule = ensemble.taille;
        for (const auto& position : ensemble.positions) {
            fichier << position.x << "," << position.z << "," << taille_particule << "\n";
        }
        fichier << "\n\n";
    }

    std::cout << "Exportation vers " << fileCSV << " réussie." << std::endl;
}
 
// Méthode pour exporter les vitesses des particules sous CSV
void FluideComplexe::exporterVitessesCSV(const std::string& fileCSV) const {
    std::cout << "Exportation des vitesses des particules...\n";
    std::ofstream fichier(fileCSV);
    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << fileCSV << std::endl;
        return;
    }
    fichier << "vx,vz\n";
    for (const auto& ensemble : particules) {
        if (ensemble.vitesses.empty()) continue; // Ignorer les ensembles vides
        
        // Écriture de l'en-tête
        fichier << "# Ensemble de particules : N = " << ensemble.N
                << " | d = " << ensemble.d
                << " | E_0 = " << ensemble.E_0
                << " | taille = " << ensemble.taille 
                << " | masse = " << ensemble.masse << "\n";
        
        for (const auto& vitesse : ensemble.vitesses) {
            fichier << vitesse.x << "," << vitesse.z << "\n";
        }
        fichier << "\n\n";
    }

    std::cout << "Exportation vers " << fileCSV << " réussie." << std::endl;
}