#include <cmath>
#include <random>
#include "particules.h"

// Constante de Boltzmann (en unités SI : J/K)
const double K_B = 1.380649e-23;

// Constructeur de la classe Particules
Particules::Particules(int N, double E_0, double d, double masse, double taille, double charge)
    : N(N), E_0(E_0), d(d), masse(masse), taille(taille), charge(charge) {}
    // Initialisation des positions et vitesses sera faite plus tard (via setters ou FluideComplexe)


// Méthode pour initialiser les vitesses en fonction de la température T
void Particules::initialiserVitesses(double T) { 
    vitesses.clear();  // On vide d'abord le vecteur existant de vitesses
    vitesses.resize(N, Vec2(0.0, 0.0));  // On redimensionne le vecteur et initialise toutes les vitesses à (0, 0)
    
    double sigma = std::sqrt(K_B * T / masse);  // Écart-type de la distribution de Maxwell-Boltzmann

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(0.0, sigma); // Distribution normale centrée en 0 avec écart-type sigma

    Vec2 sommeVitesses(0.0, 0.0);  // Pour calculer la vitesse moyenne

    // Génération des vitesses initiales
    vitesses.resize(N);
    for (int i = 0; i < N; ++i) {
        double vx = distribution(gen);
        double vz = distribution(gen);
        vitesses[i] = Vec2(vx, vz);
        sommeVitesses.x += vx;
        sommeVitesses.z += vz;
    }

    // Correction pour annuler la vitesse du centre de masse
    for (int i = 0; i < N; ++i) {
        vitesses[i] -= sommeVitesses * (1/N);    
    }
} 

// Méthode pour appliquer les conditions périodiques (dépendant de L_x et L_z)
void Particules::appliquerConditionsPeriodiques(double L_x, double L_z) {
}

// Setter 
void Particules::setPositions(const std::vector<Vec2>& newPositions) {
    positions = newPositions;  
}
void Particules::setVitesses(const std::vector<Vec2>& newVitesses) {
    vitesses = newVitesses;  
}

// Getter 
const std::vector<Vec2>& Particules::getPositions() const {
    return positions;  
}
const std::vector<Vec2>& Particules::getVitesses() const {
    return vitesses;  
}