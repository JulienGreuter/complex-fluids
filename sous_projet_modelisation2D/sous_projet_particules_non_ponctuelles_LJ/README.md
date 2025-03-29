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
Fonction pour lire les en-têtes.  

### Attributs:

***L_x*** : Longueur de la boîte selon x  
***L_z*** : Longueur de la boîte selon z  
***delta_t*** : Pas de temps pour la discrétisation en temps  
***kappa*** : Coefficient de compressibilité isotherme  
***tau_P*** : Temps de réponse du barostat  
***tau_T*** : Temps de réponse du thermostat  
***r_c*** : Rayon de coupure des intéractions, la distance limite au dessus de laquelle l'on suppose que les particules n'interagissent plus 
***particules*** : Ensemble des ensembles de particules issue de la classe particules, il s'agit d'une liste "concaténée" qui contient toutes les particules  
***forces_interactions*** : Forces d'interactions entre les particules  
***fichier_nom*** : Fichier contenant la description initiale de fluide complexe en termes d'ensemble de particules
***std::unordered_map<std::string, std::tuple<double, double, double, double>> domaines*** : Bibliothéque contenant les informations sur l'espace accessibles pour les particules comme ```xmin```, ```xmax```, ```zmin```, ```zmax```  

    void initialisation_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire);
    void traiter_domaine(double T, const std::string& domaine, std::vector<Particules>& vecteur_intermediaire);

### Constructeur:

##### FluideComplexe : ```L_x``` ,```L_z``` ,```delta_t``` ,```kappa``` ,```tau_P``` ,```tau_T``` ,```r_c``` ,```fichier_nom```  
Initialisation du fluide complexe avec les différentes grandeurs qui lui sont caractéristiques  

##### initialisation : ```T```  
Méthode d'initialisation des positions et vitesses des particules du fluide 

##### initialisationViaCSV : ```& filePositions``` ,```& fileVitesses```  
Cette méthode est assimilable à la précédente, mais l'information sur la répartition des vitesses et positions ne vient pas de la température imposée, mais de deux fichiers qui contiennent déjà la répartition.

##### calculer_forces :  
Cette méthode vise à calculer les forces d'interactions entre les particules  

##### mettre_a_jour_positions : ```P```  
Méthode pour mettre à jour les positions des particules en fonctions des forces calculé avec la méthode précédente et des conditons limites périodiques  

##### mettre_a_jour_vitesses : ```T``` ,```& forces_interactions_precedentes```  
Méthode pour mettre à jour les vitesses pour un temps ```t+dt``` des particules en fonctions des forces calculé à un temps ```t``` et un temps ```t+dt```  
    
##### appliquer_conditions_periodiques : 
Cette méthode vise à ramener une particule dans l'espace ```L_x*L_z``` par périodicité de cette boîte  

##### appliquer_thermostat : ```T```  
Pour maintenir un thermostat constament à ```T```, et surtout d'éviter une explosion des vitesses  

##### appliquer_barostat : ```P```  
Cette méthode et comme la précédente mais en prenant cette fois ci la pression en compte

##### calculer_temperature :  
Méthode de calcule de la température du fluide via un calcule statistique sur la vitesse de chaque particules  

##### calculer_tenseur_pression : ```alpha``` ,```beta``` ,```Delta_z``` ,```z_k```  
Cette méthode permet de calculer le tenseur de pression sur une tranche de l'espace selon ```z```  

    // Méthode pour faire évoluer le système vers l'état suivant
    void evoluer(double T, double P);

    // Méthode pour exporter les positions des particules sous CSV
    void exporterPositionsCSV(const std::string& fileCSV) const;

    // Méthode pour exporter les vitesses des particules sous CSV
    void exporterVitessesCSV(const std::string& fileCSV) const;
