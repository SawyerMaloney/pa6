#pragma once

#include "absyn.h"
#include "symbol.h"
#include "types.h"
#include "util.h"

typedef void * TR_TransExp;

typedef struct SEM_ExpType_ * SEM_ExpType;

struct SEM_ExpType_ {
    TR_TransExp exp;
    T_Type type;
};

SEM_ExpType make_ExpType(TR_TransExp exp, T_Type type);
SEM_ExpType SEM_trans_var(S_Table venv, S_Table tenv, A_Var var);
SEM_ExpType SEM_trans_exp(S_Table venv, S_Table tenv, A_Exp exp);
void SEM_trans_dec(S_Table venv, S_Table tenv, A_Dec dec);
T_Type SEM_trans_type(S_Table tenv, A_Type type);
SEM_ExpType SEM_trans_prog(A_Exp prog);
