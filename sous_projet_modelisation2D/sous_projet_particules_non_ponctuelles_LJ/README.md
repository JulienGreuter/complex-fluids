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
### Attributs:

