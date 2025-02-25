#include <iostream>
#include <vector>
#include <memory>
#include "../../../sous_projet_modelisation2D/sous_projet_particules_non_ponctuelles/case.h"

void testSubdivision() {
    std::cout << "=== Test de subdivision ===\n";
    Case c(0.0, 0.0, 10.0);
    c.afficher();

    std::cout << "Subdivision...\n";
    c.subdiviser();
    c.afficher();
}

void testAllocationLiberation() {
    std::cout << "\n=== Test d'allocation et de libération ===\n";
    std::vector<std::unique_ptr<Case>> cases;

    for (int i = 0; i < 10000; ++i) {
        cases.push_back(std::make_unique<Case>(i, i, 5.0));
    }

    std::cout << "Création de 10 000 cases terminée.\n";
    cases.clear();
    std::cout << "Libération de la mémoire effectuée. voir htop pour verifier \n";
}

void testSubdivisionCaseParentsEnfants() {
    std::cout << "\n=== Test de subdivision de case parent, enfant et case subdivisée ===\n";
    
    Case c(0.0, 0.0, 10.0);
    c.afficher();
    
    std::cout << "Ordre de subdivision = " << c.getOrdreSubdivision() << std::endl;
    
    std::cout << "Subdivision...\n";
    c.subdiviser();

    std::cout << "Cases enfants 1 : \n";
    for (const auto& enfant : c.getEnfants()) {
        enfant->afficher();
        std::cout << "Subdivision de l'enfant...\n";
        enfant->subdiviser();
        std::cout << "\t Cases enfants 2 : \n";
        for (const auto& enfant2 : enfant->getEnfants()) {
        enfant2->afficher();
        }
        std::cout << std::endl;
    }

    std::cout << "Subdivision de la case parent deja subdivisée...\n";
    c.subdiviser();
   
}

int main() {
    testSubdivision();
    testAllocationLiberation();
    testSubdivisionCaseParentsEnfants();
    std::cout << "\n=== Fin des tests ===\n";
    return 0;
}