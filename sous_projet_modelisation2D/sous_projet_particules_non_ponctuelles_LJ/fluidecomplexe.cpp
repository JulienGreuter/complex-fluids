#include <algorithm>  // Pour std::sort
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include "fluidecomplexe.h"
#include <random>
#include <regex>

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
    double factor = -24 * E_0 * (2 * std::pow(A, 13) - std::pow(A, 7)) / (d * distance); // test avec -24 a la place de 24

    return r_ij * factor;                         // Retourne la force de Lennard-Jones sous forme de Vec2
}

// Fonction pour générer une vitesse selon une distribution de Maxwell-Boltzmann
Vec2 maxwellBoltzmannSample(double T, double masse) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    double sigma = std::sqrt(K_B * T / masse); // Écart-type basé sur la température
    std::normal_distribution<double> dist(0.0, sigma);
    return Vec2(dist(gen),dist(gen));
}

// Fonction pour appliquer la relaxation vers Maxwell-Boltzmann
void relaxationVersMaxwell(Vec2& vitesse, double T, double masse, double alpha) {
    vitesse = vitesse * (1-alpha) + maxwellBoltzmannSample(T, masse) * alpha;  
}

// Fonction pour appliquer les conditions périodiques uniquement si nécessaire
double periodic_boundary_if_needed(double coord, double L, bool& modifie) {
    modifie = false;
    if (coord > L / 2.0 || coord < -L / 2.0) {  // Vérifie si la correction est nécessaire
        coord = std::fmod(coord, L);
        if (coord > L / 2.0) {
            coord -= L;
        } else if (coord < -L / 2.0) {
            coord += L;
        }
        modifie = true; //indique si la modification à eu lieu via modifie
    }
    return coord;
}
// Fonction pour lire les en-tetes
bool extraireEnteteEnsemble(const std::string& ligne, int& N, double& d, double& E_0, double& taille, double& masse) {
    static const std::regex regex_ensemble(R"#(# Ensemble de particules : N = (\d+) \| d = ([\d.eE-]+) \| E_0 = ([\d.eE-]+) \| taille = ([\d.eE-]+) \| masse = ([\d.eE-]+))#");

    std::smatch match;
    if (std::regex_search(ligne, match, regex_ensemble)) {
        try {
            N = std::stoi(match[1]);
            d = std::stod(match[2]);
            E_0 = std::stod(match[3]);
            taille = std::stod(match[4]);
            masse = std::stod(match[5]);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Erreur lors de la conversion des valeurs de l'en-tête : " << e.what() << std::endl;
            return false;
        }
    }
    return false;
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
        {"D33", std::make_tuple(-L_x / 2, L_x / 2, L_z / 4, L_z / 2)},

        {"DFG", std::make_tuple(-L_x / 2, L_x / 2, -L_z / 2, -L_z / 20)},
        {"DFM", std::make_tuple(-L_x / 2, L_x / 2, -L_z / 20, L_z / 20)},
        {"DFD", std::make_tuple(-L_x / 2, L_x / 2, L_z / 20, L_z / 20)}
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

void FluideComplexe::initialisationViaCSV(const std::string& filePositions, const std::string& fileVitesses) {
    std::cout << "Initialisation du fluide complexe depuis les fichiers CSV..." << std::endl;
    
    std::ifstream fichierPositions(filePositions);
    std::ifstream fichierVitesses(fileVitesses);
    
    if (!fichierPositions || !fichierVitesses) {
        std::cerr << "Erreur : Impossible d'ouvrir les fichiers CSV fournis." << std::endl;
        return;
    }
    
    std::string ligne;
    std::vector<Particules> ensembles_particules;
    
    /*** LECTURE DU FICHIER POSITIONS ***/
    while (std::getline(fichierPositions, ligne)) {
        if (ligne.empty()) continue;
        
        if (ligne[0] == '#') { // Détection d'un nouvel ensemble
            int N;
            double d, E_0, taille, masse;
            if (extraireEnteteEnsemble(ligne, N, d, E_0, taille, masse)) {
                ensembles_particules.emplace_back(N, E_0, d, masse, taille, 0.0);
                std::cout << "Ensemble chargé : N = " << N << " | d = " << d 
                          << " | E_0 = " << E_0 << " | taille = " << taille 
                          << " | masse = " << masse << std::endl;
            }
        } else if (!ensembles_particules.empty()) { // Ajout des positions
            std::istringstream iss(ligne);
            double x, z, taille_particule;
            char delimiter;
            if (iss >> x >> delimiter >> z >> delimiter >> taille_particule) {
                ensembles_particules.back().positions.emplace_back(x, z);
            }
        }
    }

    /*** LECTURE DU FICHIER VITESSES ***/
    fichierVitesses.clear(); // Reset du fichier
    fichierVitesses.seekg(0); // Revenir au début du fichier

    size_t index_ensemble = 0;
    while (std::getline(fichierVitesses, ligne)) {
        if (ligne.empty()) continue;

        if (ligne[0] == '#') { // Détection d'un nouvel ensemble
            if (index_ensemble < ensembles_particules.size()) {
                index_ensemble++;  // Passer à l'ensemble suivant
            } else {
                std::cerr << "Erreur : Trop d'ensembles de vitesses par rapport aux ensembles de particules !" << std::endl;
                return;
            }
        } else if (index_ensemble > 0) { // Ajout des vitesses
            Particules& ensemble_courant = ensembles_particules[index_ensemble - 1];

            std::istringstream iss(ligne);
            double vx, vz;
            char delimiter;
            if (iss >> vx >> delimiter >> vz) {
                ensemble_courant.vitesses.emplace_back(vx, vz);
            }
        }
    }

    /*** VÉRIFICATION DE L'INTÉGRITÉ DES DONNÉES ***/
    for (size_t i = 0; i < ensembles_particules.size(); ++i) {
        size_t nb_positions = ensembles_particules[i].positions.size();
        size_t nb_vitesses = ensembles_particules[i].vitesses.size();
        
        if (nb_positions != ensembles_particules[i].N) {
            std::cerr << "Erreur : Ensemble " << i << " contient " << nb_positions 
                      << " positions au lieu de " << ensembles_particules[i].N << std::endl;
        }

        if (nb_vitesses != ensembles_particules[i].N) {
            std::cerr << "Erreur : Ensemble " << i << " contient " << nb_vitesses 
                      << " vitesses au lieu de " << ensembles_particules[i].N << std::endl;
        }
    }

    /*** AJOUT DES DONNÉES AU FLUIDE COMPLEXE ***/
    particules = std::move(ensembles_particules);
    std::cout << "Initialisation depuis CSV terminée avec succès." << std::endl;
    
    /*** EXPORTATION POUR VÉRIFICATION ***/
    exporterPositionsCSV("positions_ini.csv");
    exporterVitessesCSV("vitesses_ini.csv");

    /*** INITIALISATION DES FORCES ***/
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
        int moyenne_k =0; // pour le calcul du nombres d'interactions en moyennes par ensembles
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
                            k += 1; // conteur pour determiner le nombre moyen d'interactions
                            f_ij = force_LJ(ensemble_i.E_0, ensemble_j.E_0, ensemble_i.d, ensemble_j.d, r_ij);
                            break;  // Par construction (r_c correctement choisi), une particule i n'interagit qu'avec une des 9 particules j 
                        }
                    }

                    // Ajouter la force d'interaction f_ij à la force totale
                    force_totale += f_ij;
                }
            }
            moyenne_k += k;
            // Ajouter la force totale pour cette particule au vecteur des forces d'interactions
            forces_interactions.push_back(force_totale);
        }
        moyenne_k /= ensemble_i.N;
        std::cout << moyenne_k << "interactions en moyenne sur les particules de l'ensemble\n";
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
    std::cout << "Pxx = " << Pxx << " Pa.m\n";
    std::cout << "Pzz = " << Pzz << " Pa.m\n";

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
    bool modifie = false;
    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        for (auto& position : ensemble.positions) {
            position.x = periodic_boundary_if_needed(position.x, L_x, modifie);
            position.z = periodic_boundary_if_needed(position.z, L_z, modifie);
            if (modifie){k+=1;}
        }
    }
    std::cout << "Modification de : " << k << " particules\n";
}

// Méthode pour appliquer le thermostat en fonction de la température T
void FluideComplexe::appliquer_thermostat(double T) {
    std::cout << "Calcul de T...\n";
    double T_mes = calculer_temperature();
    std::cout << "T = " << T_mes << " K\n";
    double alpha = 0.5;
    //double lambda = std::sqrt( 1 + delta_t * (T/T_mes -1) / tau_T ); // ancienne regulation de temperature posait probleme, à re tester
    // Parcours de chaque ensemble de particules
    for (auto& ensemble : particules) {
        for (auto& vitesse : ensemble.vitesses) {
            if (vitesse.norme() > 1e3){ // correction pour limiter explosion des vitesses, à supprimer si possible
                std::cout << "vitesse tres grande : ";
                vitesse.afficher();
                vitesse.x = 1e2;
                vitesse.z = 1e2;
            }
            //vitesse = vitesse * lambda; 
            // (mauvais parametres d'initialisation du fluide complexe ? ou juste innefficace ? ou ancienne erreur ?)
            relaxationVersMaxwell(vitesse, 3*T, ensemble.masse, alpha); // version provisoire du controle de temperature
            
        }
    }
    T_mes = calculer_temperature();
    std::cout << "La temperature est maintenant de : " << T_mes << std::endl;
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
    r_c = std::pow(lambda, 1/3.0) * r_c;

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
            T_mes += ensemble.masse * std::pow(vitesse.norme(), 2); 
        }
    }
    std::cout << "Smv2 " << T_mes << std::endl;
    T_mes /= (2 * N_tot * K_B); // 3 -> 2 pour essayer de tenir compte du passage 3D -> 2D
    return T_mes;
}

// Méthode pour calculer le tenseur de pression sur une tranche donnée
double FluideComplexe::calculer_tenseur_pression(int alpha, int beta, double Delta_z, double z_k) const {
    std::vector<Vec2> vecteurs_periodiques = {
        Vec2(0.0, 0.0),
        Vec2(L_x, 0.0), Vec2(-L_x, 0.0),
        Vec2(0.0, L_z), Vec2(0.0, -L_z),
        Vec2(L_x, L_z), Vec2(-L_x, L_z),
        Vec2(L_x, -L_z), Vec2(-L_x, -L_z)
    };
    
    // Définition des bornes du domaine avec conditions périodiques
    double z_min = z_k - Delta_z / 2;
    double z_max = z_k + Delta_z / 2;
    
    // Prise en compte des conditions périodiques
    if (z_min < -L_z / 2) z_min += L_z;
    if (z_max > L_z / 2) z_max -= L_z;
    
    // Liste des particules dans le domaine actuel
    std::vector<std::pair<const Particules*, size_t>> particules_domaine;
    
    // Sélectionner les particules qui appartiennent au domaine [z_min, z_max]
    for (const auto& ensemble : particules) {
        for (size_t i = 0; i < ensemble.positions.size(); ++i) {
            double zi = ensemble.positions[i].z;
            
            // Vérifier si la particule est dans la tranche avec conditions périodiques
            if ((zi >= z_min && zi <= z_max) || (z_min > z_max && (zi >= z_min || zi <= z_max))) {
                particules_domaine.emplace_back(&ensemble, i);
            }
        }
    }
    
    // Calcul du tenseur de pression
    double P_alpha_beta = 0.0;
    
    // Boucle sur les particules i du domaine
    for (const auto& [ensemble_i, i] : particules_domaine) {
        Vec2 position_i = ensemble_i->positions[i];
        Vec2 v_i = ensemble_i->vitesses[i];
        double m_i = ensemble_i->masse;
        // Ajouter la contribution au tenseur de pression des termes cinetiques
        // Contribution au tenseur de pression
        double contribution = 0.0;
             if (alpha == 0 && beta == 0) contribution = m_i * v_i.x * v_i.x;
        else if (alpha == 0 && beta == 1) contribution = m_i * v_i.x * v_i.z;
        else if (alpha == 1 && beta == 0) contribution = m_i * v_i.z * v_i.x;
        else if (alpha == 1 && beta == 1) contribution = m_i * v_i.z * v_i.z;

        P_alpha_beta += contribution;
        
        // Boucle sur toutes les particules j (dans et hors du domaine)
        for (const auto& ensemble_j : particules) {
            for (size_t j = 0; j < ensemble_j.positions.size(); ++j) {
                Vec2 position_j = ensemble_j.positions[j];
                
                // Éviter le calcul avec soi-même
                if (ensemble_i == &ensemble_j && i == j) continue;
                
                // Gérer les conditions périodiques
                Vec2 f_ij(0.0, 0.0);
                Vec2 r_ij(0.0, 0.0);

                for (const auto& vecteur : vecteurs_periodiques) {
                    r_ij = position_j - position_i + vecteur;

                    if (r_ij.norme() < r_c) {
                        f_ij = force_LJ(ensemble_i->E_0, ensemble_j.E_0, ensemble_i->d, ensemble_j.d, r_ij);
                        break;
                    }
                }
                
                // Ajouter la contribution au tenseur de pression
                // Contribution au tenseur de pression
                double contribution = 0.0;
                     if (alpha == 0 && beta == 0) contribution = f_ij.x * r_ij.x;
                else if (alpha == 0 && beta == 1) contribution = f_ij.x * r_ij.z;
                else if (alpha == 1 && beta == 0) contribution = f_ij.z * r_ij.x;
                else if (alpha == 1 && beta == 1) contribution = f_ij.z * r_ij.z;

                P_alpha_beta += contribution;
            }
        }
    }
    
    // Normalisation par la surface du domaine (L_x * Delta_z), correspond à des Pa.m !
    return P_alpha_beta / (L_x * Delta_z);
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