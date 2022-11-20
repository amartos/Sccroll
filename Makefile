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
ERROR	 	:= $(SCRIPTS)/pinfo error
INFO		:= $(SCRIPTS)/pinfo info
PASS		:= $(SCRIPTS)/pinfo ok
BUILD		:= build
BIN		:= $(BUILD)/bin
DEPS		:= $(BUILD)/deps
OBJS		:= $(BUILD)/objs
SHARED		:= $(BUILD)/libs
REPORTS	:= $(BUILD)/reports
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
# Paramètres de tests
###############################################################################

# Limites de couverture de code acceptées en %
COVHIGH	:= 98
COVLOW	:= 75

COV		:= gcovr
COVOPTS	:= -r $(SRCS) -u --no-marker -e ".*_tests.c"
COVXML		:= --cobertura-pretty --cobertura $(REPORTS)/coverage.xml
COVHTML :=	--html-details $(REPORTS)/coverage.html \
			--html-medium-threshold $(COVLOW) --html-high-threshold $(COVHIGH) \
			--html-details-syntax-highlighting --html-theme blue \
			--html-title "$(PROJECT) code coverage report"


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

.PHONY: all $(PROJECT) unit-tests code-coverage doc init clean help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(SHARED)/%.so

all: $(PROJECT)

# @brief Compile la librairie (cible par défaut)
$(PROJECT): init lib$(PROJECT).so

# @brief Exécute les tests du projet (unitaires, couverture, etc...)
unit-tests: TMP := $(shell mktemp -d -p /tmp $(PROJECT).XXX)
unit-tests: LOG = $(TMP)/$(PROJECT)_tests.log
unit-tests: EXE = $(BIN)/$(PROJECT)_tests
unit-tests: CFLAGS += --coverage -g -O0
unit-tests: LDLIBS += --coverage -lgcov
unit-tests: init $(PROJECT) $(BIN)/$(PROJECT)_tests
	@LD_LIBRARY_PATH=$(SHARED) $(EXE) &> $(LOG)
	@grep -q LAST. $(LOG) || $(ERROR) $@ last line not outputed last.
	@! grep -q invisible $(LOG) || $(ERROR) $@ captured output visible in log.
	@grep -q test_fail $(LOG) || $(ERROR) $@ failed test log not outputted.
	@(grep -q custom $(LOG) && grep -q Another $(LOG)) || $(ERROR) $@ custom test name not outputted
	@$(PASS) $@

# @brief Génère un rapport sur la couverture de code des tests.
code-coverage: init $(SHARED)/lib$(PROJECT).gcno
	@$(COV) $(COVOPTS) $(COVXML) $(COVHTML) $(BUILD)
	@$(PASS) $@

# @brief Génère la documentation du projet
doc:

# @brief Initialise la structure du projet
init:
	@mkdir -p $(SRCS) $(INCLUDES) $(TESTS) $(SCRIPTS)
	@mkdir -p $(BUILD) $(BIN) $(OBJS) $(DEPS) $(SHARED) $(REPORTS)
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
