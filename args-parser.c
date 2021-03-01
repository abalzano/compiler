#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "args-parser.h"
#include "scalpa.h"

void print_usage (void) {
    puts(
    "Usage:\n"
    " -version :      display the name of the project members\n"
    " -toq :          display the quadruplet table\n"
    " -tos :          display the symbol table\n"
    " -o <name> :     <name> is the custom output file,\n"
    "                 by default mips program will be writen in output.s\n"
    " -h              display help\n");
}

void display_version (void) {
    puts(
    "\nScalpa Compiler by : \n\n"
    "  BALZANO    Antoine - 2A SDIA\n"
    "  COLIN      Simon   - 2A RIO\n");
}

int test_file (char *name) {
    int fd;
    struct stat statbuf;
    
    if ((access(name, F_OK)) == -1) {
        printf("non existent file - creating one\n");
        CHECK(fd = open(name, O_CREAT | O_RDWR | O_TRUNC, 0666));
        return fd;
    }

    CHECK((lstat(name, &statbuf)));

    switch (statbuf.st_mode & S_IFMT) {
        case S_IFREG:
            break;
        default :
            handle_error("the file is not regular");
    }
    CHECK((access(name, W_OK)));
    CHECK(fd = open(name, O_RDWR | O_TRUNC));

    return fd;
}

void parse_args (int argc, char ** argv, args_t *args) {
    args->file_name = NULL;
    args->flags = NO_FLAGS;
    args->fd = 0;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i],"-version", 8) == 0) {
            args->flags |= VERSION;
        }
        else if (strncmp(argv[i],"-tos", 4) == 0) {
            args->flags |= SYM_TABLE;
        }
        else if (strncmp(argv[i],"-toq", 4) == 0) {
            args->flags |= QUA_TABLE;
        }
        else if (strncmp(argv[i],"-o", 2) == 0) {
            args->flags |= OUT_FILE;
            args->fd = test_file(argv[i+1]);
            args->file_name = argv[i+1];
            i++;
        }
        else if (strncmp(argv[i],"-h", 2) == 0) {
            print_usage();
        }
        else {
            puts("Unknown option\n\n");
            print_usage();
            exit(EXIT_FAILURE);
        }
    }
}
