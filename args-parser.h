#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

typedef enum flags_t {
    NO_FLAGS = 0,
    VERSION = 1,
    SYM_TABLE = 1 << 1,
    OUT_FILE = 1 << 2,
    QUA_TABLE = 1 << 3,
} flags_t;

typedef struct args_t {
    int fd;
    char * file_name;
    flags_t flags;
} args_t;

/**
 * @brief Option parsing
 * -version     : members of the project
 * -tos         : prints symbol table
 * -o <name>    : result file
 * -h           : prints possible options
 * @param argc (standard main argument)
 * @param argv (standard main argument)
 * @param args will receive the argument values
 */
void parse_args (int argc,  char ** argv, args_t *args);

/**
 * @brief print name and option of all project members 
 */
void display_version (void);

/**
 * @brief print the usage
 */
void print_usage (void);

/**
 * @brief execute all the necessary tests on a file name
 * This function checks if the user' file exists, if it is regular,
 * if it is accessible. If not the programme is terminated.
 * The function opens the file if it is non existent.
 * @param name path of the file given by the user
 * @return the file descriptor
 */
int test_file(char *name);

#endif

