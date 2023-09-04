#!/usr/bin/env bash
# @file        pcov.sh
# @version     0.1.0
# @brief       Coverage printer.
# @date        2023
# @author      Alexandre Martos
# @copyright   MIT License
#
# @parblock
# This script prints a formatted table of the overall project coverage
# percentages.
# Its parameters are:
# - the XML coverage file path
# @endparblock

FORMAT="%-10s| %10s | %-5.5s | %-8.*s |\n"
RULEFMT="%-10s|------------+-------+----------|\n"

printf "$FORMAT" " " "source" "lines" 10 "branches"
printf "$RULEFMT" " "
awk 'BEGIN {FS="(=\"|\" )"} /<class / {printf "'"$FORMAT"'", " ", $2, $6, 5, $8}' $1
