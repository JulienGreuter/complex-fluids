#include <iostream>
#include <vector>
#include "../../../sous_projet_modelisation2D/sous_projet_particules_non_ponctuelles/particules.h"
#include "../../../sous_projet_modelisation2D/vec2.h"  

int main() {
    std::cout << " Test de l'instanciation de l'objet Particules sans positions et vitesses initialisées\n";
    Particules particule(5, 1.0, 2.5, 1.0, 0.5, 1.0);  // Paramètres : N, E_0, d, masse, taille, charge

    std::cout << " Test des setters : Initialisation des positions et vitesses\n";
    std::vector<Vec2> positions(5, Vec2(0.0, 0.0));  // Création d'un vecteur de positions (5 particules à (0, 0))
    std::vector<Vec2> vitesses(5, Vec2(1.0, 1.0));  // Création d'un vecteur de vitesses (5 particules avec (1, 1))

    std::cout << " Utilisation des setters\n";
    particule.setPositions(positions);
    particule.setVitesses(vitesses);

    std::cout << " Test des getters : Vérification de l'initialisation des positions et vitesses\n";
    const std::vector<Vec2>& retrievedPositions = particule.getPositions();
    const std::vector<Vec2>& retrievedVitesses = particule.getVitesses();

    std::cout << " Affichage des résultats\n";
    std::cout << "Positions : " << std::endl;
    for (const auto& pos : retrievedPositions) {
        std::cout << "(" << pos.x << ", " << pos.z << ")" << std::endl;
    }

    std::cout << "Vitesses : " << std::endl;
    for (const auto& vit : retrievedVitesses) {
        std::cout << "(" << vit.x << ", " << vit.z << ")" << std::endl;
    }

    return 0;
}