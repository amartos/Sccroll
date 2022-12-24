###############################################################################
# Informations sur le projet
###############################################################################

LICENSEFILE	= LICENSE
LICENSE 	= $(shell head -n 2 $(LICENSEFILE) | tail -n 1)
BRIEF		= $(shell head -n 1 $(LICENSEFILE))
NAME		= $(firstword $(BRIEF))
PROJECT 	= $(shell echo $(NAME) | tr "[:upper:]" "[:lower:]")
VERSION		= $(shell find . -type f -name "$(PROJECT).[h|c]" | xargs grep version | awk '{print $$NF}')
LOGO		=


###############################################################################
# Environnement
###############################################################################

SHELL		= /usr/bin/env bash
SRCS		= src
INCLUDES	= include
TESTS		= tests
SCRIPTS		= scripts


###############################################################################
# Environnement de build
###############################################################################

TARGET		= lib$(PROJECT).so
CDEPS		= $(shell find $(SRCS) -type f -name "*.c")

ASSETS		= $(TESTS)/assets
TLOGS		= $(ASSETS)/logs
UNITS		= $(TESTS)/units
CUNITS		= $(shell find $(UNITS) -type f -name "*.c")

BUILD		= build
BIN			= $(BUILD)/bin
DEPS		= $(BUILD)/deps
OBJS		= $(BUILD)/objs
LIBS		= $(BUILD)/libs
REPORTS		= $(BUILD)/reports
LOGS		= $(BUILD)/logs
BUILDTREE	= $(BUILD) $(BIN) $(DEPS) $(OBJS) $(LIBS) $(REPORTS) $(LOGS)


###############################################################################
# Environnement de documentation
###############################################################################

DOCS		= docs
DOCSLANG	= French
DOXCONF		= $(DOCS)/doxygen.conf
EXAMPLES	= $(DOCS)/examples
HTML		= $(DOCS)/html
LATEX		= $(DOCS)/latex
DOCSPDF		= $(LATEX)/refman.pdf


###############################################################################
# Paramètres de compilation
###############################################################################

vpath %.c  $(SRCS) $(TESTS)
vpath %.h  $(INCLUDES)
vpath %.o  $(OBJS)
vpath %.so $(LIBS)
vpath %.d  $(DEPS)
vpath %.log  $(LOGS)

CC			= gcc
STD			= gnu99
CFLAGS		= -xc -Wall -Wextra -std=$(STD) $(INCLUDES:%=-I%) -fpic
DFLAGS		= -MMD -MP -MF
SFLAGS		= -shared
LDLIBS	 	= -L $(LIBS) -l $(PROJECT)


###############################################################################
# Paramètres de couverture de code
###############################################################################

vpath %.gcno $(BUILD)

COVFILE		= $(REPORTS)/coverage
COVXML		= $(COVFILE).xml
COVHTML		= $(COVFILE).html
# Limites de couverture de code acceptées en %
COVHIGH		= 98
COVLOW		= 75

COV			= gcovr
COVOPTS		= -r $(SRCS) -u --no-marker --exclude-directories "$(TESTS)"
COVOPTSXML	= --cobertura-pretty --cobertura $(COVXML)
COVOPTSHTML	= --html-details $(COVHTML) \
				--html-medium-threshold $(COVLOW) \
				--html-high-threshold $(COVHIGH) \
				--html-details-syntax-highlighting \
				--html-theme blue \
				--html-title "$(NAME) code coverage report"


###############################################################################
# Fonctions d'aide
###############################################################################

MOCKS		= $(SCRIPTS)/mocks.awk
INFO	 	= $(SCRIPTS)/pinfo
PDOC		= $(SCRIPTS)/pdoc.awk

# Copie l'arbre de répertoires d'un dossier dans la cible
# $(1) Le répertoire à copier
# $(2) Le répertoire cible (final $(2)/$(1))
# $(3) Des options supplémentaires pour rsync
define copytree=
rsync -a $(3) --include "*/" --exclude "*" $(1) $(2)/
endef


###############################################################################
# Cibles à patterns
###############################################################################

$(OBJS)/%.o: %.c
	@$(CC) $(CFLAGS) $(DFLAGS) $(DEPS)/$*.d -c $< -o $@

$(LIBS)/lib%.so: SFLAGS += $(shell $(MOCKS) $(CDEPS)),-soname,lib$*.so
$(LIBS)/lib%.so: $(CDEPS:%.c=$(OBJS)/%.o)
	@$(CC) $(SFLAGS) $^ -o $@.$(VERSION)
	@ln -s $(@:$(LIBS)/%=%).$(VERSION) $@

$(BIN)/%: $(OBJS)/%.o
	@$(CC) $(LDLIBS) $< -o $@

$(LOGS)/%.log: $(BIN)/%
	@LD_LIBRARY_PATH=$(LIBS) $< $(ARGS) &> $@

$(LOGS)/%.difflog: $(LOGS)/%.log
	@git diff --no-index $< $(<:$(LOGS)/%=$(TLOGS)/%) &> $@


###############################################################################
# Autres cibles
###############################################################################

.PHONY: all $(PROJECT) tests unit-tests coverage docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(LIBS)/%.so $(BIN)/%

all: $(PROJECT)

# @brief Compile la cible principale du project (cible par défaut)
$(PROJECT): CFLAGS += -O3
$(PROJECT): init $(LIBS)/$(TARGET)
	@$(INFO) ok $(TARGET) compiled

# @brief Exécute les tests du projet (unitaires, couverture, etc...)
tests: coverage
	@find $(BUILD) -type d -empty -delete
	@$(INFO) ok $@

# Compile, exécute et vérifie les tests unitaires
unit-tests: CFLAGS += -g -O0
unit-tests: LDLIBS += $(shell $(MOCKS) $*.c $(CDEPS))
unit-tests: ARGS    = 0 1 2 3 4 5
unit-tests: tests-init $(LIBS)/$(TARGET) $(CUNITS:%.c=$(LOGS)/%.difflog)
	@rm -rf $(BIN)/*
	@find $(BUILD) -type f -name "*.log" -delete
	@$(INFO) ok $@

# Calcule la couverture de code des tests unitaires
coverage: CFLAGS += --coverage
coverage: SFLAGS += --coverage
coverage: LDLIBS += --coverage
coverage: unit-tests
	@$(COV) $(COVOPTS) $(COVOPTSXML) $(COVOPTSHTML) $(BUILD)
	@find $(BUILD) -type f -name "*.gcno" -delete
	@find $(BUILD) -type f -name "*.gcda" -delete
	@$(INFO) ok $@

export NAME VERSION BRIEF LOGO DOCS EXAMPLES DOCSLANG SRCS INCLUDES TESTS

# @brief Génère la documentation automatisée du projet
docs: init $(DOXCONF)
	@doxygen -q $(DOXCONF)
	@bash -c "make -C $(LATEX) pdf" &>/dev/null
	@mv $(DOCSPDF) $(DOCS)/
	@$(INFO) ok $@

# @brief Initialise la structure du projet
init:
	@mkdir -p $(BUILDTREE)
	@$(call copytree,$(SRCS),$(OBJS))
	@$(call copytree,$(SRCS),$(DEPS))

# Initialise la structure de build des tests
tests-init: init
	@$(call copytree,$(TESTS),$(BIN),--exclude "$(ASSETS)/")
	@$(call copytree,$(TESTS),$(OBJS),--exclude "$(ASSETS)/")
	@$(call copytree,$(TESTS),$(DEPS),--exclude "$(ASSETS)/")
	@$(call copytree,$(TESTS),$(LOGS),--exclude "$(ASSETS)/")

# @brief Nettoyage post-compilation
clean:
	@git clean -q -d -f

# @brief Affiche la documentation du Makefile
help:
	@head -n 5 $(LICENSEFILE)
	@echo "Cibles disponibles:"
	@$(PDOC) Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*/*.d)
