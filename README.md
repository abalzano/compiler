# Scalpa Compiler

**BALZANO Antoine 2A SDIA**
**COLIN Simon 2A RIO**

Compilation project 2nd year engineering school

## 1 Installation and compiler usage
Installation using git:

*git clone https://https://github.com/abalzano/compiler.git && cd scalpa_compiler*

Installation from an archive:

unzip the archive and go into folder the folder named scalpa_compiler

In the scalpa_compiler folder run the command *make*
then run the compiler by using *./scalpa -tos -toq -version -h < demo/sierpinski*

*output.s* is the assembly program file created by the compiler
*demo* is a folder containing some scalpa program

## 2 Makefile commands
 - make -> create an executable file named scalpa
 - make test -> create an executable file named scalpa and immediately compile the scalpa program *sierpinski* in demo folder
 - make clean -> remove all files created by the make command

## 3 Program options

 - -version :      display the name of the project members
 - -tos :          display the symbol table
 - -toq :          display the quadruplet table
 - -o <name> :     *<name>* is the custom output file, by default mips program will be writen in *output.s*
 - -h              display help

