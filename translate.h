#pragma once

#include "absyn.h"
#include "symbol.h"
#include "types.h"
#include "util.h"
#include "frame.h"

typedef struct TR_VarList_ * TR_VarList;
typedef struct TR_FunctionList_ * TR_FunctionList;
typedef struct TR_Var_ * TR_Var;
typedef struct TR_Function_ * TR_Function


struct TR_Function_ {
    S_Symbol name;
    F_Frame frame;
    TR_Function parent;
    TR_FunctionList children
};

struct TR_FunctionList_ {
    TR_Function head;
    TR_FunctionList tail;
};

void TR_append_function(TR_Function parent, TR_Function child);
void TR_add_param(TR_Function func, S_Symbol name, T_Type type);
void TR_add_var(TR_Function func, S_Symbol name, T_Type type);
void TR_print_functions(TR_Function func);
