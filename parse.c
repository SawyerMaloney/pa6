/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"
#include "semant.h"
#include "symbol.h"
#include "util.h"
#include "y.tab.h"

extern A_Exp absyn_root;
const char * const type_names[] = {
    "T_RECORD",
    "T_NIL",
    "T_INT",
    "T_STRING",
    "T_ARRAY",
    "T_NAME",
    "T_VOID"
};


/* Parse source file fname; 
 * return abstract syntax data structure.
 */
A_Exp parse(string fname) {
    EM_reset(fname);
    if (!yyparse()) {
        puts("Parsing successful!");
        return absyn_root;
    } else {
        fprintf(stderr, "Parsing failed\n");
        return NULL;
    }
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        fprintf(stderr,"usage: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    A_Exp program = parse(argv[1]);
    if (program) {
        puts("\nAbstract syntax:\n");
        pr_exp(stdout, program, 0);
        puts("\n");
        SEM_ExpType prog_exp_type = SEM_trans_prog(program);
        if (prog_exp_type) {
            printf("Type: %s\n", type_names[prog_exp_type->type->kind]);
        } else {
            puts("Type could not be established.");
        }
    }
    puts("\nDone.");
    return EXIT_SUCCESS;
}
