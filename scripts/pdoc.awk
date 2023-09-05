#!/usr/bin/awk -f
# @file        pdoc.awk
# @version     0.1.0
# @brief       Makefile documentation parser.
# @author      Alexandre Martos
# @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
# @license     MIT License
#
# This script parses a Makefile and prints the recipes documentation.
#
# The printed lines are comments with the following regular
# expressions syntax:
# - "^#!.*" for generic documentation
# - "# @brief.*" for recipes descriptions (the recipe name is captued
# on the next line)

BEGIN { FS="(#!|# @brief)"; }

/^#!/ {
    doc = $2;
    sub(/^ */,"",doc);
    print doc;
}

/^# @brief/ {
    doc = $2;
    getline;
    cmd = $0;

    sub(/^ */,"",doc);
    sub(/\t*:.*/,"",cmd);
    printf "%s\t%s\n",cmd,doc;
}
