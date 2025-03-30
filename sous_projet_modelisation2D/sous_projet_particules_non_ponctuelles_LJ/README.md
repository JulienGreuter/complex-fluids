# Fonctionnement global:

Nous allons ici développer le fonctionnement de manière chronologique du fichier ```export_data.cpp``` qui permet de générer l'exécutable ```exportdata``` via le **Makefile**. Le tout repose sur l'utilisation de quelques classes mais surtout de la classe [```FluideComplexe```](#FC) qui va nous permettre de simuler le fluide dans un espace défini.  

<div id='MAINiniFC'/>

## Initialisation:

Pour instancier [```FluideComplexe```](#FC) nous posons en amon:  

***Lx, Lz*** : les dimensions de la boîte  
***delta_t**** : le pas de temps élémentaire pour l'évolution  
***Kappa, tau_P, tau_T*** : les paramètres physiques de la simulation  
***r_c*** : rayon de coupure pour les interactions  

De plus le ```main```, demande quatre arguments pour l'éxécution qui sont:  

1. Un **fichier d'initialisation**  
2. Une **température**  
3. Une **pression**  
4. Un **nombre d'évolutions**  

Pour éviter les problèmes plus tard nous commencons par **vérifier** que la température et la pression soient positives et que le fichier d'initialisation peut être ouvert.  

## Instanciation du fluide complexe:

À la ligne 49 du fichier ```export_data.cpp```, nous instancions un objet [```FluideComplexe```](#FC) tel que :

```cpp
FluideComplexe fluide = FluideComplexe(LX, LZ, Delta_T, Kappa, tau_P, tau_T, r_c, fichier_ini);
```

Cependant, cette instanciation n'induit pas directement un objet de type ```fluide```, en effet, ce sera le cas aprés l'uilisation de la méthode [```initialisation(double T)```](#FCini) via la température entrée précédement.  

## Initialisation des particules du fluide:

### Structure du fichier d'initialisation:

Un fichier d'initialisation contient de haut en bas:  
  
1. Une en-tête commenté présentant le cadre de la simulation  
  
2. Une ligne par **ensemble de particules** contenant:<div id='MAINiniPA'/>
```
  # Format : 

#N      E_0     d     masse   taille charge id_ini
1000     1.0    0.1     10     0.0     0.0     D31
2000     1.2    0.4     30     0.0     0.0     D32
5000     1.0    0.1     10     0.0     0.0     D32
300      0.1    0.8     100    0.3     0.0     D32
100      0.2    0.7     300    0.9     0.0     D32
5000     0.5    0.2     10     0.0     0.0     D32
1000     1.0    0.1     10     0.0     0.0     D33
```
  2.1 Le nombre de particules `N`  
  2.2 l'énergie d’interaction `E_0`  
  2.3 La distance caractéristique `d`  
  2.4 La masse, taille et charge des particules  
  2.5 Le domaine d'affectation (ex. : `D1`, `D2`, etc.)   

Ceci permet d'obtenir les informations nécessaire à l'instanciation d'un objet [```particules```](#PA), sauf pour leurs positions et vitesses. Pour la vitesse, nous pourrons utiliser la méthode [```initialiserVitesses```](#PAini) 

### b) Méthode `initialisation(double T)`

La méthode `initialisation` :

1. **Lit le fichier d'initialisation** et extrait les données.
2. **Crée des ensembles de particules** avec les paramètres lus.
3. **Trie et place les particules** dans leurs domaines respectifs via `traiter_domaine()`.
4. **Initialise les vitesses** avec `initialiserVitesses(T)`, en suivant une distribution de Maxwell-Boltzmann.
5. **Exporte les positions et vitesses** (`positions_ini.csv`, `vitesses_ini.csv`).
6. **Calcule les forces d'interaction initiales** (`calculer_forces()`).

### c) Placement des particules

L'initialisation des positions et vitesses est réalisée par `initialisation_domaine()`. Cette méthode :

- Récupère les bornes du domaine (définies dans `domaines` lors de l'instanciation de `FluideComplexe`).
- Utilise la classe `Reseau` pour subdiviser l’espace en cellules.
- Place les particules en **taille décroissante** pour optimiser le remplissage.
- Affecte les vitesses aux particules.

## 4. Évolution du fluide

Dans `export_data.cpp`, après l'initialisation, une boucle `for` exécute un nombre donné d’évolutions :

```cpp
for (int i = 0; i < nombre_d_evolutions; i++) {
    fluide.evoluer(Température, Pression);
}
```

La méthode `evoluer(T, P)` fait évoluer le fluide en mettant à jour :

1. **Les positions** en fonction des vitesses et forces (`mettre_a_jour_positions(P)`).
2. **Les forces d'interaction** (`calculer_forces()`).
3. **Les vitesses** selon l'algorithme de Verlet (`mettre_a_jour_vitesses(T, forces_t, forces_t+dt)`).

## 5. Rôle du thermostat et du barostat

- `appliquer_thermostat(T)` ajuste les vitesses pour **maintenir la température cible**.
- `appliquer_barostat(P)` modifie les positions pour **ajuster la pression**.

Ces ajustements suivent une relaxation définie par `tau_T` (thermostat) et `tau_P` (barostat).

## 6. Conclusion

Le programme `exportdata` permet de :

1. **Initialiser** un fluide complexe en lisant un fichier d'initialisation.
2. **Faire évoluer le système** en mettant à jour positions, vitesses et forces.
3. **Réguler la température et la pression** à l’aide d’un thermostat et d’un barostat.

Chaque évolution du système est enregistrée dans des fichiers CSV pour analyse ultérieure.

# Les Classes:

<div id='PA'/>

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
#### Particules : ```N``` ,```E_0``` ,```d``` ,```masse,taille = 0.0``` ,```charge = 0.0```  
Nous initialisons certaines constantes des particules, elles sont donc supposés ponctuelles et neutre dans ce premier cas, les autres grandeurs seront définies avec les prochaines méthodes  
<div id='PAini'/>
 
#### [initialiserVitesses](#MAINiniPA) : ```T```  
Cette méthode vise à statistiquement définir une vitesse pour chaque particules en fonction de la température global du fluide. 
Nous posons alors ```sigma = std::sqrt(K_B * T / masse)``` l'écart-type de la distribution de Maxwell-Boltzmann, puis nous générons un nombre aléatoire qui va nous permettre de calculer une distribution de position et de vitesse aléatoire et de type maxwellienne centrée et d'écart type ```sigma```.  
Puis d'autres pars nous créeons le vecteur ```sommeVitesses(0.0, 0.0)``` initialisé nulle, ce vecteur permet de "normer" les vecteurs vitesses pour que la vitesse du barycentre reste nulle  
 
#### setPositions : ```newPositions``` , setVitesses : ```newVitesses``` , getPositions et getVitesses  
Ces deux méthodes ont pour objetifs respectifs de définir la position et la vitesse et de lire c'est dernières, ce sont des méthodes non utilisées pour le fluide complexe. Elles servent à tester la classe particules pour tester les différentes modifications qui ont été amenées  
<div id='FC'/>

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

#### initialisation_domaine : ```T``` ,```& domaine``` ,```& vecteur_intermediaire```  
Cette initialisation vise à définir les positions et vitesses initiales des particules du fluide  

#### traiter_domaine : ```T``` ,```& domaine``` ,```& vecteur_intermediaire```  
Cette méthode vise à préparer l'ordre des particules dasn chaques domaines, en effet pour placer les particules initialement nous avons besoin de trier les particules dans l'ordre décroissant des tailles  

#### FluideComplexe : ```L_x``` ,```L_z``` ,```delta_t``` ,```kappa``` ,```tau_P``` ,```tau_T``` ,```r_c``` ,```fichier_nom```  
Initialisation du fluide complexe avec les différentes grandeurs qui lui sont caractéristiques  
<div id='FCini'/>
  
#### [initialisation](#MAINiniFC) : ```T```  
Méthode d'initialisation des positions et vitesses des particules du fluide, elle permet de "finir" l'instanciation du fluide  

#### initialisationViaCSV : ```& filePositions``` ,```& fileVitesses```  
Cette méthode est assimilable à la précédente dans son objectif, mais l'information sur la répartition des vitesses et positions ne vient pas de la température imposée, mais de deux fichiers qui contiennent déjà la répartition  

#### calculer_forces :  
Cette méthode vise à calculer les forces d'interactions entre les particules  

#### mettre_a_jour_positions : ```P```  
Méthode pour mettre à jour les positions des particules en fonctions des forces calculé avec la méthode précédente et des conditons limites périodiques  

#### mettre_a_jour_vitesses : ```T``` ,```& forces_interactions_precedentes```  
Méthode pour mettre à jour les vitesses pour un temps ```t+dt``` des particules en fonctions des forces calculé à un temps ```t``` et un temps ```t+dt```  
    
#### appliquer_conditions_periodiques : 
Cette méthode vise à ramener une particule dans l'espace ```L_x*L_z``` par périodicité de cette boîte  

#### appliquer_thermostat : ```T```  
Pour maintenir un thermostat constament à ```T```, et surtout d'éviter une explosion des vitesses  

#### appliquer_barostat : ```P```  
Cette méthode et comme la précédente mais en prenant cette fois ci la pression en compte

#### calculer_temperature :  
Méthode de calcule de la température du fluide via un calcule statistique sur la vitesse de chaque particules  

#### calculer_tenseur_pression : ```alpha``` ,```beta``` ,```Delta_z``` ,```z_k```  
Cette méthode permet de calculer le tenseur de pression sur une tranche de l'espace selon ```z```  

#### evoluer : ```T``` ,```P```  
Cette méthode est celle qui permet l'évolution global du système avec toutes les méthodes précédentes, c'est pour cela qu'elle demande une température et une pression, ce sera le thermostat et le barostat  
    
#### exporterPositionsCSV : ```& fileCSV```  
Méthode pour exporter les positions des particules vers un fichier CSV  

#### exporterVitessesCSV : ```& fileCSV```
Méthode pour exporter les vitesses des particules vers un fichier CSV  

## Case:

Cette classe est faite pour faciliter la manipulation des cases qui pave le domaine de travaill  

### Attributs:
***x, z*** : Position du centre de la case  
***ordre_subdivision*** : Ordre de subdivision, le nombre de subdivision à suivre pour être dans cette case  
***enfants*** : Pointeurs intelligents pour limiter la taille en mémoire du réseau  
***taille_case*** : Taille d'une case  
***est_libre*** : État de la case, pour savoir si elle est vide ou non  

### Méthodes:

#### Case : ```x``` ,```z```, ```taille_case```, ```ordre_subdivision = 0```, ```est_libre = true```  
Initialisation de la classe avec des cases  

#### subdiviser :  
Méthode pour subdiviser une case en 4 case enfants de même tailles  

#### estParent :  
Vérifie si la case est un parent, donc si elle admet des case d'ordre plus élevée en elle  

#### estLibre :  
Vérifie si la case est libre (non occupée)  

#### marquerOccupee :  
Change l'état de liberté de la case vers occupée  

#### afficher :  
Affiche des informations de la case  

#### getX , getZ , getOrdreSubdivision , getEnfants et getTaille :  
Getter pour accéder aux attributs et privés d'une case  

## Reseau:
### Attributs:
***xmin, xmax, zmin, zmax*** : Les limites de la boîte  
***taille_case*** : Tailles des cases du réseau  
***n_lignes, n_colonnes*** : nombre de lignes et de colonnes  
***cases*** : Matrice 2D de cases stockées sous forme de pointeurs intelligents  
***cases_libres*** : Ensemble de pointeurs lié à des cases libres

### Méthodes:

Reseau(double xmin, double xmax, double zmin, double zmax, double taille_case);

#### tirerCaseLibre :  
Méthode pour tirer au hasard une case parmi les cases libres  

#### subdiviserCase : ```case_a_subdiviser```  
Méthode pour subdiviser une case libre en 4 enfants  

#### subdiviser : ```ordre```  
Méthode pour subdiviser toutes les cases libres du reseau pour un ordre de subdivision donné  

#### retirerCaseLibre : ```case_a_retirer```  
Cette méthode permet de retirer une case de l'ensemble des cases libres  

#### ajouterEnfantsCasesLibres : ```case_parent```  
Méthode pour ajouter les enfants d'une case à ```cases_libres```  

#### afficher :  
Méthode d'affichage du réseau et de ses caractéristiques  

#### afficher_details :  
Méthode pour afficher les statistiques du réseau  

#### exporterCSV : ```filename```  
Méthode pour exporter le réseau et ses caractéristiques sous CSV  

#### getCases , getCasesLibres :  
Méthode pour accéder aux grandeurs misent en ```private```  

