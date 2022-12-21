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
UNITS		= $(shell find $(TESTS) -type f -name "*.c")

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

# Cherche une chaîne dans un fichier et lève une erreur si elle n'est
# pas trouvée.
# $(1) Le nom du test.
# $(2) La chaîne recherchée.
# $(3) Le nom du fichier.
# $(4) Des commandes à effectuer avant la recherche.
define assertLogHas =
$(4) grep -q $(2) $(3) || $(INFO) error $(1) "Not found in log:" $(2)
endef

# Copie l'arbre de répertoires d'un dossier dans la cible
# $(1) Le répertoire à copier
# $(2) Le répertoire cible (final $(2)/$(1))
define copytree=
rsync -a --include "*/" --exclude "*" $(1) $(2)/
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


###############################################################################
# Autres cibles
###############################################################################

.PHONY: all $(PROJECT) unit-tests coverage docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(LIBS)/%.so $(BIN)/%

all: $(PROJECT)

# @brief Compile la cible principale du project (cible par défaut)
$(PROJECT): CFLAGS += -O3
$(PROJECT): init $(LIBS)/$(TARGET)
	@$(INFO) ok $(TARGET) compiled

# @brief Exécute les tests du projet (unitaires, couverture, etc...)
unit-tests: CFLAGS += --coverage -g -O0
unit-tests: SFLAGS += --coverage
unit-tests: LDLIBS += --coverage $(shell $(MOCKS) $*.c)
unit-tests: ARGS 	= $(shell for ((n=0; $$n<($$RANDOM % 100); n = ($$n+1))); do echo -e $$n; done)
unit-tests: $(LIBS)/$(TARGET) $(UNITS:%.c=$(LOGS)/%.log)
	@$(call assertLogHas,"basics", ">>>>>> First line of tests.",,head -n 1 $(LOGS)/$(TESTS)/$(PROJECT)_basics_tests.log |)
	@$(call assertLogHas,"basics", ">>>>>> Last line of tests.",,tail -n 1 $(LOGS)/$(TESTS)/$(PROJECT)_basics_tests.log |)
	@$(call assertLogHas,"main", "Main executed with $(words $(ARGS)) arguments: \[ $(ARGS) \]",$(LOGS)/$(TESTS)/$(PROJECT)_main_tests.log)
	@$(call assertLogHas,"mocks", "calloc mocked.",$(LOGS)/$(TESTS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks", "free mocked",$(LOGS)/$(TESTS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","sccroll_run mocked: nothing executed",$(LOGS)/$(TESTS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","sccroll_run mocked: flag seen.",$(LOGS)/$(TESTS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","printf not mocked: OK",$(LOGS)/$(TESTS)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","Assertion",$(LOGS)/$(TESTS)/$(PROJECT)_mocks_tests.log,!)
	@$(call assertLogHas,"asserts","this test must fail successfully",$(LOGS)/$(TESTS)/$(PROJECT)_asserts_tests.log);
	@$(call assertLogHas,"asserts","l.",$(LOGS)/$(TESTS)/$(PROJECT)_asserts_tests.log);
	@$(call assertLogHas,"asserts","invisible line",$(LOGS)/$(TESTS)/$(PROJECT)_asserts_tests.log, !);
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
	@$(call copytree,$(SRCS),$(OBJS))
	@$(call copytree,$(SRCS),$(DEPS))
	@$(call copytree,$(TESTS),$(BIN))
	@$(call copytree,$(TESTS),$(OBJS))
	@$(call copytree,$(TESTS),$(DEPS))
	@$(call copytree,$(TESTS),$(LOGS))

# @brief Nettoyage post-compilation
clean:
	@git clean -q -d -f
	@$(INFO) ok $@

# @brief Affiche la documentation du Makefile
help:
	@head -n 5 $(LICENSEFILE)
	@echo "Cibles disponibles:"
	@$(PDOC) Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*/*.d)
