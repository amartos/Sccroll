###############################################################################
# Informations sur le projet
###############################################################################

PROJECT 	:= sccroll
DESCRIPTION	:= Makefile du projet $(PROJECT)
VERSION		:= 0.1.0
COPYRIGHT 	:= Copyright (c) 2022 Alexandre Martos <contact@amartos.fr>
LICENSE 	:= Licence MIT

###############################################################################
# Environnement
###############################################################################

SHELL		:= /usr/bin/bash
SRCS		:= src
INCLUDES	:= include
TESTS		:= tests
SCRIPTS		:= scripts
BUILD		:= build
BIN		:= $(BUILD)/bin
DEPS		:= $(BUILD)/deps
OBJS		:= $(BUILD)/objs
SHARED		:= $(BUILD)/libs
DOCS		:= docs

vpath %.c  $(SRCS) $(TESTS)
vpath %.h  $(INCLUDES)
vpath %.o  $(OBJS)
vpath %.so $(SHARED)
vpath %.d  $(DEPS)


###############################################################################
# Paramètres de compilation
###############################################################################

CC		:= gcc
STD		:= gnu99
CFLAGS		:= -xc -Wall -std=$(STD) $(INCLUDES:%=-I%) -c
LDLIBS	 	:= -L $(SHARED) -l $(PROJECT)
DEPFLAGS	:= -MMD -MP -MF


###############################################################################
# Cibles à patterns
###############################################################################

%.o: %.c
	@$(CC) $(CFLAGS) $(DEPFLAGS) $(DEPS)/$*.d $< -o $(OBJS)/$@

lib%.so: %.c
	@$(CC) $(CFLAGS) -fpic -shared $(DEPFLAGS) $(DEPS)/$*.d $< -o $(SHARED)/$@

$(BIN)/%: %.o
	@$(CC) $(OBJS)/$*.o $(LDLIBS) -o $@


###############################################################################
# Autres cibles
###############################################################################

.PHONY: all $(PROJECT) tests doc init clean help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(SHARED)/%.so

all: $(PROJECT)

# @brief Compile la librairie (cible par défaut)
$(PROJECT): init lib$(PROJECT).so

# @brief Exécute les tests unitaires
tests:

# @brief Génère la documentation du projet
doc:

# @brief Initialise la structure du projet
init:
	@mkdir -p $(SRCS) $(INCLUDES) $(TESTS) $(SCRIPTS)
	@mkdir -p $(BUILD) $(BIN) $(OBJS) $(DEPS) $(SHARED)
	@mkdir -p $(DOCS)

# @brief Nettoyage post-compilation
clean:
	@rm -rf $(BUILD)

# @brief Affiche la documentation du Makefile
help:
	@echo "$(DESCRIPTION) - version $(VERSION)"
	@echo "$(COPYRIGHT)"
	@echo "$(LICENSE)"
	@echo ""
	@echo "Cibles disponibles:"
	@$(SCRIPTS)/pdoc.awk Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*.d)
