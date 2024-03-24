#include <stdlib.h>

#include "env.h"
#include "symbol.h"
#include "table.h"
#include "types.h"
#include "util.h"

E_EnvEntry make_E_VarEntry(T_Type type) {
    E_EnvEntry entry = malloc_checked(sizeof(*entry));
    entry->kind = E_VAR_ENTRY;
    entry->u.var.type = type;
    return entry;
}

E_EnvEntry make_E_FunEntry(T_TypeList formals, T_Type result) {
    // Fun entry!
    E_EnvEntry entry = malloc_checked(sizeof(*entry));
    entry->kind = E_FUN_ENTRY;
    entry->u.fun.formals = formals;
    entry->u.fun.result = result;
    return entry;
}

S_Table E_base_tenv() {
    // should have the basic types; INT and STRING
    S_Table table = S_empty();
    // do we have to enter the scope?
    // Reasoning why not -- because we only enter the scope when we need to access values? 
    // Or (maybe) because when we enter the scope, we have to exit it, and how do we exit it when we need to return it and use it?
    
    // from book:
    // This maps the symbol int to Ty_Int and string to Ty_String, so using make_S_Symbol?

    S_enter(table, make_S_Symbol("int"), make_T_Int());
    S_enter(table, make_S_Symbol("string"), make_T_String());
    return table;
}

S_Table E_base_venv() {
    // includes all the function params and return types from the appendix :)
    /*
     * print (s : string)
     * flush()
     * getchar() : string
     * ord(s : string) : int
     * chr(i : int) : string
     * size(s : string) : int
     * substring(s : string, first : int, n : int) : string
     * concat(s1 : string, s2 : string) : string
     * not(i : int) : int
     * exit(i : int)
     */

    S_Table table = S_empty();
    // idea here: enter into the table, mapped to print, a function entry with string formal and no result
    S_enter(table, make_S_Symbol("print"), make_E_FunEntry(make_T_TypeList(make_T_String(), NULL), NULL));
    S_enter(table, make_S_Symbol("flush"), make_E_FunEntry(NULL, NULL)); // since it doesn't take anything
    S_enter(table, make_S_Symbol("getchar"), make_E_FunEntry(NULL, make_T_String()));
    S_enter(table, make_S_Symbol("ord"), make_E_FunEntry(make_T_TypeList(make_T_String(), NULL), make_T_Int()));
    S_enter(table, make_S_Symbol("chr"), make_E_FunEntry(make_T_TypeList(make_T_Int(), NULL), make_T_String()));
    S_enter(table, make_S_Symbol("size"), make_E_FunEntry(make_T_TypeList(make_T_String(), NULL), make_T_Int()));
    S_enter(table, make_S_Symbol("substring"), make_E_FunEntry(make_T_TypeList(make_T_String(), make_T_TypeList(make_T_Int(), make_T_TypeList(make_T_Int(), NULL))), make_T_String()));
    S_enter(table, make_S_Symbol("concat"), make_E_FunEntry(make_T_TypeList(make_T_String(), make_T_TypeList(make_T_String(), NULL)), make_T_String()));
    S_enter(table, make_S_Symbol("not"), make_E_FunEntry(make_T_TypeList(make_T_Int(), NULL), make_T_Int()));
    S_enter(table, make_S_Symbol("exit"), make_E_FunEntry(make_T_TypeList(make_T_Int(), NULL), NULL));

    return table;
}

