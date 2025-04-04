#ifndef RESEAU_H
#define RESEAU_H

#include "case.h"  
#include <vector>
#include <iostream>
#include <memory>  // Pour unique_ptr
#include <fstream>



class Reseau {
private:
    double xmin, xmax, zmin, zmax;
    double taille_case;
    int n_lignes, n_colonnes;
    
    // Matrice 2D de cases stockées sous forme de pointeurs intelligents
    std::vector<std::vector<std::unique_ptr<Case>>> cases;

    // Ensemble de pointeurs vers les cases libres
    std::vector<Case*> cases_libres;

public:
    // Constructeur
    Reseau(double xmin, double xmax, double zmin, double zmax, double taille_case);

    // Méthode pour tirer une case libre aléatoire
    Case* tirerCaseLibre();

    // Méthode pour subdiviser une case libre en 4 enfants
    void subdiviserCase(Case* case_a_subdiviser);

    // Méthode pour subdiviser toutes les cases libres du reseau pour un ordre de subdivision donné
    void subdiviser(int ordre);

    // Méthode pour retirer une case de l'ensemble des cases libres
    void retirerCaseLibre(Case* case_a_retirer);

    // Méthode pour ajouter les enfants d'une case parent à cases_libres
    void ajouterEnfantsCasesLibres(Case* case_parent);

    // Affichage du réseau
    void afficher() const;

    // Méthode pour afficher les statistiques du reseau
    void afficher_details() const;

    // Méthode pour exporter le reseau sous CSV
    void exporterCSV(const std::string& filename) const;

    // Getter pour accéder aux attributs privés
    const std::vector<std::vector<std::unique_ptr<Case>>>& getCases() const { return cases; }
    const std::vector<Case*>& getCasesLibres() const {return cases_libres;}
};

#endif // RESEAU_H