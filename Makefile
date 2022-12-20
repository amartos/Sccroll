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
DOCSLANG	= French


###############################################################################
# Environnement
###############################################################################

SHELL		:= /usr/bin/bash
SRCS		:= src
INCLUDES	:= include
TESTS		:= tests
TESTSPTRN	:= *_tests
_UNITS		:= $(wildcard $(TESTS)/$(TESTSPTRN).c)
UNITS		:= $(_UNITS:$(TESTS)/%.c=%)
TMP			:= /tmp/$(PROJECT)
SCRIPTS		:= scripts
BUILD		:= build
BIN		:= $(BUILD)/bin
DEPS		:= $(BUILD)/deps
OBJS		:= $(BUILD)/objs
SHARED		:= $(BUILD)/libs
REPORTS	:= $(BUILD)/reports
DOCS		:= docs
EXAMPLES	:= $(DOCS)/examples
DOXCONF		:= $(DOCS)/doxygen.conf
HTML		:= $(DOCS)/html
LATEX		:= $(DOCS)/latex

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
CFLAGS		:= -xc -Wall -Wextra -std=$(STD) $(INCLUDES:%=-I%) $(shell $(MOCKS) $(SRCS)/$(PROJECT).c) -c
LDLIBS	 	:= -L $(SHARED) -l $(PROJECT)
DEPFLAGS	:= -MMD -MP -MF


###############################################################################
# Paramètres de tests
###############################################################################

# Limites de couverture de code acceptées en %
COVHIGH	:= 98
COVLOW	:= 75

COV		:= gcovr
COVOPTS	:= -r $(SRCS) -u --no-marker -e ".$(TESTSPTRN).c"
COVXML		:= --cobertura-pretty --cobertura $(REPORTS)/coverage.xml
COVHTML :=	--html-details $(REPORTS)/coverage.html \
			--html-medium-threshold $(COVLOW) --html-high-threshold $(COVHIGH) \
			--html-details-syntax-highlighting --html-theme blue \
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
	@$(CC) $(CFLAGS) -fpic -shared $(DEPFLAGS) $(DEPS)/$*.d $< -o $(SHARED)/$@

$(BIN)/%: %.o
	@$(CC) $(OBJS)/$*.o $(LDLIBS) -o $@

%.log: $(BIN)/%
	@mkdir -p $(TMP)
	@LD_LIBRARY_PATH=$(SHARED) $< $(ARGS) &> $(TMP)/$@


###############################################################################
# Autres cibles
###############################################################################

.PHONY: all $(PROJECT) unit-tests coverage docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(SHARED)/%.so

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
	@$(call assertLogHas,"basics", ">>>>>> First line of tests.",,head -n 1 $(TMP)/$(PROJECT)_basics_tests.log |)
	@$(call assertLogHas,"basics", ">>>>>> Last line of tests.",,tail -n 1 $(TMP)/$(PROJECT)_basics_tests.log |)
	@$(call assertLogHas,"main", "Main executed with $(words $(ARGS)) arguments: \[ $(ARGS) \]",$(TMP)/$(PROJECT)_main_tests.log)
	@$(call assertLogHas,"mocks", "calloc mocked.",$(TMP)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks", "free mocked",$(TMP)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","sccroll_run mocked: nothing executed",$(TMP)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","sccroll_run mocked: flag seen.",$(TMP)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","printf not mocked: OK",$(TMP)/$(PROJECT)_mocks_tests.log)
	@$(call assertLogHas,"mocks","Assertion",$(TMP)/$(PROJECT)_mocks_tests.log,!)
	@$(call assertLogHas,"asserts","this test must fail successfully",$(TMP)/$(PROJECT)_asserts_tests.log);
	@$(call assertLogHas,"asserts","l.",$(TMP)/$(PROJECT)_asserts_tests.log);
	@$(call assertLogHas,"asserts","invisible line",$(TMP)/$(PROJECT)_asserts_tests.log, !);
	@rm -r $(TMP)
	@$(INFO) ok $@

# @brief Génère un rapport sur la couverture de code des tests.
coverage: unit-tests $(SHARED)/lib$(PROJECT).gcno
	@rm -rf $(REPORTS)
	@mkdir -p $(REPORTS)
	@$(COV) $(COVOPTS) $(COVXML) $(COVHTML) $(BUILD)
	@$(INFO) ok $@

export NAME VERSION BRIEF LOGO DOCS EXAMPLES DOCSLANG SRCS INCLUDES TESTS

# @brief Génère la documentation automatisée du projet
docs: init html pdf
	@(INFO) ok $@

$(LATEX)/Makefile $(HTML)/index.html: $(DOXCONF)
	@doxygen -q $(DOXCONF)

html: $(HTML)/index.html

pdf: $(LATEX)/Makefile
	@bash -c "make -C $(DOCS)/latex pdf" &>/dev/null
	@mv $(DOCS)/latex/refman.pdf $(DOCS)
	@$(INFO) ok $@

# @brief Initialise la structure du projet
init:
	@mkdir -p $(SRCS) $(INCLUDES) $(TESTS) $(SCRIPTS)
	@mkdir -p $(BUILD) $(BIN) $(OBJS) $(DEPS) $(SHARED) $(REPORTS)
	@mkdir -p $(DOCS)

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
