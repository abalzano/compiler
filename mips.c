#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "scalpa.h"
#include "linked_list.h"
#include "table_of_symbol.h"
#include "args-parser.h"
#include "quad.h"
#include "expr.h"
#include "mips.h"


extern struct quad_table_t quad_table;
extern struct symbol_table_t symbol_table;
extern struct string_table_t string_table;


extern int fd_out;

/* -------------------------------------------------------------------------- */
/*                             write spim file                                */
/* -------------------------------------------------------------------------- */

void write_instr(int i) {
        struct quad_t quad;
        quad = quad_table.quads[i];

        if (quad_table.quads[i].is_label) {
            dprintf(fd_out, "__label_%i:\n", i);
        }
        if (DEBUG) {
            switch(quad.instruction) {
            case GOTO_QUAD :
                dprintf(fd_out, "#%i,\tGOTO_QUAD\n", i);        break;
            case AFF_QUAD :
                dprintf(fd_out, "#%i,\tAFF_QUAD\n", i);         break;
            case IF_QUAD :
                dprintf(fd_out, "#%i,\tIF_QUAD\n", i);          break;
            case IF_LT_QUAD :
                dprintf(fd_out, "#%i,\tIF_LT_QUAD\n", i);       break;
            case IF_GT_QUAD :
                dprintf(fd_out, "#%i,\tIF_GT_QUAD\n", i);       break;
            case IF_LT_EQ_QUAD :
                dprintf(fd_out, "#%i,\tIF_LT_EQ_QUAD\n", i);    break;
            case IF_GT_EQ_QUAD :
                dprintf(fd_out, "#%i,\tIF_GT_EQ_QUAD\n", i);    break;
            case IF_EQ_QUAD :
                dprintf(fd_out, "#%i,\tIF_EQ_QUAD\n", i);       break;
            case IF_DIFF_QUAD :
                dprintf(fd_out, "#%i,\tIF_DIFF_QUAD\n", i);     break;
            case OPB_PLUS_QUAD:
                dprintf(fd_out, "#%i,\tOPB_PLUS_QUAD\n", i);    break;
            case OPB_MINUS_QUAD:
                dprintf(fd_out, "#%i,\tOPB_MINUS_QUAD\n", i);   break;
            case OPB_STAR_QUAD:
                dprintf(fd_out, "#%i,\tOPB_STAR_QUAD\n", i);    break;
            case OPB_DIVIDE_QUAD:
                dprintf(fd_out, "#%i,\tOPB_DIVIDE_QUAD\n", i);  break;
            case OPB_POW_QUAD:
                dprintf(fd_out, "#%i,\tOPB_POW_QUAD\n", i);     break;
            case OPU_MINUS_QUAD:
                dprintf(fd_out, "#%i,\tOPU_MINUS_QUAD\n", i);   break;
            case READ_QUAD:
                dprintf(fd_out, "#%i,\tREAD_QUAD\n", i);        break;
            case WRITE_QUAD:
                dprintf(fd_out, "#%i,\tWRITE_QUAD\n", i);       break;
            case CALL_QUAD:
                dprintf(fd_out, "#%i,\tCALL_QUAD\n", i);        break;
            case CALL_AFF_QUAD:
                dprintf(fd_out, "#%i,\tCALL_AFF_QUAD\n", i);    break;
            case PARAM_QUAD:
                dprintf(fd_out, "#%i,\tPARAM_QUAD\n", i);       break;
            case RETURN_UNIT_QUAD: 
                dprintf(fd_out, "#%i,\tRETURN_UNIT_QUAD\n", i); break;
            case RETURN_QUAD: 
                dprintf(fd_out, "#%i,\tRETURN_QUAD\n", i);      break;
            case EXIT_QUAD: 
                dprintf(fd_out, "#%i,\tEXIT_QUAD\n", i);        break;
            }
        }
        switch(quad.instruction) {
        case GOTO_QUAD :
            dprintf(fd_out, "\tj __label_%i\n",quad_table.quads[i].target);
            break;
        
        case AFF_QUAD :
            if (quad.op1.quad_op_type == QO_VAR && 
                quad.op1.is_array) {
                dprintf(fd_out, "\tla $t%i, %s_%i\n",
                    quad.res.temp.ptr,
                    symbol_table.symbols[quad.op1.var.ptr].ident,
                    symbol_table.symbols[quad.op1.var.ptr].scope);
                return;
            }
            switch(quad.res.quad_op_type) {
            case QO_VAR:
                switch(quad.op1.quad_op_type) {
                case QO_CST:
                    dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                    dprintf(fd_out, "\tsw $t9, %s_%i\n",
                        symbol_table.symbols[quad.res.var.ptr].ident,
                        symbol_table.symbols[quad.res.var.ptr].scope);
                    break;
                    
                case QO_TEMP:
                    dprintf(fd_out, "\tsw $t%i, %s_%i\n",
                        quad.op1.temp.ptr,
                        symbol_table.symbols[quad.res.var.ptr].ident,
                        symbol_table.symbols[quad.res.var.ptr].scope);
                    break;
                default: break;
                }
                break;
            
            case QO_TEMP:
                switch(quad.op1.quad_op_type) {
                case QO_CST:
                    if (quad.res.temp.symbol_type == ARRAY_TYPE && 
                        !quad.res.is_array) {
                        dprintf(fd_out, "\tadd $t%i, $t%i, $t%i\n",
                            quad.res.temp.ptr,
                            quad.res.temp.ptr,
                            quad.res.temp.ptr_to_index);
                        dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                        dprintf(fd_out, "\tsw $t9, ($t%i)\n",quad.res.temp.ptr);
                    }
                    else {
                        dprintf(fd_out, "\tli $t%i, %i\n",
                            quad.res.temp.ptr,
                            quad.op1.const_int);
                    }
                    break;
                
                case QO_VAR:
                    dprintf(fd_out, "\tlw $t%i, %s_%i\n",
                        quad.res.temp.ptr,
                        symbol_table.symbols[quad.op1.var.ptr].ident,
                        symbol_table.symbols[quad.op1.var.ptr].scope);
                    break;
                    
                case QO_TEMP:
                    if (quad.op1.temp.symbol_type == ARRAY_TYPE && 
                        !quad.op1.is_array) {
                        dprintf(fd_out, "\tadd $t%i, $t%i, $t%i\n",
                            quad.op1.temp.ptr,
                            quad.op1.temp.ptr,
                            quad.op1.temp.ptr_to_index);
                        dprintf(fd_out, "\tlw $t%i, ($t%i)\n",
                            quad.res.temp.ptr,
                            quad.op1.temp.ptr);
                    }
                    else if (quad.res.temp.symbol_type == ARRAY_TYPE && 
                            !quad.res.is_array) {
                        dprintf(fd_out, "\tadd $t%i, $t%i, $t%i\n",
                            quad.res.temp.ptr,
                            quad.res.temp.ptr,
                            quad.res.temp.ptr_to_index);
                        dprintf(fd_out, "\tsw $t%i, ($t%i)\n",
                            quad.op1.temp.ptr,
                            quad.res.temp.ptr);
                    }
                    else {
                        dprintf(fd_out, "\tmove $t%i, $t%i\n",
                            quad.res.temp.ptr,
                            quad.op1.temp.ptr);
                    }
                    break;
                }
                break;

            default: break;
            }
            break;
        
        case IF_QUAD :
            if (quad.op1.quad_op_type == QO_CST) {
                dprintf(fd_out, "\tli $t9, 1\n");
                dprintf(fd_out, "\tli $t8, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tbeq $t8, $t9, __label_%i\n", 
                    quad_table.quads[i].target);
            }
            else {
                dprintf(fd_out, "\tli $t9, 1\n");
                dprintf(fd_out, "\tbeq $t%i, $t9, __label_%i\n",
                    quad.op1.temp.ptr,
                    quad_table.quads[i].target);
            }
            break;
        
        case IF_LT_QUAD :
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tblt $t%i, $t9, __label_%i\n",
                    quad.op1.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                     (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tblt $t9, $t%i, __label_%i\n",
                    quad.op2.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else {
                dprintf(fd_out, "\tblt $t%i, $t%i, __label_%i\n",
                    quad.op1.temp.ptr,
                    quad.op2.temp.ptr,
                    quad_table.quads[i].target);
            }
            break;
        
        case IF_GT_QUAD :
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tbgt $t%i, $t9, __label_%i\n",
                    quad.op1.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                     (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tbgt $t9, $t%i, __label_%i\n",
                    quad.op2.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else {
                dprintf(fd_out, "\tbgt $t%i, $t%i, __label_%i\n",
                    quad.op1.temp.ptr,
                    quad.op2.temp.ptr,
                    quad_table.quads[i].target);
            }
            break;
        
        case IF_LT_EQ_QUAD :
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tble $t%i, $t9, __label_%i\n",
                    quad.op1.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                     (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tble $t9, $t%i, __label_%i\n",
                    quad.op2.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else {
                dprintf(fd_out, "\tble $t%i, $t%i, __label_%i\n",
                    quad.op1.temp.ptr,
                    quad.op2.temp.ptr,
                    quad_table.quads[i].target);
            }
            break;
        
        case IF_GT_EQ_QUAD :
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tbge $t%i, $t9, __label_%i\n",
                    quad.op1.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                     (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tbge $t9, $t%i, __label_%i\n",
                    quad.op2.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else {
                dprintf(fd_out, "\tbge $t%i, $t%i, __label_%i\n",
                    quad.op1.temp.ptr,
                    quad.op2.temp.ptr,
                    quad_table.quads[i].target);
            }
            break;
        
        case IF_EQ_QUAD :
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tbeq $t%i, $t9, __label_%i\n",
                    quad.op1.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                     (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tbeq $t9, $t%i, __label_%i\n",
                    quad.op2.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else {
                dprintf(fd_out, "\tbeq $t%i, $t%i, __label_%i\n",
                    quad.op1.temp.ptr,
                    quad.op2.temp.ptr,
                    quad_table.quads[i].target);
            }
            break;
        
        case IF_DIFF_QUAD :
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tbne $t%i, $t9, __label_%i\n",
                    quad.op1.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                     (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tbne $t9, $t%i, __label_%i\n",
                    quad.op2.temp.ptr, 
                    quad_table.quads[i].target);
            }
            else {
                dprintf(fd_out, "\tbne $t%i, $t%i, __label_%i\n",
                    quad.op1.temp.ptr,
                    quad.op2.temp.ptr,
                    quad_table.quads[i].target);
            }
            break;
        
        case OPB_PLUS_QUAD:
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\taddi $t%i, $t%i, %i\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr,
                    quad.op2.const_int);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                    (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\taddi $t%i, $t%i, %i\n",
                    quad.res.temp.ptr,
                    quad.op2.temp.ptr,
                    quad.op1.const_int);
            }
            else {
                dprintf(fd_out, "\tadd $t%i, $t%i, $t%i\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr,
                    quad.op2.const_int);
            }
            break;

        case OPB_MINUS_QUAD:
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tsub $t%i, $t%i, $t9\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                    (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tsub $t%i, $t9, $t%i\n",
                    quad.res.temp.ptr,
                    quad.op2.temp.ptr);
            }
            else {
                dprintf(fd_out, "\tsub $t%i, $t%i, $t%i\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr,
                    quad.op2.const_int);
            }
            break;

        case OPB_STAR_QUAD:
            if ((quad.op1.quad_op_type == QO_CST) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t8, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tmulo $t%i, $t8, $t9\n", quad.res.temp.ptr);
            }
            else if ((quad.op1.quad_op_type == QO_TEMP) && 
                    (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tmulo $t%i, $t%i, $t9\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                    (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tmulo $t%i, $t9, $t%i\n",
                    quad.res.temp.ptr,
                    quad.op2.temp.ptr);
            }
            else {
                dprintf(fd_out, "\tmulo $t%i, $t%i, $t%i\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr,
                    quad.op2.temp.ptr);
            }
            break;

        case OPB_DIVIDE_QUAD:
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op2.const_int);
                dprintf(fd_out, "\tdiv $t%i, $t%i, $t9\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tdiv $t%i, $t9, $t%i\n",
                    quad.res.temp.ptr,
                    quad.op2.temp.ptr);
            }
            else {
                dprintf(fd_out, "\tdiv $t%i, $t%i, $t%i\n",
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr,
                    quad.op2.const_int);
            }
            break;

        case OPB_POW_QUAD:
            if ((quad.op1.quad_op_type == QO_TEMP) && 
                (quad.op2.quad_op_type == QO_CST)) {
                dprintf(fd_out, "\tli $t9, 0\n");
                dprintf(fd_out, "\tli $t%i, 1\n", quad.res.temp.ptr);
                dprintf(fd_out, "__pow_loop_%i: beq $t9, %i, __end_pow_%i\n",
                    i,quad.op2.const_int,i);
                dprintf(fd_out, "\tmulo $t%i, $t%i, $t%i\n",
                    quad.res.temp.ptr,
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr);
                dprintf(fd_out, "\taddi $t9, $t9, 1\n");
                dprintf(fd_out, "\tj __pow_loop_%i\n", i);
                dprintf(fd_out, "__end_pow_%i : li $t9, 0\n", i);
            }
            else if ((quad.op1.quad_op_type == QO_CST) &&
                (quad.op2.quad_op_type == QO_TEMP)) {
                dprintf(fd_out, "\tli $t9, 0\n");
                dprintf(fd_out, "\tli $t%i, 1\n", quad.res.temp.ptr);
                dprintf(fd_out, "__pow_loop_%i: beq $t9, $t%i, __end_pow_%i\n",
                    i, quad.op2.temp.ptr, i);
                dprintf(fd_out, "\tmulo $t%i, $t%i, %i\n",
                    quad.res.temp.ptr,
                    quad.res.temp.ptr,
                    quad.op1.const_int);
                dprintf(fd_out, "\taddi $t9, $t9, 1\n");
                dprintf(fd_out, "\tj __pow_loop_%i\n", i);
                dprintf(fd_out, "__end_pow_%i : li $t9, 0\n", i);
            }
            else {
                dprintf(fd_out, "\tli $t9, 0\n");
                dprintf(fd_out, "\tli $t%i, 1\n", quad.res.temp.ptr);
                dprintf(fd_out, "__pow_loop_%i: beq $t9, $t%i, __end_pow_%i\n",
                    i,quad.op2.temp.ptr,i);
                dprintf(fd_out, "\tmulo $t%i, $t%i, $t%i\n",
                    quad.res.temp.ptr,
                    quad.res.temp.ptr,
                    quad.op1.temp.ptr);
                dprintf(fd_out, "\taddi $t9, $t9, 1\n");
                dprintf(fd_out, "\tj __pow_loop_%i\n", i);
                dprintf(fd_out, "__end_pow_%i : li $t9, 0\n", i);
            }
            break;
        
        case OPU_MINUS_QUAD:
            dprintf(fd_out, "\tneg $t%i, $t%i\n",
                quad.res.temp.ptr,
                quad.op1.temp.ptr);
            break;
        
        case READ_QUAD:
            if (quad.op1.quad_op_type == QO_TEMP) {
                dprintf(fd_out, "\tli $v0, 5\n");
                dprintf(fd_out, "\tsyscall\n");
                dprintf(fd_out, "\tmove $v0, $t%i\n", quad.op1.temp.ptr);
            }
            else {
                dprintf(fd_out, "\tli $v0, 5\n");
                dprintf(fd_out, "\tsyscall\n");
                dprintf(fd_out, "\tsw $v0, %s_%i\n", 
                    symbol_table.symbols[quad.op1.var.ptr].ident,
                    symbol_table.symbols[quad.op1.var.ptr].scope);
            }
            break;
        
        case WRITE_QUAD:
            if (quad.op1.quad_op_type == QO_TEMP) {
                dprintf(fd_out, "\tli $v0, 1\n");
                dprintf(fd_out, "\tmove $a0, $t%i\n", quad.op1.temp.ptr);
                dprintf(fd_out, "\tsyscall\n");
            }
            else if (quad.op1.quad_op_type == QO_CST) {
                if (quad.op1.type == STRING) {
                    if (string_table.table_size <= 
                        string_table.last_ident_index + 1) {
                        string_table.table_size += INIT_TABLE_SIZE;
                        string_table.strings = realloc(string_table.strings,
                            string_table.table_size * sizeof(char *));
                        MCHECK(string_table.strings);
                    }
                    string_table.strings[string_table.last_ident_index] = 
                        quad.op1.const_string;
                    dprintf(fd_out, "\tli $v0, 4\n");
                    dprintf(fd_out, "\tla $a0, __printed_string_%i\n",
                        string_table.last_ident_index);
                    dprintf(fd_out, "\tsyscall\n");
                    string_table.last_ident_index ++;
                }
                else if (quad.op1.type == INT) {
                    dprintf(fd_out, "\tli $v0, 1\n");
                    dprintf(fd_out, "\tli $a0, %i\n", quad.op1.const_int);
                    dprintf(fd_out, "\tsyscall\n");
                }
            }
            break;

        case CALL_QUAD:
            dprintf(fd_out, "\tjal __func_label_%s\n", 
                symbol_table.symbols[quad.op1.var.ptr].ident);
            break;
        case CALL_AFF_QUAD:
            dprintf(fd_out, "\tjal __func_label_%s\n", 
                symbol_table.symbols[quad.op2.var.ptr].ident);
            dprintf(fd_out, "\tmove $t%i, $v0\n", quad.op1.temp.ptr);
            break;

        case PARAM_QUAD:
            if (quad.op1.quad_op_type == QO_CST) {
                dprintf(fd_out, "\taddiu $sp, $sp, -4\n");
                dprintf(fd_out, "\tli $t9, %i\n", quad.op1.const_int);
                dprintf(fd_out, "\tsw $t9, 0($sp)\n");
            }
            else {
                dprintf(fd_out, "\taddiu $sp, $sp, -4\n");
                dprintf(fd_out, "\tsw $t%i, 0($sp)\n", quad.op1.temp.ptr);   
            }
            break;

        case RETURN_UNIT_QUAD:
            dprintf(fd_out, "\tjr $ra\n\n");
            break;

        case RETURN_QUAD:
            if (quad.res.quad_op_type == QO_CST && quad.res.type == BOOL) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.res.const_bool);
                dprintf(fd_out, "\tmove $v0, $t9\n");
                dprintf(fd_out, "\tjr $ra\n\n");
            }
            else if (quad.res.quad_op_type == QO_CST && quad.res.type == INT) {
                dprintf(fd_out, "\tli $t9, %i\n", quad.res.const_int);
                dprintf(fd_out, "\tmove $v0, $t9\n");
                dprintf(fd_out, "\tjr $ra\n\n");
            }
            else {
                dprintf(fd_out, "\tmove $v0, $t%i\n", quad.res.temp.ptr);
                dprintf(fd_out, "\tjr $ra\n\n");
            }
            break;

        case EXIT_QUAD:
            dprintf(fd_out, "\tli $v0, 10\n");
            dprintf(fd_out, "\tsyscall\n\n");
            break;
    }
}

void write_main(char * program_name) {
    dprintf(fd_out, "# program : %s\n\t.text\n\t.globl main\nmain:\n",
        program_name);

    string_table.table_size = INIT_TABLE_SIZE;
    string_table.last_ident_index = 0;
    string_table.strings = malloc(INIT_TABLE_SIZE * sizeof(char *));
    MCHECK(symbol_table.symbols);

    for (int i = symbol_table.quad_main ; i < quad_table.nextquad ; i++) {
        dprintf(fd_out, "\n");
        write_instr(i);
    }
}

void write_functions() {
    struct symbol_t symbol;
    int quad_start = 0;
    int quad_end = 0;
    for (int i = 0; i < symbol_table.last_ident_index; i++) {
        symbol = symbol_table.symbols[i];
        if (symbol.var_func_par == FUNC_T) {
            quad_start = symbol.type.func.quad_start;
            quad_end = symbol.type.func.quad_end;
            dprintf(fd_out, "__func_label_%s:\n",symbol.ident);
            for (int j = symbol.type.func.nb_param; j > 0 ; j--)  {
                struct param_t parame = symbol_table.symbols[i+j].type.param;
                if (parame.typename->symbol_type == ARRAY_TYPE) {
                    int nb_element = 1;
                    for (int j = 0; j < parame.typename->len_range_list; j++) {
                        nb_element *= (parame.typename->range_array[j][1] 
                            - parame.typename->range_array[j][0] + 1); 
                    }
                    dprintf(fd_out, "\tlw $t9, 0($sp)\n");
                    dprintf(fd_out, "\tla $t8, %s_%i\n",
                        symbol_table.symbols[i+j].ident,
                        symbol_table.symbols[i+j].scope);

                    // loop to copy array
                    dprintf(fd_out, "\tli $t0, %i\n", nb_element);
                    dprintf(fd_out, "\tli $t1, 0\n");
                    dprintf(fd_out, "\t__label_%s_%i:\n", symbol.ident, j);
                    dprintf(fd_out, "\tbgt $t1, $t0, __label_%s_%i_end\n",
                        symbol.ident, j);
                    dprintf(fd_out, "\tlw $t7, 0($t9)\n");
                    dprintf(fd_out, "\tsw $t7, ($t8) \n");
                    dprintf(fd_out, "\taddiu $t9, $t9, 4\n");
                    dprintf(fd_out, "\taddiu $t8, $t8, 4\n");
                    dprintf(fd_out, "\taddi $t1, $t1, 1\n");
                    dprintf(fd_out, "\tj __label_%s_%i\n", symbol.ident, j);
                    dprintf(fd_out, "\t__label_%s_%i_end:\n", symbol.ident, j);

                    dprintf(fd_out, "\taddiu $sp, $sp, 4\n");
                }
                else {
                    dprintf(fd_out, "\tlw $t9, 0($sp)\n");
                    dprintf(fd_out, "\tsw $t9, %s_%i\n",
                        symbol_table.symbols[i+j].ident,
                        symbol_table.symbols[i+j].scope);
                    dprintf(fd_out, "\taddiu $sp, $sp, 4\n");
                }
            }
            for (int j = quad_start; j < quad_end; j++) {
                write_instr(j);
                dprintf(fd_out, "\n");
            }
        }
    }
}

void write_data() {
    dprintf(fd_out, "\n\t.data\n");
    for (int i = 0; i < symbol_table.last_ident_index; i++) {
        struct typename_t *cur_typename;
        if (symbol_table.symbols[i].var_func_par == VAR_T) {
            cur_typename = symbol_table.symbols[i].type.var.typename;
        }
        else if (symbol_table.symbols[i].var_func_par == PARAM_T) {
            cur_typename = symbol_table.symbols[i].type.param.typename;
        }
        else {
            continue;
        }
        if (cur_typename->symbol_type == ATOMIC_TYPE) {
            dprintf(fd_out, "\t%s_%i:\t.word 0\n",
                symbol_table.symbols[i].ident,
                symbol_table.symbols[i].scope);
        }
        else {
            int nb_element = 1;
            for (int j = 0; j < cur_typename->len_range_list; j++) {
                nb_element *= (cur_typename->range_array[j][1] 
                    - cur_typename->range_array[j][0] + 1); 
            }
            dprintf(fd_out, "\t%s_%i:\t.word ",
                symbol_table.symbols[i].ident,
                symbol_table.symbols[i].scope);
            for (int j = 0; j < nb_element -1; j++) {
                dprintf(fd_out, "0, ");
            }
            dprintf(fd_out, "0\n");
        }
    }
    for (int i = 0; i < string_table.last_ident_index; i++) {
        dprintf(fd_out, "\t__printed_string_%i: .asciiz %s\n",i , 
            string_table.strings[i]);
    }
}