#include <stdio.h>
#include <stdlib.h>
#include "quad.h"
#include "y.tab.h"

extern struct quad_table_t quad_table;
extern struct symbol_table_t symbol_table;

void init_quad_table() {
    quad_table.table_size = INIT_TABLE_SIZE;
    quad_table.nextquad = 0;
    quad_table.quads = malloc(INIT_TABLE_SIZE * sizeof(struct quad_t));
    MCHECK(quad_table.quads);
}

void free_quad_table();

void display_instruction(int instr) {
    switch (instr) {
        case GOTO_QUAD:       printf("goto");    break;
        case AFF_QUAD:        printf(":=");      break;
        case IF_LT_QUAD:      printf("<");       break;
        case IF_GT_QUAD:      printf(">");       break;
        case IF_LT_EQ_QUAD:   printf("<=");      break;
        case IF_GT_EQ_QUAD:   printf(">=");      break;
        case IF_EQ_QUAD:      printf("==");      break;
        case IF_DIFF_QUAD:    printf("!=");      break;
        case OPB_PLUS_QUAD:   printf("+");       break;
        case OPB_MINUS_QUAD:  printf("-");       break;
        case OPB_STAR_QUAD:   printf("*");       break;
        case OPB_DIVIDE_QUAD: printf("/");       break;
        case OPB_POW_QUAD:    printf("^");       break;
        case OPU_MINUS_QUAD:  printf("-");       break;
    }
}

void display_quad_op(struct expr_t quad_op) {
    switch (quad_op.quad_op_type) {
        case QO_CST  :
            switch (quad_op.type) {
                case INT:    printf(" %i ", quad_op.const_int); break;
                case BOOL:   printf(" %i ", quad_op.const_bool); break;
                case STRING: printf(" %s ", quad_op.const_string); break;
            } 
            break;
        case QO_VAR  :
            printf(" %s", symbol_table.symbols[quad_op.var.ptr].ident);
            if (quad_op.var.symbol_type == ARRAY_TYPE && !quad_op.is_array) {
                printf("[%%T%i]", quad_op.var.ptr_to_index);
            }
            printf(" ");
            break;
        case QO_TEMP :
            printf(" %%T%i ", quad_op.temp.ptr);
            if (quad_op.temp.symbol_type == ARRAY_TYPE && !quad_op.is_array) {
                printf("[%%T%i]", quad_op.temp.ptr_to_index);
            }
            printf(" ");
            break;
    }
}

void display_quad(struct quad_t quad) {
    if (quad.instruction == IF_GT_QUAD || quad.instruction == IF_LT_QUAD ||
        quad.instruction == IF_LT_EQ_QUAD || quad.instruction == IF_GT_EQ_QUAD||
        quad.instruction == IF_EQ_QUAD || quad.instruction == IF_DIFF_QUAD) {
        printf("[ if (");
        display_quad_op(quad.op1);
        display_instruction(quad.instruction);
        display_quad_op(quad.op2);
        printf(") then goto %i ]\n", quad.target);
    }
    else if (quad.instruction == IF_QUAD) {
        printf("[ if (");
        display_quad_op(quad.op1);
        printf(") then goto %i ]\n", quad.target);
    }
    else if (quad.instruction == GOTO_QUAD) {
        printf("[ goto %i ]\n", quad.target);
    }
    else if (quad.instruction == AFF_QUAD) {
        printf("[");
        display_quad_op(quad.res);
        display_instruction(quad.instruction);
        display_quad_op(quad.op1);
        printf("]\n");
    }
    else if(quad.instruction == OPU_MINUS_QUAD) {
        printf("[");
        display_quad_op(quad.res);
        printf(":= ");
        display_instruction(quad.instruction);
        display_quad_op(quad.op1);
        printf("]\n");
    }
    else if (quad.instruction == READ_QUAD) {
        printf("[ read :");
        display_quad_op(quad.op1);
        printf("]\n");
    }
    else if (quad.instruction == WRITE_QUAD) {
        printf("[ write :");
        display_quad_op(quad.op1);
        printf("]\n");
    }
    else if (quad.instruction == PARAM_QUAD) {
        printf("[ param");
        display_quad_op(quad.op1);
        printf("]\n");
    }
    else if (quad.instruction == CALL_AFF_QUAD) {
        printf("[");
        display_quad_op(quad.op1);
        printf(":= call");
        display_quad_op(quad.op2);
        printf(",");
        display_quad_op(quad.res);
        printf("]\n");
    }
    else if (quad.instruction == CALL_QUAD) {
        printf("[ call");
        display_quad_op(quad.op1);
        printf(",");
        display_quad_op(quad.res);
        printf("]\n");
    }
    else if (quad.instruction == RETURN_QUAD) {
        printf("[ return");
        display_quad_op(quad.res);
        printf("]\n");
    }
    else if (quad.instruction == RETURN_UNIT_QUAD) {
        printf("[ return ]\n");
    }
    else if (quad.instruction == EXIT_QUAD) {
        printf("[ exit ]\n");
    }
    else{
        printf("[");
        display_quad_op(quad.res);
        printf(":=");
        display_quad_op(quad.op1);
        display_instruction(quad.instruction);
        display_quad_op(quad.op2);
        printf("]\n");
    }
}

void display_quad_table() {
    printf("\nTABLE OF QUADS : (number of  quad = %i)\n", quad_table.nextquad);
    for (int i = 0; i < quad_table.nextquad; i++) {
        printf("%i : ", i);
        display_quad(quad_table.quads[i]);
    }
}

struct quad_list_t* create_quad_list(int position) {
    struct quad_list_t* list = malloc(sizeof(struct quad_list_t));
    MCHECK(list);
    list->position = position;
    list->next = NULL;
    return list;
}

struct quad_list_t* concat_quad_list(struct quad_list_t* list_1, 
                                     struct quad_list_t* list_2) {
    struct quad_list_t* list_concat;
    if (list_1 != NULL) {
        list_concat = list_1;
    }
    else {
        if (list_2 != NULL) {
            list_concat = list_2;
        }
        else {
            list_concat = NULL;
        }
    }
    if (list_1 != NULL) {
        while (list_1->next!=NULL) {
            list_1 = list_1->next;
        }
        list_1->next = list_2;
    }
    return list_concat;
}

void complete_quad_list(struct quad_list_t* quad_list, int target) {
    if (quad_list == NULL) {
        return;
    }
    quad_table.quads[quad_list->position].target = target;
    while (quad_list->next != NULL) {
        quad_list = quad_list->next;
        quad_table.quads[quad_list->position].target = target;
    }
}

void free_quad_list(struct quad_list_t* quad_list) {
    if (quad_list == NULL) {
        return;
    }
    while (quad_list->next != NULL) {
        struct quad_list_t* first = quad_list->next;
        quad_list->next = first->next;
        free(first);
    }
    free(quad_list);
}

void free_quad_op(struct expr_t quad_op) {
    if (quad_op.type == STRING) {
        free(quad_op.const_string);
    }
}

void free_quad(struct quad_t quad) {
    if (quad.instruction == IF_GT_QUAD || quad.instruction == IF_LT_QUAD ||
        quad.instruction == IF_LT_EQ_QUAD || quad.instruction == IF_GT_EQ_QUAD||
        quad.instruction == IF_EQ_QUAD || quad.instruction == IF_DIFF_QUAD) {
        free_quad_op(quad.op1);
        free_quad_op(quad.op2);
    }
    else if (quad.instruction == IF_QUAD) {
        free_quad_op(quad.op1);
    }
    else if (quad.instruction == GOTO_QUAD) {
        return;
    }
    else if (quad.instruction == AFF_QUAD) {
        free_quad_op(quad.res);
        free_quad_op(quad.op1);
    }
    else if(quad.instruction == OPU_MINUS_QUAD) {
        free_quad_op(quad.res);
        free_quad_op(quad.op1);
    }
    else if (quad.instruction == READ_QUAD) {
        free_quad_op(quad.op1);
    }
    else if (quad.instruction == WRITE_QUAD) {
        free_quad_op(quad.op1);
    }
    else if (quad.instruction == PARAM_QUAD) {
        free_quad_op(quad.op1);
    }
    else if (quad.instruction == CALL_AFF_QUAD) {
        free_quad_op(quad.op1);
        free_quad_op(quad.op2);
        free_quad_op(quad.res);
    }
    else if (quad.instruction == CALL_QUAD) {
        free_quad_op(quad.op1);
        free_quad_op(quad.res);
    }
    else if (quad.instruction == RETURN_QUAD) {
        free_quad_op(quad.res);
    }
    else if (quad.instruction == RETURN_UNIT_QUAD) {
        return;
    }
    else if (quad.instruction == EXIT_QUAD) {
        return;
    }
    else {
        free_quad_op(quad.res);
        free_quad_op(quad.op1);
        free_quad_op(quad.op2);
    }
}

void free_quad_table() {
    for (int i = 0; i < quad_table.nextquad; i++) {
        free_quad(quad_table.quads[i]);
    }
    free(quad_table.quads);
}

int get_instr(int op, int is_unary) {
    switch (op) {
        case OPB_PLUS:   return OPB_PLUS_QUAD;   break;
        case OPB_STAR:   return OPB_STAR_QUAD;   break;
        case OPB_DIVIDE: return OPB_DIVIDE_QUAD; break;
        case OPB_POW:    return OPB_POW_QUAD;    break;
        case OPB_L:      return IF_LT_QUAD;      break;
        case OPB_L_EQ:   return IF_LT_EQ_QUAD;   break;
        case OPB_G:      return IF_GT_QUAD;      break;
        case OPB_G_EQ:   return IF_GT_EQ_QUAD;   break;
        case OPB_EQ:     return IF_EQ_QUAD;      break;
        case OPB_DIFF:   return IF_DIFF_QUAD;    break;
        case OP_MINUS:   return is_unary ? OPU_MINUS_QUAD : OPB_MINUS_QUAD;
        break;
    }
    return -1;
}

void gencode (int instruction, 
              struct expr_t op1, 
              struct expr_t op2,
              struct expr_t res) {
    if (quad_table.table_size <= quad_table.nextquad + 1) {
        quad_table.table_size += INIT_TABLE_SIZE;
        quad_table.quads = realloc(quad_table.quads,
            quad_table.table_size * sizeof(struct quad_t));
        MCHECK(quad_table.quads);
    }
    quad_table.quads[quad_table.nextquad].is_label = 0;
    quad_table.quads[quad_table.nextquad].res_type = 
        (instruction == GOTO_QUAD || instruction == IF_GT_QUAD || 
         instruction == IF_LT_QUAD || instruction == IF_LT_EQ_QUAD ||
         instruction == IF_GT_EQ_QUAD || instruction == IF_EQ_QUAD ||
         instruction == IF_DIFF_QUAD || instruction ==IF_QUAD);
    quad_table.quads[quad_table.nextquad].instruction = instruction;
    quad_table.quads[quad_table.nextquad].op1 = op1;
    quad_table.quads[quad_table.nextquad].op2 = op2;
    if (quad_table.quads[quad_table.nextquad].res_type) {
        quad_table.quads[quad_table.nextquad].target = GOTO_INCOMPLETE;
    }
    else {
        quad_table.quads[quad_table.nextquad].res = res;
    }
    quad_table.nextquad ++;
}

void gencode_goto (int target) {
    if (quad_table.table_size <= quad_table.nextquad + 1) {
        quad_table.table_size += INIT_TABLE_SIZE;
        quad_table.quads = realloc(quad_table.quads,
            quad_table.table_size * sizeof(struct quad_t));
        MCHECK(quad_table.quads);
    }
    quad_table.quads[quad_table.nextquad].is_label = 0;
    quad_table.quads[quad_table.nextquad].instruction = GOTO_QUAD;
    quad_table.quads[quad_table.nextquad].res_type = 1;
    quad_table.quads[quad_table.nextquad].target = target;
    quad_table.nextquad ++;
}

void add_exit () {
    quad_table.quads[quad_table.nextquad].instruction = EXIT_QUAD;
    quad_table.quads[quad_table.nextquad].res_type = 0;
    quad_table.quads[quad_table.nextquad].is_label = 0;
    quad_table.nextquad ++;
}

void complete_labels() {
    for (int i = 0; i < quad_table.nextquad; i++) {
        if (quad_table.quads[i].res_type) {
            if (quad_table.quads[i].target == -1) {
                handle_error("imcoplete goto at quad %i", i); // can't happen
            }
            else if (quad_table.quads[i].target >= quad_table.nextquad) {
                handle_error("goto out of range at quad %i", i); // can't happen
            }
            else {
                quad_table.quads[quad_table.quads[i].target].is_label = 1;
            }
        }
    }
}

int newtemp(int mode) {
    static int ptr = -1;
    ptr = (mode ? -1 : ptr + 1);
    if (ptr >= 8) {
        handle_error(
            "\nExpression is too complicated, more than seven temporary "
            "variable are needed.\nPlease try to split the expression or "
            "rewrite it.\n(no algorithm to reduce the number of temp are"
            "implemented yet)");
    }
    return ptr;
}