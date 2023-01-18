/**
 * @file        files.c
 * @version     0.1.0
 * @brief       Tests unitaires d'analyse des modifications de fichiers.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/definition/files.c \
 *     -o build/objs/tests/units/core/definition/files.o
 * gcc -L build/libs -lsccroll \
 *     build/objs/tests/units/core/definition/files.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/core/definition/files
 * @endcode
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

#include <fcntl.h>

// clang-format off

/******************************************************************************
 * Préparatifs des tests unitaires.
 ******************************************************************************/
// clang-format on

// Format du nom de fichier temporaire.
#define template "/tmp/sccroll.%s.%x"

// Test unitaire factice.
static SccrollEffects test = { 0 };

// Chaînes testées.
#define contentstr "expected string"
#define errstr "this is not expected"

// Ouvre un fichier avec les droits lectures / écriture et renvoie le
// descripteur.
int openfile(const char* restrict path)
{
    return open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
}

// Prépare les données et fichiers du test.
void preptests(void)
{
    int fd;
    char path[BUFSIZ] = { 0 };

    for (int i = 0; i < SCCMAX; ++i, memset(path, 0, strlen(path))) {
        sprintf(path, template, "file", i);
        test.files[i].content = i < SCCMAX/2 ? strdup(contentstr) : strdup(errstr);
        test.files[i].path = strdup(path);

        if (i < SCCMAXSTD) {
            memset(path, 0, strlen(path));
            sprintf(path, template, "std", i);
            test.std[i].path = strdup(path);
            // Pour les E/S standard, le fichier sert de source, on
            // inscrit donc ce qui est attendu dans le fichier.
            if ((fd = openfile(test.std[i].path)) < 0 ||
                write(fd, contentstr, strlen(contentstr)*sizeof(char)) < 0)
                err(EXIT_FAILURE, "could not write in tmp file %s", test.std[i].path);
            close(fd);
        }
    }
}

// Supprime le fichier et libère path.
void freefile(char* restrict path)
{
    if (remove(path) < 0)
        err(EXIT_FAILURE, "could not remove file at %s", path);
    free(path);
}

// Libère les données et ferme les fichiers de files.
void cleantest(void)
{
    for (int i = 0; i < SCCMAX; ++i) {
        freefile((char*)test.files[i].path);
        if (i < SCCMAXSTD) freefile((char*)test.std[i].path);
    }
}

// Test unitaire factice effectuant les opérations avec le contenu de
// files.
void test_dummy(void)
{
    int fd;
    char buffer[SCCMAX] = { 0 };
    for (int i = 0; i < SCCMAX; ++i) {
        if (i < SCCMAXSTD)
            switch(i)
            {
            case STDIN_FILENO:
                fread(buffer, sizeof(char), SCCMAX, stdin);
                assert(!strcmp(contentstr, buffer));
                break;
            case STDOUT_FILENO: fprintf(stdout, "%s\n", contentstr); break;
            case STDERR_FILENO: fprintf(stderr, "%s\n", contentstr); break;
            default: break;
            }
        if ((fd = openfile(test.files[i].path)) < 0 ||
            write(fd, contentstr, strlen(contentstr)*sizeof(char)) < 0 ||
            close(fd) < 0)
            err(EXIT_FAILURE, "in test: could not write in tmp file %s", test.files[i].path);
    }
}

// clang-format off

/******************************************************************************
 * Exécution des tests
 ******************************************************************************/
// clang-format on

int main(void)
{
    preptests();
    test.wrapper = test_dummy;
    test.name = "test dummy";
    sccroll_register(&test);
    assert(sccroll_run() == 1);
    cleantest();
    return EXIT_SUCCESS;
}
