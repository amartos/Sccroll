#!/usr/bin/awk -f
# @file        mocks.awk
# @version     0.1.0
# @brief       Script d'aide à la compilation de la librairie Sccroll.
# @date        2022
# @author      Alexandre Martos
# @copyright   MIT License
#
# Ce script analyse les fichiers qui lui sont confiés et génère
# l'option de compilation -Wl correspondante pour l'ensemble des mocks
# détectés.

BEGIN { ORS=""; FS=",[\n\t ]*"; }

/^SCCROLL_MOCK(.*)$/ {
    if (length($2) > 0) {
        if (f == 0) {
            print "-Wl";
            f = 1;
        }
        print ",--wrap,"$2;
    }
}
