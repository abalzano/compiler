#include "expr.h"
#include "y.tab.h"
#include <stdio.h>

extern struct symbol_table_t symbol_table;
extern struct quad_table_t quad_table;

void get_operator_str (int op, char (*str_op)[5]) {
    switch(op) {
        case OPB_PLUS   : strncpy(*str_op, "+", 2);    break;
        case OP_MINUS   : strncpy(*str_op, "-", 2);    break;
        case OPB_STAR   : strncpy(*str_op, "*", 2);    break;
        case OPB_DIVIDE : strncpy(*str_op, "/", 2);    break;
        case OPB_POW    : strncpy(*str_op, "^", 2);    break;
        case OPB_L      : strncpy(*str_op, "<", 2);    break;
        case OPB_L_EQ   : strncpy(*str_op, "<=", 3);   break;
        case OPB_G      : strncpy(*str_op, ">", 2);    break;
        case OPB_G_EQ   : strncpy(*str_op, ">=", 3);   break;
        case OPB_EQ     : strncpy(*str_op, "=", 3);    break;
        case OPB_DIFF   : strncpy(*str_op, "<>", 3);   break;
        case OPB_AND    : strncpy(*str_op, "and", 4);  break;
        case OPB_OR     : strncpy(*str_op, "or", 3);   break;
        case OPB_XOR    : strncpy(*str_op, "xor", 4);  break;
        case OPU_NOT    : strncpy(*str_op, "not", 4);  break;
        default         : strncpy(*str_op, "null", 5); break; 
    }
}

struct expr_t compute_opb_const_expr (struct expr_t expr1, 
                                      struct expr_t expr2, 
                                      int opb,
                                      int result_type) {
    struct expr_t result;
    result.type = result_type;
    result.is_array = 0;
    result.quad_op_type = QO_CST;
    if (expr1.type == INT) {
        switch(opb) {
        case OPB_PLUS   :
            result.const_int = expr1.const_int + expr2.const_int;      break;
        case OP_MINUS   :
            result.const_int = expr1.const_int - expr2.const_int;      break;
        case OPB_STAR   :
            result.const_int = expr1.const_int * expr2.const_int;      break;
        case OPB_DIVIDE :
            result.const_int = expr1.const_int / expr2.const_int;      break;
        case OPB_POW    :
            result.const_int = pow(expr1.const_int, expr2.const_int);  break;
        case OPB_L      :
            result.const_bool = (expr1.const_int < expr2.const_int);   break;
        case OPB_L_EQ   :
            result.const_bool = (expr1.const_int <= expr2.const_int);  break;
        case OPB_G      :
            result.const_bool = (expr1.const_int > expr2.const_int);   break;
        case OPB_G_EQ   :
            result.const_bool = (expr1.const_int >= expr2.const_int);  break;
        case OPB_EQ     :
            result.const_bool = (expr1.const_int == expr2.const_int);  break;
        case OPB_DIFF   :
            result.const_bool = (expr1.const_int != expr2.const_int);  break;
        default : break;
        }
    }
    else {
        switch(opb) {
        case OPB_EQ     :
            result.const_bool = (expr1.const_bool == expr2.const_bool); break;
        case OPB_DIFF   :
            result.const_bool = (expr1.const_bool != expr2.const_bool); break;
        case OPB_AND    :
            result.const_bool = (expr1.const_bool && expr2.const_bool); break;
        case OPB_OR     :
            result.const_bool = (expr1.const_bool || expr2.const_bool); break;
        case OPB_XOR    :
            result.const_bool = (expr1.const_bool ^ expr2.const_bool);  break;
        default : break;
        }
    }
    return result;
}

struct expr_t compute_opu_const_expr (struct expr_t expr) {
    struct expr_t result;
    result.quad_op_type = QO_CST;
    result.is_array = 0;
    if (expr.type == INT) {
        result.type = INT;
        result.const_int = - expr.const_int;
    }
    else {
        result.type = BOOL;
        result.const_bool = ! expr.const_bool;
    }
    return result;
}

void copy_expr_t (struct expr_t *dest, struct expr_t *origin) {
    dest->quad_op_type = origin->quad_op_type;
    dest->type = origin->type;
    switch (dest->quad_op_type) {
    case QO_CST  :
        switch(dest->type) {
        case INT    :
            dest->const_int = origin->const_int; 
            break;
        case BOOL   :
            dest->const_string = origin->const_string; 
            break;
        case STRING :
            dest->const_bool = origin->const_bool;
            break;
        }
        break;
    case QO_VAR  :
        dest->var.ptr = origin->var.ptr;
        dest->var.symbol_type = origin->var.symbol_type;
        dest->var.ptr_to_index = origin->var.ptr_to_index;
        break;
    case QO_TEMP :
        dest->temp.ptr = origin->temp.ptr;
        dest->temp.symbol_type = origin->temp.symbol_type;
        dest->temp.ptr_to_index = origin->temp.ptr_to_index;
        break;
    }
}

int get_index_array (int (*range_array)[2],
                     int len_range_list,
                     struct linked_list *exprlist) {
    if (len_range_list != list_len(exprlist)) {
        handle_error("exprlist and rangelist have different size.");
    }
    struct expr_t *expr_temp = (struct expr_t *)list_get_first(exprlist);
    if (expr_temp->type != INT) {
        handle_error("bool or string can't serve as element indexation"
            "in an array, only integers allowed.");
    }
    // add offset for dim not starting at 0
    if (range_array[0][0] != 0) {
        if (expr_temp->quad_op_type == QO_CST) {
            expr_temp->const_int = expr_temp->const_int - range_array[0][0];
        }
        else {
            struct expr_t offset;
            offset.quad_op_type = QO_CST;
            offset.type = INT;
            offset.const_int = - range_array[0][0];
            gencode (OPB_PLUS_QUAD, *expr_temp, offset, *expr_temp);
        }
    }
    struct expr_t old_expr, i_expr, n_expr, res_expr, res_expr2;
    copy_expr_t(&old_expr, expr_temp);
    int temp_ptr = newtemp(0);
    list_pop(exprlist);
    for (int i = 1; i < len_range_list; i++) {
        struct expr_t *expr_temp = (struct expr_t *)list_get_first(exprlist);
        if (expr_temp->type != INT) {
            handle_error("bool or string can't serve as element indexation "
                "in an array, only integers allowed.");
        }
        // add offset for dim not starting at 0
        if (range_array[i][0] != 0) {
            if (expr_temp->quad_op_type == QO_CST) {
                expr_temp->const_int = expr_temp->const_int - range_array[i][0];
            }
            else {
                struct expr_t offset;
                offset.quad_op_type = QO_CST;
                offset.type = INT;
                offset.const_int = - range_array[i][0];
                gencode (OPB_PLUS_QUAD, *expr_temp, offset, *expr_temp);
            }
        }
        copy_expr_t(&i_expr, expr_temp);

        n_expr.quad_op_type = QO_CST;
        n_expr.type = INT;
        n_expr.const_int = range_array[i][1] - range_array[i][0] + 1;

        res_expr.quad_op_type = QO_TEMP;
        res_expr.type = INT;
        res_expr.temp.ptr = temp_ptr;
        res_expr.temp.symbol_type = ATOMIC_TYPE;
        res_expr.temp.ptr_to_index = 0;

        res_expr2.quad_op_type = QO_TEMP;
        res_expr2.type = INT;
        res_expr2.temp.ptr = temp_ptr;
        res_expr2.temp.symbol_type = ATOMIC_TYPE;
        res_expr2.temp.ptr_to_index = 0;

        gencode (OPB_STAR_QUAD, n_expr, old_expr, res_expr);
        gencode (OPB_PLUS_QUAD, res_expr, i_expr, res_expr2);
        copy_expr_t(&old_expr, &res_expr2);

        list_pop(exprlist);
    }
    if (len_range_list == 1) {
        res_expr.quad_op_type = QO_TEMP;
        res_expr.type = INT;
        res_expr.temp.ptr = temp_ptr;
        res_expr.temp.symbol_type = ATOMIC_TYPE;
        res_expr.temp.ptr_to_index = 0;
        gencode (AFF_QUAD, old_expr, old_expr, res_expr);
    }
    struct expr_t nbw;
    nbw.quad_op_type = QO_CST;
    nbw.type = INT;
    nbw.const_int = 4;
    gencode (OPB_STAR_QUAD, res_expr, nbw, res_expr);
    return temp_ptr;
}

void check_function_parameters(struct symbol_t func_symbol,
                               struct linked_list *param_list,
                               char *func_name) {
    int ptr = is_symbol_in_table(func_name, 0);
    int nb_parameters = list_len(param_list);
    if (func_symbol.type.func.nb_param != nb_parameters) {
        handle_error("arguments given in call of function [%s] are "
            "different from [%s] type", func_name, func_name);
    }
    for (int i = 0; i < nb_parameters; i++) {
        struct expr_t *expr_temp = (struct expr_t *)list_get_first(param_list);
        struct param_t cur_param = 
            symbol_table.symbols[ptr + 1 + i].type.param;
        if ((int)cur_param.typename->atomic_type != (int)expr_temp->type) {
            handle_error("arguments given in call of function [%s] are "
                "different from [%s] type, for parameter [%i]", 
                func_name, func_name,i+1);
        }
        if (cur_param.typename->symbol_type == ARRAY_TYPE) {
            if (cur_param.typename->symbol_type == ARRAY_TYPE && 
                !expr_temp->is_array) {
                handle_error("arguments given in call of function [%s] are "
                    "different from [%s] type, for parameter [%i]", 
                    func_name, func_name,i+1);
            }
            struct typename_t *arraytype_2;
            switch (symbol_table.symbols[expr_temp->index_symbol_table]
                    .var_func_par) {
            case VAR_T:
                arraytype_2 = 
                    symbol_table.symbols[expr_temp->index_symbol_table]
                    .type.var.typename;
                break;
            case PARAM_T:
                arraytype_2 = 
                    symbol_table.symbols[expr_temp->index_symbol_table]
                    .type.param.typename;
                break;
            default:
                handle_error("arguments given in call of function [%s] are "
                "different from [%s] type, for parameter [%i]", 
                func_name, func_name,i+1);
                break;
            }
            if (cur_param.typename->len_range_list != 
                arraytype_2->len_range_list) {
                handle_error("arguments given in call of function [%s] are "
                "different from [%s] type, for parameter [%i]", 
                func_name, func_name,i+1);
            }
            for (int i = 0; i < arraytype_2->len_range_list; i++) {
                if ((arraytype_2->range_array[i][0] != 
                    cur_param.typename->range_array[i][0]) ||
                    (arraytype_2->range_array[i][1] != 
                    cur_param.typename->range_array[i][1])) {
                    handle_error("arguments given in call of function [%s]"
                        "are different from [%s] type, for parameter [%i]", 
                        func_name, func_name,i+1);
                }
            }
        }
        if (expr_temp->type == BOOL) {
            complete_quad_list(expr_temp->true, quad_table.nextquad);
            free_quad_list(expr_temp->true);
            complete_quad_list(expr_temp->false, quad_table.nextquad);
            free_quad_list(expr_temp->false);
        }
        gencode(PARAM_QUAD, *expr_temp, *expr_temp, *expr_temp);
        list_pop(param_list);
    }
    list_free(param_list);
}