#!/usr/bin/awk -f
# @file        pdoc.awk
# @version     0.1.0
# @brief       Script d'affichage de l'aide du Makefile.
# @date        2022
# @author      Alexandre Martos
# @copyright   MIT License
#
# Ce script analyse le Makefile du projet et affiche l'aide issue de
# la documentation.
#
# Les lignes considérées sont les commentaires dont la syntaxe est
# - "^#!.*" Pour la documentation générale
# - "# @brief.*" Pour la documentation d'une recette (le nom de la
# recette est capturé automatiquement à la ligne suivante.)

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
