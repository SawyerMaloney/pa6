#pragma once

#include "absyn.h"
#include "symbol.h"
#include "types.h"
#include "util.h"

typedef struct F_Frame_ * F_Frame;

struct F_Frame {
    int nesting_level;
    int end;
    F_VarList params;
    F_VarList vars;
};

struct F_VarList_ {
    F_Var head;
    F_VarList tail;
};

struct F_Var_ {
    S_Symbol name;
    T_Type type;
};

void F_add_param(F_Frame frame, F_Var param);
void F_add_var(F_Frame frame, F_Var var);
void print_frame(F_Frame frame);
