#ifndef CASE_H
#define CASE_H

#include <vector>
#include <iostream>
#include <memory>  // Pour unique_ptr

class Case {
private:

    double x, z;                        // Position du centre de la case
    int ordre_subdivision;              // Ordre de subdivision
    std::vector<std::unique_ptr<Case>> enfants; // Pointeurs intelligents pour éviter les fuites mémoire
    double taille_case;                 // Taille de la case
    bool est_libre;                     // État de la case

public:

    // Constructeur
    Case(double x, double z, double taille_case, int ordre_subdivision = 0, bool est_libre = true);

    // Méthode pour subdiviser une case en 4 enfants
    void subdiviser();

    // Vérifie si la case est un parent
    bool estParent() const { return !enfants.empty(); }

    // Vérifie si la case est libre
    bool estLibre() const { return est_libre; }

    // Marque la case comme occupée
    void marquerOccupee() { est_libre = false; }

    // Affichage des informations de la case
    void afficher() const;

    // Getter
    int getOrdreSubdivision() const { return ordre_subdivision; }
    
    const std::vector<std::unique_ptr<Case>>& getEnfants() const { return enfants; }
};

#endif // CASE_H
