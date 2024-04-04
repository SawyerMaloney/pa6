#include <stdlib.h>
#include <stdio.h>
#include "frame.h"


void F_add_param(F_Frame frame, F_Var param) {
    F_VarList p = malloc_checked(sizeof(*p));
    p->head = param;
    p->tail = frame->params;
    frame->params = p;
    frame->end += T_size(param->type);
}

void F_add_var(F_Frame frame, F_Var var) {
    // prof code
    if (!frame->vars) {
        frame->vars = make_F_VarList(var, NULL);
    }
    else {
        F_VarList vars = frame->vars;
        while (vars->tail) {
            vars = vars->tail;
        }
        vars->tail = make_F_VarList(var, NULL);
    }
    // end
    F_VarList p = malloc_checked(sizeof(*p));
    p->head = var;
    p->tail = frame->vars;
    frame->vars = p;
    frame->end += T_size(var->type); // this is what he has too
}

void F_print_frame(F_Frame frame) {
    printf("Nesting level: %d\n", frame->nesting_level);
    printf("\tEnd: %d\n", frame->end);
    printf("Params:\n");
    for (F_VarList f = frame->params; f ; f = f->tail) {
        printf("%s : %d", S_name(f->head->name), T_size(f->head->type));
    }
    for (F_VarList f = frame->vars; f ; f = f->tail) {
        printf("%s : %d", S_name(f->head->name), T_size(f->head->type));
    }
}
