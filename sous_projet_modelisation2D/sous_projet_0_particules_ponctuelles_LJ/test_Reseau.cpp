#include "reseau.h"

void testSubdivisionMultiple( int N ){
    std::cout << "\nInstanciation du reseau...\n";
    Reseau reseau(0.0, 55.0, 0.0, 50.0, 10.0);
    std::cout << "Affichage du reseau...\n";
    reseau.afficher();

    std::cout << "\n=== Test subdivision multiple N=" << N <<" ===\n";
    for (int it = 0; it < N ; ++it){
        std::cout << "Tirage d'une case libre...\n";
        Case* case_tiree = reseau.tirerCaseLibre();
        if (case_tiree) {
        case_tiree->afficher();
        std::cout << "Subdivision de la case tiree...\n";
        reseau.subdiviserCase(case_tiree);
        }
        else { 
            std::cout << "Aucune case libre tirée\n";    
        }
    }
    std::cout << "\nAffichage du reseau...\n";
    reseau.afficher();
    std::cout << "\nAffichage details...\n";
    reseau.afficher_details();
}



int main() {

    std::cout << "\n=== Test classe Reseau ===\n";

    testSubdivisionMultiple( 20 );

    std::cout << "\n=== Fin des tests de la classe Reseau ===\n";
    return 0;
}