# Les Classes:
## Particules:
### Attributs:

***N*** : Nombre de particules  
***E_0*** : Paramètre du potentiel de Lennard-Jones  
***d*** : Paramètre du potentiel de Lennard-Jones  
***masse*** : Masse des particules  
***taille*** : Taille des particules (initialisée à 0 pour des particules ponctuelles)  
***charge*** : Charge électrique des particules (initialisée à 0 pour )  
***positions*** : Vecteurs positions des particules  
***vitesses*** : Vecteurs vitesses des particules  

Nous avons mis ces attributs en privés sauf pour la classe **FluideComplexe** via la ligne:  
    ```friend class FluideComplexe;```
Cela permet de faciliter l'interaction entre ces deux classes.  

### Constructeur:
##### Particules : ```N``` ,```E_0``` ,```d``` ,```masse,taille = 0.0``` ,```charge = 0.0```  
Nous initialisons certaines constantes des particules, elles sont donc supposés ponctuelles et neutre dans ce premier cas, les autres grandeurs seront définies avec les prochaines méthodes. 
 
##### initialiserVitesses : ```T```  
Cette méthode vise à statistiquement définir une vitesse pour chaque particules en fonction de la température global du fluide. 
Nous posons alors ```sigma = std::sqrt(K_B * T / masse)``` l'écart-type de la distribution de Maxwell-Boltzmann, puis nous générons un nombre aléatoire qui va nous permettre de calculer une distribution de position et de vitesse aléatoire et de type maxwellienne centrée et d'écart type ```sigma```.  
Puis d'autres pars nous créeons le vecteur ```sommeVitesses(0.0, 0.0)``` initialisé nulle, ce vecteur permet de "normer" les vecteurs vitesses pour que la vitesse du barycentre reste nulle.  

##### appliquerConditionsPeriodiques : ```L_x``` ,```L_z```  
Cette méthode permet de redéfinir les distances calculées lors d'une itération comme un module selon x et z. Cela permet d'appliquer les conditions périodiques aux limites. 
 
##### setPositions : ```newPositions``` , setVitesses : ```newVitesses``` , getPositions et getVitesses  
Ces deux méthodes ont pour objetifs respectifs de définir la position et la vitesse et de lire c'est dernières, ce sont des méthodes non utilisées pour le fluide complexe. Elles servent à tester la classe particules pour tester les différentes modifications qui ont été amenées  

## Fluidecomplexe:
### Fonction:

***Vec2 force_LJ*** : ```E_0i``` ,```E_0j``` ,```di``` ,```dj``` ,```r_ij```  
Fonction pour calculer la force de Lennard-Jones entre deux particules i et j, cela repose sur le produit de ```r_ij``` et d'un facteur calculé à partir des quatre paramètres du potentiel de Lennard-Jones.  

***Vec2 maxwellBoltzmannSample*** : ```T``` ,```masse```  
Fonction pour générer une vitesse selon une distribution de Maxwell-Boltzmann centrée et d'écart type ```sigma = std::sqrt(K_B * T / masse)```.  

***void relaxationVersMaxwell*** : ```& vitesse``` ,```T``` ,```masse``` ,```alpha```  
Fonction pour appliquer la relaxation de Maxwell-Boltzmann sur une vitesse.

***double periodic_boundary_if_needed*** : ```coord``` ,```L``` ,```& modifie```  
Fonction pour appliquer les conditions périodiques à une unique coordonné si nécessaire.

***bool extraireEnteteEnsemble*** ```& ligne``` ,```& N``` ,```& d``` ,```& E_0``` ,```& taille``` ,```& masse```  


### Attributs:

***L_x*** : Longueur de la boîte selon x  
***L_z*** : Longueur de la boîte selon z  
***delta_t*** : Pas de temps pour la discrétisation en temps  
***kappa*** : Coefficient de compressibilité isotherme  
***tau_P*** : Temps de réponse du barostat  
***tau_T*** : Temps de réponse du thermostat  
***r_c*** : Rayon de coupure des intéractions, la distance limite au dessus de laquelle l'on suppose que les particules n'interagissent plus  
***particules*** : Ensemble des ensembles de particules issue de la classe particules
***forces_interactions*** :  // Forces d'interactions des particules (indices correspondants à particules)
***fichier_nom*** :   // Fichier contenant la description de fluide complexe en termes d'ensemble de particules
    
    std::unordered_map<std::string, std::tuple<double, double, double, double>> domaines;
    void initialisation_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire);
    void traiter_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire);

### Constructeur:
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
