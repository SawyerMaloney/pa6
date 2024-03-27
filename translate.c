#include <stdlib.h>
#include <stdio.h>
#include "translate.h"


void TR_append_function(TR_Function parent, TR_Function child) {
    if (parent->children == NULL) {
        // need to make our own!
        TR_FunctionList p = malloc_checked(sizeof(*p));
        p->head = child;
        parent->children = p;
    }
    else {
        TR_FunctionList p = malloc_checked(sizeof(*p));
        p->head = child;
        p->tail = parent->children;
        parent->children = p;
    }
}

void TR_add_param(TR_Function func, S_Symbol name, T_Type type) {
    F_Var param = malloc_checked(sizeof(*param));
    param->name = name;
    param->type = type;

    F_add_param(func->frame, param);
}

void TR_add_var(TR_Function func, S_Symbol name, T_Type type) {
    F_Var var = malloc_checked(sizeof(*var));
    var->name = name;
    var->type = type;

    F_add_var(func->frame, var);
}

void TR_print_functions(TR_Function func) {
    if (func->parent == NULL) {
        printf("Function %s\n", S_name(func->name));
    }
    else {
        printf("Function %s with parent %s", S_name(func->name), S_name(func->parent->name));
    }
    F_print_frame(func->frame);
    for (TR_FunctionList fl = func->children; fl ; fl = fl->tail) {
        TR_print_functions(fl->head);
    }
}
