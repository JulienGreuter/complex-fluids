# Les Classes:
## Particules:
### Attributs
N : Nombre de particules
E_0 : Paramètre du potentiel de Lennard-Jones
d : Paramètre du potentiel de Lennard-Jones
masse : Masse des particules
taille : Taille des particules (initialisée à 0 pour des particules ponctuelles)
charge : Charge électrique des particules (initialisée à 0 pour )
positions : Positions des particules
vitesses : Vitesses des particules

Nous avons mis ces attributs en privés sauf pour la classe **FluideComplexe** via la ligne:
    `friend class FluideComplexe;`
Cela permet de faciliter l'interaction entre ces deux classes.

