# Variables de compilation
CXX = g++                       # Compilateur C++
CXXFLAGS = -std=c++17 -Wall     # Options de compilation
LDFLAGS =                       # Drapeaux de l'éditeur de liens (à ajuster selon besoins)

# Répertoires
SRC_DIR = sous_projet_modelisation2D/sous_projet_particules_non_ponctuelles_LJ
OBJ_DIR = build/sous_projet_modelisation2D/sous_projet_particules_non_ponctuelles_LJ
BIN_DIR = bin/sous_projet_modelisation2D/sous_projet_particules_non_ponctuelles_LJ

# Fichiers sources communs
COMMON_SRC = $(SRC_DIR)/fluidecomplexe.cpp \
             $(SRC_DIR)/particules.cpp \
             $(SRC_DIR)/reseau.cpp \
             $(SRC_DIR)/case.cpp

COMMON_OBJ = $(COMMON_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Fichiers principaux (chaque exécutable a son propre fichier source principal)
MAIN_SRC = $(SRC_DIR)/test_FluideComplexe.cpp $(SRC_DIR)/pressions.cpp $(SRC_DIR)/pressions_via_fichiers.cpp $(SRC_DIR)/export_data.cpp
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Exécutables cibles
TARGETS = $(BIN_DIR)/test_FluideComplexe $(BIN_DIR)/pressions $(BIN_DIR)/pressions_via_fichiers $(BIN_DIR)/export_data

# Vérification des fichiers sources
$(info Fichiers sources communs : $(COMMON_SRC))
$(info Fichiers sources principaux : $(MAIN_SRC))

# Règle par défaut (compilation de tous les exécutables)
all: $(TARGETS)

# Règle pour générer chaque exécutable
$(BIN_DIR)/test_FluideComplexe: $(OBJ_DIR)/test_FluideComplexe.o $(COMMON_OBJ)
	@echo "Création de l'exécutable test_FluideComplexe..."
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/pressions: $(OBJ_DIR)/pressions.o $(COMMON_OBJ)
	@echo "Création de l'exécutable pressions..."
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/pressions_via_fichiers: $(OBJ_DIR)/pressions_via_fichiers.o $(COMMON_OBJ)
	@echo "Création de l'exécutable pressions_via_fichiers..."
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/export_data: $(OBJ_DIR)/export_data.o $(COMMON_OBJ)
	@echo "Création de l'exécutable export_data..."
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)
	
# Règle pour compiler les fichiers source en objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compilation de $< en $@..."
	@mkdir -p $(OBJ_DIR)
	@if [ ! -f $< ]; then echo "Erreur: $< n'existe pas"; exit 1; fi
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Règle de nettoyage
clean:
	@echo "Nettoyage des fichiers objets..."
	rm -rf $(OBJ_DIR)/*.o

fclean: clean
	@echo "Nettoyage des exécutables..."
	rm -rf $(TARGETS)

# Règle pour régénérer le projet
re: fclean all

.PHONY: all clean fclean re