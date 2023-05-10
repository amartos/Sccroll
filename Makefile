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
UNITS		:= $(TESTS)/units
ASSETS		:= $(TESTS)/assets
TLOGS		:= $(ASSETS)/logs

SCRIPTS		= scripts
INFO	 	:= $(SCRIPTS)/pinfo
PDOC		:= $(SCRIPTS)/pdoc.awk

BUILD		= build
LIBS		= $(BUILD)/libs
BIN			:= $(BUILD)/bin
LIBS		:= $(BUILD)/lib
DEPS		:= $(BUILD)/deps
OBJS		:= $(BUILD)/objs
LOGS		:= $(BUILD)/logs
REPORTS		:= $(BUILD)/reports

PREFIX		?= ~/.local
LIBINSTALL	:= $(PREFIX)/lib/$(PROJECT)
INCINSTALL	:= $(PREFIX)/include/$(PROJECT)


###############################################################################
# Chemins des sources
###############################################################################

SRCTREE		:= $(shell find $(SRCS) -type d)
UNITREE		:= $(shell find $(UNITS) -type d)
HDRTREE		:= $(shell find $(INCLUDES) -type d)
CPPTREE		:= $(SRCTREE) $(UNITREE)

vpath %.h    $(HDRTREE)
vpath %.c    $(CPPTREE)
vpath %.so   $(LIBS)
vpath %.log  $(LOGS)
vpath %.gcno $(BUILD)


###############################################################################
# Paramètres de compilation
###############################################################################

CDEPS		:= $(shell find $(SRCS) -type f -name "*.c")
UDEPS		:= $(shell find $(UNITS) -type f -name "*.c")

CC			= gcc
CFLAGS		:= $(shell cat compile_flags.txt)
DFLAGS		= -MMD -MP -MF
SFLAGS		= -shared
LDLIBS	 	= -L $(LIBS) -l$(PROJECT) -ldl
LIBPATH		:= $(LIBS):$(LIBINSTALL):/usr/local/lib


###############################################################################
# Paramètres de couverture de code
###############################################################################

COVFILE		:= $(REPORTS)/coverage
COVXML		:= $(COVFILE).xml
COVHTML		:= $(COVFILE).html

# Limites de couverture de code acceptées en %
COVHIGH		= 98
COVLOW		= 75

COV			= gcovr
COVEXCL		:= --exclude-directories "$(TESTS)" \
				-e ".*\.(tab|yy)\.c" \
				-e ".*\.l"
COVOPTS		:= -r $(SRCS) -u $(COVEXCL)
COVOPTSXML	:= --xml-pretty --xml $(COVXML)
COVOPTSHTML	:= --html-details $(COVHTML) \
				--html-medium-threshold $(COVLOW) \
				--html-high-threshold $(COVHIGH) \
				--html-title "$(NAME) code coverage report"


###############################################################################
# Paramètres de documentation
###############################################################################

DOCS		= docs
DOCSLANG	= French
DOX			= doxygen
DOXCONF		:= $(DOCS)/$(DOX).conf
DOXOPTS		:= -q $(DOXCONF)
EXAMPLES	:= $(DOCS)/examples
HTML		:= $(DOCS)/html
LATEX		:= $(DOCS)/latex
PDF			:= $(LATEX)/refman.pdf


###############################################################################
# Cibles à patterns
###############################################################################

$(OBJS)/%.o: %.c
	@mkdir -p $(dir $@) $(DEPS)/$(dir $*)
	@$(CC) $(CFLAGS) $(DFLAGS) $(DEPS)/$*.d -c $< -o $@

$(LIBS)/lib%.so: SFLAGS += -Wl,-soname,lib$*.so
$(LIBS)/lib%.so: $(CDEPS:%.c=$(OBJS)/%.o)
	@mkdir -p $(dir $@)
	@$(CC) $(SFLAGS) $^ -o $@.$(VERSION)
	@ln -s $(@:$(LIBS)/%=%).$(VERSION) $@

$(BIN)/%: $(OBJS)/%.o
	@mkdir -p $(dir $@)
	@$(CC) $(LDLIBS) $^ -o $@

$(LOGS)/%.log: $(BIN)/%
	@mkdir -p $(dir $@)
	@LD_LIBRARY_PATH=$(LIBPATH) $< $(ARGS) &> $@ \
		&& $(INFO) pass $(notdir $*) \
		|| ($(INFO) fail $(notdir $*); true)

# Cette recette ne devrait pas être souvent utilisée. Elle existe pour
# le cas où l'on est en train de construire un test unitaire, et que
# le premier log est inexistant.
$(TLOGS)/%.log:
	@mkdir -p $(dir $@)
	@touch $@

$(LOGS)/%.difflog: $(TLOGS)/%.log $(LOGS)/%.log
	@mkdir -p $(dir $@)
	@git diff --no-index $^ > $@ \
		|| (sed -i "s+$(BUILD)+$(ASSETS)+g" $@ \
			&& $(INFO) error $(notdir $* log); true)
	@rm $(BIN)/$* $(LOGS)/$*.log


###############################################################################
# Autres cibles
###############################################################################

.PHONY: all $(PROJECT) install tests docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(LIBS)/%.so $(LOGS)/%.difflog $(TLOGS)/%.log

# @brief Compile la cible principale du project
all: $(PROJECT)

# Compile la cible principale du project (cible par défaut)
$(PROJECT): CFLAGS += -O3
$(PROJECT): %: clean init $(LIBS)/lib%.so
	@$(INFO) ok $@ compiled

# @brief Compile la cible principale avec fonctionnalités de debuggage
debug: CFLAGS += -g -DDEBUG
debug: $(PROJECT)
	@$(INFO) ok $(PROJECT) $@ version compiled

# @brief Installe le logiciel compilé sur le système.
install: $(PROJECT)
	@mkdir -p $(LIBINSTALL) $(INCINSTALL)
	@rsync -aq $(LIBS)/ $(LIBINSTALL)/
	@rsync -aq $(INCLUDES)/ $(INCINSTALL)/
	@$(INFO) ok $(PROJECT) installed

# @brief Exécute les tests du projet (unitaires, couverture, etc...)
tests: CFLAGS += -g -O0 -D_SCCUNITTESTS --coverage
tests: LDLIBS += --coverage
tests: SFLAGS += --coverage
tests: ARGS    = 0 1 2 3 4 5
tests: clean init $(LIBS)/lib$(PROJECT).so $(UDEPS:%.c=$(LOGS)/%.difflog)
	@$(COV) $(COVOPTS) $(COVOPTSXML) $(COVOPTSHTML) $(BUILD)
	@find $(BUILD) \( -name "*.gcno" -or -name "*.gcda" -or -empty \) -delete
	@$(INFO) ok $(PROJECT) coverage

export NAME VERSION BRIEF LOGO DOCS EXAMPLES DOCSLANG SRCS INCLUDES TESTS

# @brief Génère la documentation automatisée du projet
docs: $(DOXCONF)
	@$(DOX) $(DOXOPTS)
	@$(MAKE) -C $(LATEX) pdf && mv $(PDF) $(DOCS)/
	@$(INFO) ok $(PROJECT) $@

# @brief Initialise le dossier de compilation
init:
	@mkdir -p $(BIN) $(OBJS) $(LOGS) $(DEPS) $(LIBS) $(REPORTS)

# @brief Nettoyage post-compilation
clean:
	@git clean -q -d -f

# @brief Affiche la documentation du Makefile
help:
	@head -n 5 $(LICENSEFILE)
	@echo "Cibles disponibles:"
	@$(PDOC) Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*/*.d)
