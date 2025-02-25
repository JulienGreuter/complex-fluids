#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct Vitesse {
    double vx, vz;
};

// Fonction pour lire un fichier CSV et extraire l'en-tête et les valeurs
bool lireFichierCSV(const std::string& nomFichier, std::vector<Vitesse>& donnees, std::vector<std::string>& entete) {
    std::ifstream fichier(nomFichier);
    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << nomFichier << std::endl;
        return false;
    }

    std::string ligne;
    bool estEnTete = true;

    while (std::getline(fichier, ligne)) {
        if (ligne.empty()) continue;

        if (estEnTete || ligne[0] == '#') {
            entete.push_back(ligne);
            estEnTete = false;
            continue;
        }

        std::istringstream ss(ligne);
        std::string valeur1, valeur2;

        if (std::getline(ss, valeur1, ',') && std::getline(ss, valeur2, ',')) {
            try {
                donnees.push_back({std::stod(valeur1), std::stod(valeur2)});
            } catch (...) {
                std::cerr << "Erreur de conversion : " << ligne << std::endl;
            }
        }
    }

    return true;
}

// Fonction pour écrire les résultats dans un fichier CSV
void ecrireFichierCSV(const std::string& nomFichier, const std::vector<Vitesse>& donnees, const std::vector<std::string>& entete) {
    std::ofstream fichier(nomFichier);
    if (!fichier) {
        std::cerr << "Erreur : Impossible de créer le fichier " << nomFichier << std::endl;
        return;
    }

    for (const auto& ligne : entete) {
        fichier << ligne << "\n";
    }

    for (const auto& v : donnees) {
        fichier << v.vx << "," << v.vz << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <fichier_1> <fichier_2> <alpha>" << std::endl;
        return 1;
    }

    std::string fichier_1 = argv[1];
    std::string fichier_2 = argv[2];
    double alpha = std::stod(argv[3]); //doit etre entre 0 et 1, permet de ponderer la moyenne
    std::string fichier_sortie = "vitesse_moy.csv";

    std::vector<Vitesse> donnees1, donnees2, moyenneDonnees;
    std::vector<std::string> entete;

    if (!lireFichierCSV(fichier_1, donnees1, entete) || !lireFichierCSV(fichier_2, donnees2, entete)) {
        return 1;
    }

    if (donnees1.size() != donnees2.size()) {
        std::cerr << "Erreur : Les fichiers ne contiennent pas le même nombre de lignes de données." << std::endl;
        return 1;
    }

    for (size_t i = 0; i < donnees1.size(); ++i) {
        moyenneDonnees.push_back({(donnees1[i].vx * alpha + donnees2[i].vx * (1-alpha)), (donnees1[i].vz * alpha + donnees2[i].vz * (1-alpha))});
    }

    ecrireFichierCSV(fichier_sortie, moyenneDonnees, entete);

    std::cout << "Fichier de sortie généré : " << fichier_sortie << std::endl;
    return 0;
}