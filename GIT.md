# **Guide Complet sur Git**

## &#x31;&#xFE0F;&#x20E3; **Pourquoi utiliser Git ?**

### **Qu'est-ce que le contrôle de version ?**

Le **contrôle de version** (ou *Version Control System*, VCS) est un système qui permet de suivre les modifications d'un fichier ou d'un ensemble de fichiers au fil du temps. Cela facilite :

- **Le suivi des modifications** : Voir qui a modifié quoi et pourquoi.
- **La récupération d'anciennes versions** : Restaurer une version précédente en cas d'erreur.
- **La collaboration** : Plusieurs personnes peuvent travailler sur le même projet simultanément sans conflits majeurs.

### Limites des méthodes traditionnelles

Une méthode simple pour suivre les modifications consiste à copier les fichiers dans différents dossiers en ajoutant un **timestamp** (horodatage) :

```plaintext
mon_projet_20240601/
mon_projet_20240602/
```
### Problèmes avec cette méthode :
Inefficace pour collaborer : Difficile de gérer les contributions de plusieurs personnes.
Risque d'erreurs : Se tromper de dossier peut entraîner des pertes de données.
Pas de suivi des changements : Impossible de voir facilement ce qui a été modifié.
### Les systèmes de contrôle de version
Pour résoudre ces problèmes, on utilise :

- **CVCS** (Centralized Version Control Systems) : Utilise un serveur central pour stocker le projet.
Inconvénient : Risque de perte totale des données si le serveur central tombe en panne.

- **DVCS** (Distributed Version Control Systems) comme Git :
Chaque utilisateur possède une copie complète du projet (historique inclus), en local et sur un serveur distant comme GitHub. Cela rend le système plus résilient.

## &#x32;&#xFE0F;&#x20E3; **Installation de Git**
**Sur Ubuntu / Debian**
Installez Git avec :

```bash
sudo apt install git-all
```

Vérifiez l’installation avec :

```bash
git --version
```
## &#x33;&#xFE0F;&#x20E3; **Configuration de Git**
Après l’installation, configurez votre nom et e-mail :

```bash
git config --global user.name "Votre Nom"
git config --global user.email "votre.email@exemple.com"
```
## &#x34;&#xFE0F;&#x20E3; **Commandes de base Git**
Les trois zones de Git
- **Working Directory** : La zone de travail où vous modifiez les fichiers.
- **Staging Area** : La zone où vous ajoutez les modifications avant de les valider.
- **Repository** : La base de données locale où les modifications sont enregistrées après un commit.

### **Suivi des fichiers**
Vérifier l'état des fichiers :

```bash
git status
```
### **Ajouter des fichiers à la Staging Area** :

```bash
git add nom_du_fichier
```
### **Pour ajouter tous les fichiers** :

```bash
git add .
```
### **Voir les différences avant un commit** :

```bash
git diff --staged
```
### **Valider les modifications (commit) avec un message descriptif** :

```bash
git commit -m "Description des modifications"
```
## &#x35;&#xFE0F;&#x20E3; **Gestion des branches**
Pourquoi utiliser des branches ?
Les branches permettent de travailler sur différentes fonctionnalités sans affecter la branche principale (`master` ou `main`).

## &#x36;&#xFE0F;&#x20E3; **Commandes pour gérer les branches**
- **Créer une nouvelle branche** :

```bash
git branch nom_de_la_branche
```
- **Lister les branches existantes** :

```bash
git branch
```
- **Changer de branche** :

```bash
git checkout nom_de_la_branche  "si la branche est existente"

git checkout -b nom_de_la_branche  "si elle n'existe pas encore"
```

## &#x37;&#xFE0F;&#x20E3; **Utilisation des remotes avec GitHub**
- **Pour lier votre projet local à un repository distant sur GitHub**:

```bash
git remote add origin https://github.com/utilisateur/nom_du_repository.git
```
- **Voir les remotes existants**
```bash
git remote -v
```
- **Envoyer les modifications sur le remote**
```bash
git push origin nom_de_la_branche
``` 
- **Récupérer les modifications depuis le remote**
```bash
git pull origin nom_de_la_branche
```
- **Historique des modifications**
   - **Afficher l'historique des commits** :

   ```bash
   git log
   ```
   - **Voir l'historique en résumé (une ligne par commit)** :

   ```bash
   git log --pretty=oneline
   ```
   - **Afficher les 2 derniers commits avec leurs différences** :

   ```bash
   git log -p -2
   ```

