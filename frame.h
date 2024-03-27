#pragma once

#include "absyn.h"
#include "symbol.h"
#include "types.h"
#include "util.h"

typedef struct F_Frame_ * F_Frame;
typedef struct F_Var_ * F_Var;
typedef struct F_VarList_ * F_VarList;

struct F_Frame_ {
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
void F_print_frame(F_Frame frame);
