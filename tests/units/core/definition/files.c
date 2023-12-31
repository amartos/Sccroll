/**
 * @file        files.c
 * @version     0.1.0
 * @brief       Core module unit tests for files handling.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 */

#include <assert.h>

#include "sccroll.h"

#include <fcntl.h>

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

// Format string for tempfiles
#define template "/tmp/sccroll.%s.%x"

static SccrollEffects test = { 0 };

#define contentstr "expected string"
#define errstr "this is not expected"

int openfile(const char* restrict path)
{
    return open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
}

// Prepare the data and files for the test.
void preptests(void)
{
    int fd;
    char path[BUFSIZ] = { 0 };

    for (int i = 0; i < SCCMAX; ++i, memset(path, 0, strlen(path))) {
        sprintf(path, template, "file", i);
        test.files[i].content.blob = i < SCCMAX/2 ? contentstr : errstr;
        test.files[i].content.size = (i % 2) * sizeof(char) * strlen(test.files[i].content.blob);
        test.files[i].path = strdup(path);

        if (i < SCCMAXSTD) {
            memset(path, 0, strlen(path));
            sprintf(path, template, "std", i);
            test.std[i].path = strdup(path);
            // As for standard IO the file is used as a source, first
            // write the tests strings in the tempfile.
            if ((fd = openfile(test.std[i].path)) < 0 ||
                write(fd, contentstr, strlen(contentstr)*sizeof(char)) < 0)
                err(EXIT_FAILURE, "could not write in tmp file %s", test.std[i].path);
            close(fd);
        }
    }
}

void freefile(char* restrict path)
{
    if (remove(path) < 0)
        err(EXIT_FAILURE, "could not remove file at %s", path);
    free(path);
}

void cleantest(void)
{
    for (int i = 0; i < SCCMAX; ++i) {
        freefile((char*)test.files[i].path);
        if (i < SCCMAXSTD) freefile((char*)test.std[i].path);
    }
}

// Fake test operating on files.
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
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{
    preptests();
    test.wrapper = test_dummy;
    test.name = "test dummy";
    sccroll_register(&test);
    test.flags |= NODIFF;
    sccroll_register(&test);
    assert(sccroll_run() == 2);
    cleantest();
    return EXIT_SUCCESS;
}
