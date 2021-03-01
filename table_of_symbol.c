#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "scalpa.h"
#include "quad.h"
#include "linked_list.h"
#include "table_of_symbol.h"

extern struct symbol_table_t symbol_table;
extern struct quad_table_t quad_table;

void init_symbol_table() {
    symbol_table.table_size = INIT_TABLE_SIZE;
    symbol_table.last_ident_index = 0;
    symbol_table.symbols = malloc(INIT_TABLE_SIZE * sizeof(struct symbol_t));
    MCHECK(symbol_table.symbols);
    symbol_table.cur_symbol_scope = 0;
    symbol_table.quad_main = 0;
}

void free_symbol_table() {
    for (int i = 0; i < symbol_table.last_ident_index; i++) {
        free(symbol_table.symbols[i].ident);
        switch (symbol_table.symbols[i].var_func_par) {
        case VAR_T:
            if (symbol_table.symbols[i].type.var.typename->range_list != NULL){
                free(symbol_table.symbols[i].type.var.typename->range_list);
            }
            if (symbol_table.symbols[i].type.var.typename->range_array != NULL){
                free(symbol_table.symbols[i].type.var.typename->range_array);
            }
            free(symbol_table.symbols[i].type.var.typename);
            break;
        case FUNC_T:
            if (symbol_table.symbols[i].type.func.index_param != NULL) {
                free(symbol_table.symbols[i].type.func.index_param);
            }
            break;
        case PARAM_T:
            if (symbol_table.symbols[i].type.param.typename->range_list 
                != NULL) {
                free(symbol_table.symbols[i].type.param.typename->range_list);
            }
            if (symbol_table.symbols[i].type.param.typename->range_array 
                != NULL) {
                free(symbol_table.symbols[i].type.param.typename->range_array);
            }
            free(symbol_table.symbols[i].type.param.typename);
            break;
        }
    }
    free(symbol_table.symbols);
}

void display_symbol(struct symbol_t *symbol, int index, int n) {
    char atomic_type[5];
    if (symbol->var_func_par == FUNC_T) {
        printf("\n");
    }
    for (int j = 0; j < n; j++) {
        printf(" ");
    }
    switch (symbol->var_func_par) {
    case VAR_T:
        switch (symbol->type.var.typename->atomic_type) {
            case VOID_A : strncpy(atomic_type, "unit", 5); break;
            case BOOL_A : strncpy(atomic_type, "bool", 5); break;
            case INT_A  : strncpy(atomic_type, "int ", 5); break;
            default     : strncpy(atomic_type, "??? ", 5); break;
        }
        printf("| variable  | %s        | %s", atomic_type, symbol->ident);
        if (symbol->type.var.typename->symbol_type == ARRAY_TYPE) {
            printf(" ( rangelist[");
            for (int j = 0; j < symbol->type.var.typename->len_range_list; j++){
               printf("%i..%i,", symbol->type.var.typename->range_array[j][0],
                   symbol->type.var.typename->range_array[j][1]);
            }
            printf("] )");
        }
        break;
    
    case FUNC_T:
        switch (symbol->type.func.atomic_type) {
            case VOID_A : strncpy(atomic_type, "unit", 5); break;
            case BOOL_A : strncpy(atomic_type, "bool", 5); break;
            case INT_A  : strncpy(atomic_type, "int ", 5); break;
            default     : strncpy(atomic_type, "??? ", 5); break;
        }
        printf("| function  | %s        | %s", atomic_type, symbol->ident);
        printf(" ( parameters[");
        for (int j = 0; j < symbol->type.func.nb_param; j++) {
            printf("%i,", symbol->type.func.index_param[j]);
        }
        printf("] )");
        printf(" quad = [%i,%i[", 
            symbol->type.func.quad_start, symbol->type.func.quad_end);
        break;

    case PARAM_T:
        switch (symbol->type.param.typename->atomic_type) {
            case VOID_A : strncpy(atomic_type, "unit", 5); break;
            case BOOL_A : strncpy(atomic_type, "bool", 5); break;
            case INT_A  : strncpy(atomic_type, "int ", 5); break;
            default     : strncpy(atomic_type, "??? ", 5); break;
        }
        if (symbol->type.param.ref) {
            printf("| parameter | ref %s    | %s", atomic_type, symbol->ident);
        }
        else {
            printf("| parameter | %s        | %s", atomic_type, symbol->ident);
        }
        if (symbol->type.param.typename->symbol_type == ARRAY_TYPE) {
            printf(" ( rangelist[");
            for (int j = 0; j < symbol->type.param.typename->len_range_list; 
                 j++) {
               printf("%i..%i,", symbol->type.param.typename->range_array[j][0],
                   symbol->type.param.typename->range_array[j][1]);
            }
            printf("] )");
        }
        break;
    default: break;
    }
        printf("\r| %i\n", index);
}

void display_symbol_table() {
    switch (symbol_table.last_ident_index) {
        case 0: 
            printf("\nTABLE OF SYMBOLS : EMPTY\n\n"); return;
        case 1:
            printf("\nTABLE OF SYMBOLS : (1 symbol in table)\n\n"); break;
        default :
            printf("\nTABLE OF SYMBOLS : (%i symbols in table)\n\n", 
                symbol_table.last_ident_index);
    }
    int n = floor(log10(symbol_table.last_ident_index)) + 1;
    (n + 3 < 8) ? (n = 8) : (n += 3); // for column alignment
    for (int j = 0; j < n; j++) {
        printf(" ");
    }
    printf("| type      | atomic_type | identifier | rangelist/parameters"
           "\r| index\n");
    for (int i = 0; i < symbol_table.last_ident_index; i ++) {
        display_symbol(&symbol_table.symbols[i], i, n);
    }
}

struct vardecl_t * create_vardecl(struct linked_list *_identlist,
                                  struct typename_t *_typename) {
    if (_typename->atomic_type == VOID_A) {
        handle_error("variable can't be of type unit");
    }
    struct vardecl_t *x = malloc(sizeof(struct vardecl_t));
    MCHECK(x);
    x->identlist = _identlist;
    x->typename = _typename;
    return x;
}

struct typename_t * create_typename_array(struct linked_list *_range_list,
                                          int _atomic_type) {
    struct typename_t *x = malloc(sizeof(struct typename_t));
    MCHECK(x);
    x->symbol_type = ARRAY_TYPE;
    x->range_list = _range_list;
    x->atomic_type = _atomic_type;
    return x;
}

struct typename_t * create_typename_atomic(int _atomic_type) {
    struct typename_t *x = malloc(sizeof(struct typename_t));
    MCHECK(x);
    x->symbol_type = ATOMIC_TYPE;
    x->range_list = NULL;
    x->atomic_type = _atomic_type;
    return x;
}

int is_symbol_in_table(char *identname, int scope) {
    int size = strlen(identname) + 1;
    for (int i = 0; i < symbol_table.last_ident_index; i++) {
        if ((scope == symbol_table.symbols[i].scope ||
            (scope != 0 && symbol_table.symbols[i].scope == 0 && 
            symbol_table.symbols[i].var_func_par == VAR_T)) &&
            size == symbol_table.symbols[i].ident_length &&
            !strncmp(identname, symbol_table.symbols[i].ident, size)) {
                return i;
        }
    }
    return -1;
}

void realloc_table () {
    if (symbol_table.table_size <= symbol_table.last_ident_index + 1) {
        symbol_table.table_size += INIT_TABLE_SIZE;
        symbol_table.symbols = realloc(symbol_table.symbols,
            symbol_table.table_size * sizeof(struct symbol_t));
        MCHECK(symbol_table.symbols);
    }
}

void copy_typename_table(struct typename_t *dest, struct typename_t origin) {
    dest->atomic_type = 0;
    dest->atomic_type = origin.atomic_type;
    dest->symbol_type = origin.symbol_type;
    // if new variable is an array then, the rangelist is stored as an array
    // of type (* array) [2]
    dest->range_list = NULL;
    if (origin.symbol_type == ARRAY_TYPE) {
        int len = list_len(origin.range_list) / 2;
        dest->len_range_list = len;
        dest->range_array = malloc(len * sizeof(int[2]));
        MCHECK(dest->range_array);
        int i = 0;
        struct node *temp_node =  origin.range_list->first;
        while (temp_node != NULL) {
            dest->range_array[i][0] = *(int*)temp_node->data;
            temp_node = temp_node->next;
            dest->range_array[i][1] = *(int*)temp_node->data;
            temp_node = temp_node->next;
            i++;
        }
    }
    // can't be a function or a parameter
    else {
        dest->len_range_list = 0;
        dest->range_array = NULL;
    }
}

void add_vardecl_table (struct linked_list *identlist,
                        struct typename_t *_typename) {
    while (list_len(identlist) != 0) {
        char *new_ident_name = (char *)list_get_first(identlist);
        if (is_symbol_in_table(new_ident_name, symbol_table.cur_symbol_scope) 
            != -1) {
            handle_error("identifier [%s] already declared.", new_ident_name);
        }
        realloc_table();
        struct symbol_t *new_symbol = 
            &symbol_table.symbols[symbol_table.last_ident_index];
        new_symbol->scope = symbol_table.cur_symbol_scope;
        new_symbol->var_func_par = VAR_T;
        new_symbol->ident_length = strlen(new_ident_name) + 1;
        new_symbol->ident = malloc(new_symbol->ident_length);
        MCHECK(new_symbol->ident);
        strncpy(new_symbol->ident, new_ident_name, new_symbol->ident_length);
        new_symbol->type.var.typename = malloc(sizeof(struct typename_t));
        MCHECK(new_symbol->type.var.typename);
        copy_typename_table(new_symbol->type.var.typename, *_typename);
        symbol_table.last_ident_index ++;
        list_pop(identlist);
    }
    list_free(identlist);
    if (_typename->symbol_type == ARRAY_TYPE) {
        list_free(_typename->range_list);
    }
    free(_typename);
}

void add_vardecllist_table(struct linked_list *vardecllist) {
    while (list_len(vardecllist) != 0) {
        struct vardecl_t *vardecl = 
            (struct vardecl_t *)list_get_first(vardecllist);
        add_vardecl_table(vardecl->identlist, vardecl->typename);
        list_pop(vardecllist);
    }
    list_free(vardecllist);
}

void add_paramlist_table(struct linked_list *parlist) {
    while (list_len(parlist) != 0) {
        struct param_t *param = (struct param_t *)list_get_first(parlist);
        if (is_symbol_in_table(param->ident, symbol_table.cur_symbol_scope)
            != -1) {
            handle_error("identifier [%s] already declared.", param->ident);
        }
        realloc_table();
        struct symbol_t *new_symbol = 
            &symbol_table.symbols[symbol_table.last_ident_index];
        new_symbol->var_func_par = PARAM_T;
        new_symbol->ident_length = strlen(param->ident)+1;
        new_symbol->ident = malloc(new_symbol->ident_length);
        MCHECK(new_symbol->ident);
        strncpy(new_symbol->ident, param->ident, new_symbol->ident_length);
        free(param->ident);
        new_symbol->scope = symbol_table.cur_symbol_scope;
        new_symbol->type.param.ident = NULL;
        new_symbol->type.param.ref = param->ref;
        new_symbol->type.param.typename = malloc(sizeof(struct typename_t));
        MCHECK(new_symbol->type.param.typename);
        copy_typename_table(new_symbol->type.param.typename, *param->typename);
        symbol_table.last_ident_index ++;
        if (param->typename->symbol_type == ARRAY_TYPE) {
            list_free(param->typename->range_list);
        }
        free(param->typename);
        list_pop(parlist);
    }
    if(parlist != NULL) {
        list_free(parlist);
    }
}

void add_func_ident_table(char *ident, 
                          int atomictype, 
                          struct linked_list *parlist) {
    if (is_symbol_in_table(ident, 0) != -1) {
        handle_error("identifier [%s] already declared.", ident);
    }
    realloc_table();
    struct symbol_t *new_symbol = 
            &symbol_table.symbols[symbol_table.last_ident_index];
    
    new_symbol->var_func_par = FUNC_T;
    new_symbol->ident_length = strlen(ident) + 1;
    new_symbol->ident = ident;
    new_symbol->scope = 0;
    new_symbol->type.func.atomic_type = atomictype;
    new_symbol->type.func.quad_start = quad_table.nextquad;
    new_symbol->type.func.nb_param = list_len(parlist);
    if (list_len(parlist) != 0) {
        new_symbol->type.func.index_param = 
            malloc(list_len(parlist) * sizeof(int));
        MCHECK(new_symbol->type.func.index_param);
    }
    else {
        new_symbol->type.func.index_param = NULL;
    }
    for (int i = 0; i < new_symbol->type.func.nb_param; i ++) {
        new_symbol->type.func.index_param[i] = 
            symbol_table.last_ident_index + 1 + i;
    }
    int index_function = symbol_table.last_ident_index;
    symbol_table.last_ident_index ++;
    symbol_table.cur_symbol_scope = index_function;
}