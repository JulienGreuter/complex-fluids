#ifndef FLUIDECOMPLEXE_H
#define FLUIDECOMPLEXE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include "particules.h"
#include "../vec2.h"  // Inclut la structure Vec2 pour les vecteurs 2D (x, z)
#include "reseau.h" // Inclut la classe Reseau et Case pour l'initialisation des positions

// Fonction pour calculer la force de Lennard-Jones entre deux particules
Vec2 force_LJ(double E_0i, double E_0j, double di, double dj, Vec2 r_ij);
// Fonction pour générer une vitesse selon une distribution de Maxwell-Boltzmann
Vec2 maxwellBoltzmannSample(double T, double masse);
// Fonction pour appliquer la relaxation vers Maxwell-Boltzmann sur une vitesse
void relaxationVersMaxwell(Vec2& vitesse, double T, double masse, double alpha);
// Fonction pour appliquer les conditions périodiques à une coordonné uniquement si nécessaire
double periodic_boundary_if_needed(double coord, double L, bool& modifie);
// Fonction pour lire les en-tetes
bool extraireEnteteEnsemble(const std::string& ligne, int& N, double& d, double& E_0, double& taille, double& masse);

class FluideComplexe {
private:
    // Attributs
    double L_x;                          // Longueur de la boîte en x
    double L_z;                          // Longueur de la boîte en z
    double delta_t;                      // Pas de temps pour la discrétisation
    double kappa;                        // Coefficient de compressibilité isotherme
    double tau_P;                        // Temps de réponse du barostat
    double tau_T;                        // Temps de réponse du thermostat
    double r_c;                          // Rayon de coupure des interactions
    std::vector<Particules> particules;  // Ensemble des ensembles de particules
    std::vector<Vec2> forces_interactions;  // Forces d'interactions des particules (indices correspondants à particules)
    std::string fichier_nom;   // Fichier contenant la description de fluide complexe en termes d'ensemble de particules
    
    std::unordered_map<std::string, std::tuple<double, double, double, double>> domaines;
    void initialisation_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire);
    void traiter_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire);

public:
    // Constructeur
    FluideComplexe(double L_x, double L_z, double delta_t, double kappa, double tau_P, double tau_T, double r_c, const std::string fichier_nom);

    // Méthode d'initialisation des positions et vitesses
    void initialisation(double T);
    void initialisationViaCSV(const std::string& filePositions, const std::string& fileVitesses);

    // Méthode pour calculer les forces d'interactions entre les particules
    void calculer_forces();

    // Méthode pour mettre à jour les positions des particules
    void mettre_a_jour_positions(double P);

    // Méthode pour mettre à jour les vitesses des particules
    void mettre_a_jour_vitesses(double T, const std::vector<Vec2>& forces_interactions_precedentes);

    // Méthode pour appliquer les conditions périodiques
    void appliquer_conditions_periodiques();

    // Méthode pour appliquer le thermostat en fonction de la température T
    void appliquer_thermostat(double T);

    // Méthode pour appliquer le barostat en fonction de la pression P
    void appliquer_barostat(double P);

    // Méthode pour calculer la température du fluide
    double calculer_temperature() const;

    // Méthode pour calculer le tenseur de pression sur une tranche donnée
    double calculer_tenseur_pression(int alpha, int beta, double Delta_z, double z_k) const;

    // Méthode pour faire évoluer le système vers l'état suivant
    void evoluer(double T, double P);

    // Méthode pour exporter les positions des particules sous CSV
    void exporterPositionsCSV(const std::string& fileCSV) const;

    // Méthode pour exporter les vitesses des particules sous CSV
    void exporterVitessesCSV(const std::string& fileCSV) const;
};

#endif // FLUIDECOMPLEXE_H
