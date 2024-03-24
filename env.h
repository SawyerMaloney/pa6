#include "types.h"

typedef struct E_EnvEntry_ * E_EnvEntry;

struct E_EnvEntry_ {
    enum { E_VAR_ENTRY, E_FUN_ENTRY } kind;
    union {
        struct { T_Type type; } var;
        struct { T_TypeList formals; T_Type result; } fun;
    } u;
};

E_EnvEntry make_E_VarEntry(T_Type type);
E_EnvEntry make_E_FunEntry(T_TypeList formals, T_Type result);
S_Table E_base_tenv();
S_Table E_base_venv();
