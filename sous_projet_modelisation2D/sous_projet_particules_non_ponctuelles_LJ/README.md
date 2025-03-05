# Les Classes:
## Particules:
### Attributs:

N : Nombre de particules  
E_0 : Paramètre du potentiel de Lennard-Jones  
d : Paramètre du potentiel de Lennard-Jones  
masse : Masse des particules  
taille : Taille des particules (initialisée à 0 pour des particules ponctuelles)  
charge : Charge électrique des particules (initialisée à 0 pour )  
positions : Positions des particules  
vitesses : Vitesses des particules  

Nous avons mis ces attributs en privés sauf pour la classe **FluideComplexe** via la ligne:  
    ```friend class FluideComplexe;```
Cela permet de faciliter l'interaction entre ces deux classes.  

### Constructeur
Particules(int N, double E_0, double d, double masse, double taille = 0.0, double charge = 0.0);
// Pour l'utilisation dans FluideComplexe, positions et vitesses sont initialisés plus tard

// Méthode pour initialiser les vitesses en fonction de la température T
void initialiserVitesses(double T);

    // Méthode pour appliquer les conditions périodiques (dépendant de L_x et L_z)
    void appliquerConditionsPeriodiques(double L_x, double L_z);

    // Setter pour positions et vitesses dans le cas d'une utilisation de la classe Particules en dehors de la classe FluideComplexe
    void setPositions(const std::vector<Vec2>& newPositions);
    void setVitesses(const std::vector<Vec2>& newVitesses);

    // Getter pour accéder aux attributs privés
    const std::vector<Vec2>& getPositions() const;
    const std::vector<Vec2>& getVitesses() const;
