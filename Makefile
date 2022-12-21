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

UNITS		= $(wildcard $(TESTS)/*.c)
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

CC		:= gcc
STD		:= gnu99
CFLAGS		:= -xc -Wall -Wextra -std=$(STD) $(INCLUDES:%=-I%) $(shell $(MOCKS) $(SRCS)/$(PROJECT).c) -c
DEPFLAGS	:= -MMD -MP -MF
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

# Cherche une chaîne dans un fichier et lève une erreur si elle n'est
# pas trouvée.
# $(1) Le nom du test.
# $(2) La chaîne recherchée.
# $(3) Le nom du fichier.
# $(4) Des commandes à effectuer avant la recherche.
define assertLogHas =
$(4) grep -q $(2) $(3) || $(INFO) error $(1) "Not found in log:" $(2)
endef


###############################################################################
# Cibles à patterns
###############################################################################

%.o: %.c
	@$(CC) $(CFLAGS) $(DEPFLAGS) $(DEPS)/$*.d $< -o $(OBJS)/$@

lib%.so: %.c
	@$(CC) $(CFLAGS) -fpic -shared $(DEPFLAGS) $(DEPS)/$*.d $< -o $(LIBS)/$@

$(BIN)/%: %.o
	@$(CC) $(OBJS)/$*.o $(LDLIBS) -o $@

%.log: $(BIN)/%
	@LD_LIBRARY_PATH=$(LIBS) $< $(ARGS) &> $(LOGS)/$@


###############################################################################
# Autres cibles
###############################################################################

.PHONY: all $(PROJECT) unit-tests coverage docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(LIBS)/%.so

all: $(PROJECT)

# @brief Compile la librairie (cible par défaut)
$(PROJECT): init lib$(PROJECT).so
	@$(INFO) ok $(TARGET) compiled

# @brief Exécute les tests du projet (unitaires, couverture, etc...)
unit-tests: CFLAGS += --coverage -g -O0
unit-tests: LDLIBS += --coverage -lgcov
unit-tests: LDLIBS += $(shell $(MOCKS) $(TESTS)/$*.c $(SRCS)/$(PROJECT).c)
unit-tests: ARGS := $(shell for ((n=0; $$n<($$RANDOM % 100); n = ($$n+1))); do echo -e $$n; done)
unit-tests: init $(PROJECT) $(UNITS:%=$(BIN)/%) $(UNITS:%=%.log)
	@$(call assertLogHas,"basics", ">>>>>> First line of tests.",,head -n 1 $(LOGS)/$(PROJECT)_basics_tests.log |)
	@$(call assertLogHas,"basics", ">>>>>> Last line of tests.",,tail -n 1 $(LOGS)/$(PROJECT)_basics_tests.log |)
	@$(call assertLogHas,"main", "Main executed with $(words $(ARGS)) arguments: \[ $(ARGS) \]",$(LOGS)/$(PROJECT)_main_tests.log)
	@$(call assertLogHas,"mocks", "calloc mocked.",$(LOGS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks", "free mocked",$(LOGS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","sccroll_run mocked: nothing executed",$(LOGS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","sccroll_run mocked: flag seen.",$(LOGS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","printf not mocked: OK",$(LOGS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","Assertion",$(LOGS)/$(PROJECT)_mocks_tests.log,!)
	@$(call assertLogHas,"asserts","this test must fail successfully",$(LOGS)/$(PROJECT)_asserts_tests.log);
	@$(call assertLogHas,"asserts","l.",$(LOGS)/$(PROJECT)_asserts_tests.log);
	@$(call assertLogHas,"asserts","invisible line",$(LOGS)/$(PROJECT)_asserts_tests.log, !);
	@$(INFO) ok $@

# @brief Génère un rapport sur la couverture de code des tests.
coverage: unit-tests
	@$(COV) $(COVOPTS) $(COVOPTSXML) $(COVOPTSHTML) $(BUILD)
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

# @brief Nettoyage post-compilation
clean:
	@git clean -q -d -f
	@$(INFO) ok $@

# @brief Affiche la documentation du Makefile
help:
	@head -n 5 $(LICENSEFILE)
	@echo "Cibles disponibles:"
	@$(PDOC) Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*.d)
