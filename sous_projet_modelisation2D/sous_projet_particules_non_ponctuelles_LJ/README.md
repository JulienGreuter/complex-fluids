# Les Classes:
## Particules:

Cette classe décrit des ensembles de particules de même types  

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
Cela permet de faciliter l'interaction entre ces deux classes  

### Méthodes:
##### Particules : ```N``` ,```E_0``` ,```d``` ,```masse,taille = 0.0``` ,```charge = 0.0```  
Nous initialisons certaines constantes des particules, elles sont donc supposés ponctuelles et neutre dans ce premier cas, les autres grandeurs seront définies avec les prochaines méthodes  
 
##### initialiserVitesses : ```T```  
Cette méthode vise à statistiquement définir une vitesse pour chaque particules en fonction de la température global du fluide. 
Nous posons alors ```sigma = std::sqrt(K_B * T / masse)``` l'écart-type de la distribution de Maxwell-Boltzmann, puis nous générons un nombre aléatoire qui va nous permettre de calculer une distribution de position et de vitesse aléatoire et de type maxwellienne centrée et d'écart type ```sigma```.  
Puis d'autres pars nous créeons le vecteur ```sommeVitesses(0.0, 0.0)``` initialisé nulle, ce vecteur permet de "normer" les vecteurs vitesses pour que la vitesse du barycentre reste nulle.  
 
##### setPositions : ```newPositions``` , setVitesses : ```newVitesses``` , getPositions et getVitesses  
Ces deux méthodes ont pour objetifs respectifs de définir la position et la vitesse et de lire c'est dernières, ce sont des méthodes non utilisées pour le fluide complexe. Elles servent à tester la classe particules pour tester les différentes modifications qui ont été amenées  

## Fluidecomplexe:

Cette méthode permet de regrouper plusieurs objets particules avec l'objet de fluide complexe et de décrire l'évolution de ce nouvel objet et d'appliquer les conditions limites périodiques  

### Fonction:

***Vec2 force_LJ*** : ```E_0i``` ,```E_0j``` ,```di``` ,```dj``` ,```r_ij```  
Fonction pour calculer la force de Lennard-Jones entre deux particules i et j, cela repose sur le produit de ```r_ij``` et d'un facteur calculé à partir des quatre paramètres du potentiel de Lennard-Jones  

***Vec2 maxwellBoltzmannSample*** : ```T``` ,```masse```  
Fonction pour générer une vitesse selon une distribution de Maxwell-Boltzmann centrée et d'écart type ```sigma = std::sqrt(K_B * T / masse)```  

***void relaxationVersMaxwell*** : ```& vitesse``` ,```T``` ,```masse``` ,```alpha```  
Fonction pour appliquer la relaxation de Maxwell-Boltzmann sur une vitesse  

***double periodic_boundary_if_needed*** : ```coord``` ,```L``` ,```& modifie```  
Fonction pour appliquer les conditions périodiques à une unique coordonné si nécessaire  

***bool extraireEnteteEnsemble*** ```& ligne``` ,```& N``` ,```& d``` ,```& E_0``` ,```& taille``` ,```& masse```  
Fonction pour lire les en-têtes  

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

### Méthodes:

##### initialisation_domaine : ```T``` ,```& domaine``` ,```& vecteur_intermediaire```  
Cette initialisation vise à définir les positions et vitesses initiales des particules du fluide  

##### traiter_domaine : ```T``` ,```& domaine``` ,```& vecteur_intermediaire```  
Cette méthode vise à préparer l'ordre des particules dasn chaques domaines, en effet pour placer les particules initialement nous avons besoin de trier les particules dans l'ordre décroissant des tailles  

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

##### evoluer : ```T``` ,```P```  
Cette méthode est celle qui permet l'évolution global du système avec toutes les méthodes précédentes, c'est pour cela qu'elle demande une température et une pression, ce sera le thermostat et le barostat  
    
##### exporterPositionsCSV : ```& fileCSV```  
Méthode pour exporter les positions des particules vers un fichier CSV  

##### exporterVitessesCSV : ```& fileCSV```
Méthode pour exporter les vitesses des particules vers un fichier CSV  

## Case:
### Attributs:
***x, z*** : Position du centre de la case
***ordre_subdivision*** : Ordre de subdivision, le nombre de subdivision à suivre pour être dans cette case
***enfants*** : Pointeurs intelligents pour éviter les fuites mémoire------------------------------------------------------------------------***taille_case*** : Taille d'une case
***est_libre*** : État de la case, pour savoir si elle est vide ou non

### Méthodes:

##### Case : ```x``` ,```z```, ```taille_case```, ```ordre_subdivision = 0```, ```est_libre = true```  
Initialisation de la classe avec des cases

##### subdiviser :  
Méthode pour subdiviser une case en 4 case enfants de même tailles  

##### estParent :  
Vérifie si la case est un parent, donc si elle admet des case d'ordre plus élevée en elle  

##### estLibre :  
Vérifie si la case est libre (non occupée)  

##### marquerOccupee :  
Change l'état de liberté de la case vers occupée  

##### afficher :  
Affiche des informations de la case  

##### getX , getZ , getOrdreSubdivision , getEnfants et getTaille :  
Getter pour accéder aux attributs et privés d'une case  

