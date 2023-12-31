###############################################################################
# Project infos
###############################################################################

LICENSEFILE	= LICENSE
LICENSE 	= License MIT
BRIEF		= Sccroll -- a units tests library for C
COPYRIGHT	= Copyright 2023 Alexandre Martos <contact@amartos.fr>
NAME		= $(firstword $(BRIEF))
PROJECT 	= $(shell echo $(NAME) | tr "[:upper:]" "[:lower:]")
VERSION		= $(shell find . -type f -name "$(PROJECT).[h|c]" | xargs grep version | awk '{print $$NF}')
LOGO		=


###############################################################################
# Environment
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
PCOV		:= $(SCRIPTS)/pcov.sh

BUILD		= build
LIBS		= $(BUILD)/libs
BIN			:= $(BUILD)/bin
LIBS		:= $(BUILD)/lib
DEPS		:= $(BUILD)/deps
OBJS		:= $(BUILD)/objs
LOGS		:= $(BUILD)/logs
REPORTS		:= $(BUILD)/reports

PREFIX		?= /usr/local
LIBINSTALL	:= $(PREFIX)/lib
INCINSTALL	:= $(PREFIX)/include


###############################################################################
# Sources
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
# Compilation parameters
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
# Code coverage
###############################################################################

COVFILE		:= $(REPORTS)/coverage
COVXML		:= $(COVFILE).xml
COVHTML		:= $(COVFILE).html

# Accepted limits for coverage in %
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
# Documentation
###############################################################################

DOCS		= docs
DOCSLANG	= English
DOX			= doxygen
DOXCONF		:= $(DOCS)/$(DOX).conf
DOXOPTS		:= -q $(DOXCONF)
EXAMPLES	:= $(DOCS)/examples
HTML		:= $(DOCS)/html
LATEX		:= $(DOCS)/latex
PDF			:= $(LATEX)/refman.pdf


###############################################################################
# Patterns recipes
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

# This recipe is used when building a unit test and the test log is
# not yet available.
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
# Other recipes
###############################################################################

.PHONY: all $(PROJECT) install tests docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(LIBS)/%.so $(LOGS)/%.difflog $(TLOGS)/%.log

# @brief Compile the library
all: $(PROJECT)

$(PROJECT): CFLAGS += -O3
$(PROJECT): %: clean init $(LIBS)/lib%.so
	@$(INFO) ok $@ compiled

# @brief Compile the debug version of the library
debug: CFLAGS += -g -DDEBUG
debug: $(PROJECT)
	@$(INFO) ok $(PROJECT) $@ version compiled

# @brief Compile and install the library
install: $(PROJECT)
	@sudo mkdir -p $(LIBINSTALL) $(INCINSTALL)
	@sudo rsync -aq $(LIBS)/ $(LIBINSTALL)/
	@sudo rsync -aq $(INCLUDES)/ $(INCINSTALL)/
	@$(INFO) ok $(PROJECT) installed

# @brief Execute the tests: units tests, coverage
tests: CFLAGS += -g -O0 -DDEBUG -D_SCCUNITTESTS --coverage
tests: LDLIBS += --coverage
tests: SFLAGS += --coverage
tests: ARGS    = 0 1 2 3 4 5
tests: clean init $(LIBS)/lib$(PROJECT).so $(UDEPS:%.c=$(LOGS)/%.difflog)
	@$(COV) $(COVOPTS) $(COVOPTSXML) $(COVOPTSHTML) $(BUILD)
	@find $(BUILD) \( -name "*.gcno" -or -name "*.gcda" -or -empty \) -delete
	@$(INFO) ok $(PROJECT) coverage
	@$(PCOV) $(COVXML)

export NAME VERSION BRIEF LOGO DOCS EXAMPLES DOCSLANG SRCS INCLUDES TESTS

# @brief Build the project documentation
docs: $(DOXCONF)
	@$(DOX) $(DOXOPTS)
	@$(MAKE) -C $(LATEX) pdf && mv $(PDF) $(DOCS)/
	@$(INFO) ok $(PROJECT) $@

# @brief Initialize the compilation directory
init:
	@mkdir -p $(BIN) $(OBJS) $(LOGS) $(DEPS) $(LIBS) $(REPORTS)

# @brief Nuke all files not in VCS
clean:
	@git clean -q -d -f

# @brief Print the Makefile documentation
help:
	@echo '$(BRIEF)'
	@echo '$(COPYRIGHT)'
	@echo '$(LICENSE)'
	@echo -e "Available recipes:\n"
	@$(PDOC) Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*/*.d)
