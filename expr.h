#ifndef EXPR_H
#define EXPR_H

#include "scalpa.h"
#include <string.h>
#include <math.h>



/**
 * @brief write the string associated the operator code : op in str_op
 * @param op operator code
 * @param str_op result string
 */
void get_operator_str (int op, char (*str_op)[5]);


/**
 * @brief Compute and return the result of : expr1 opb expr2 
 * exit if expr1 or expr2 have a type that doesn't support opb
 * example : string + string, true < false, int xor int
 * exit if expr1 and expr2 have different type
 * @param expr1 struct representing operand 1
 * @param expr2 struct representing operand 2
 * @param opb operator code
 * @param result_type constant type of the result
 * @return a struct containing the result of the operation
 */
struct expr_t compute_opb_const_expr (struct expr_t expr1,
                                      struct expr_t expr2,
                                      int opb,
                                      int result_type);


/**
 * @brief Compute and return the result of : opu expr
 * @param expr struct containing the operand's attributes
 * @return a struct containing the result of the operation
 */
struct expr_t compute_opu_const_expr (struct expr_t expr);


/**
 * @brief Copies the attributes of the expression pointed by 
 * origin in the struct pointed by dest. The function does not
 * test if dest is NULL or not, dest must be allocated.
 * @param dest struct containing the copy
 * @param origin expression to be copied 
 */
void copy_expr_t (struct expr_t *dest, struct expr_t *origin);


/**
 * @brief Creates all necessary quadruplets for computing the 
 * index when accessing a value, in an array.
 * @param range_array 2D array representing max/min boundary in a
 * rangelist
 * @param len_range_list length of range_array
 * @param exprlist list containing the indexes of the value to 
 * be access
 * @return number of the temporary variable containing the index
 */
int get_index_array (int (*range_array)[2],
                     int len_range_list,
                     struct linked_list *exprlist);


/**
 * @brief check if parameters used in a function call, matchs the parameters of 
 * this function in the table of symbol, exit with an apropriate message if it 
 * doesn't matchs
 * create "param" quadruplets for each parameter given to the function
 * @param func_symbol symbol of called function
 * @param param_list list of call's parameters
 * @param func_name function name
 * @return void
 */
void check_function_parameters(struct symbol_t func_symbol,
                               struct linked_list *param_list,
                               char *func_name);
#endif
