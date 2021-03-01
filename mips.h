#ifndef MIPS_H
#define MIPS_H




/**
 * @brief generates mips instructions corresponding
 * to a quadruplet.
 * @param i number of the quadruplet to be processed 
 */
void write_instr(int i);


/**
 * @brief writes generic mips instruction for a program,
 * creates the table containing the strings used in the 
 * program and calls write_instr for each quadruplet.
 * @param program_name is the name of the program
 */
void write_main(char * program_name);


/**
 * @brief handles the generation of mips instructions for functions.
 * Writes the code to take out the arguments of the stack and then
 * calls write_instr for all the quadruplets related to the function.
 */
void write_functions();


/**
 * @brief writes the mips code for the data segment.
 */
void write_data();

#endif