#include "particules.h"

// Constructeur de la classe Particules
Particules::Particules(int N, double E_0, double d, double masse, double taille, double charge)
    : N(N), E_0(E_0), d(d), masse(masse), taille(taille), charge(charge) {}
    // Initialisation des positions et vitesses sera faite plus tard (via setters ou FluideComplexe)


// Méthode pour initialiser les vitesses en fonction de la température T
void Particules::initialiserVitesses(double T) {
    // IMPLEMENTATION PROVISOIRE 
    vitesses.clear();  // On vide d'abord le vecteur existant de vitesses
    vitesses.resize(N, Vec2(0.0, 0.0));  // On redimensionne le vecteur et initialise toutes les vitesses à (0, 0) 
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