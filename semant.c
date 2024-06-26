/*
 * PA6; Sawyer Maloney and Jack Edwards
 * */

#include <stdlib.h>
#include "env.h"
#include "semant.h"


SEM_ExpType make_ExpType(TR_TransExp exp, T_Type type) {
    // seems a little too simple?
    SEM_ExpType exptype = malloc_checked(sizeof(*exptype));
    exptype->exp = exp;
    exptype->type = type;
    return exptype;
}

SEM_ExpType SEM_trans_var(S_Table venv, S_Table tenv, TR_Function func, A_Var var) {
    switch(var->kind) {
        case A_SIMPLE_VAR: {
                               // TODO how to get the function that we are in here
            return make_ExpType(func, (T_Type)S_look(venv, var->u.simple));
        }
        case A_FIELD_VAR: {
            // lowkey do not know how to get the type of the field from a fieldvar
            // A_Var var and Symbol sym
            SEM_ExpType trans_var = SEM_trans_var(venv, tenv, func, var->u.field.var); // gives us type
            if (trans_var->type != T_RECORD) {
                EM_error(var->pos, "Not a record type");
                return NULL;
            }
            // iterate through fields and return that type
            T_FieldList head = trans_var->type->u.record;

            while (head != NULL) {
                if (head->head->name == var->u.field.sym) {
                    return make_ExpType(trans_var->exp, head->head->type);
                }
                head = head->tail;
            }
            EM_error(var->pos, "Did not find record field in record type");
            return NULL; // TODO but not sure what it's referring to
        }
        case A_SUBSCRIPT_VAR: {
            // verify exp is an integer for array lookup (right?) return lvalue lookup val
            SEM_ExpType exp = SEM_trans_exp(venv, tenv, func, var->u.subscript.exp);
            if (exp->type->kind != T_INT) {
                EM_error(var->u.subscript.exp->pos, "array indexing must resolve to an integer");
            }
            // translate the var, make sure it is array type, return type of array elements
            SEM_ExpType array_type = SEM_trans_var(venv, tenv, func, var->u.subscript.var);
            if (array_type->type->kind != T_ARRAY) {
                EM_error(var->pos, "did not evaluate to array type\n");
                return NULL;
            }
            // alls well
            return make_ExpType(func, array_type->type->u.array);
        }
    }
    return NULL;
}

SEM_ExpType SEM_trans_exp(S_Table venv, S_Table tenv, TR_Function func, A_Exp exp) {
    switch (exp->kind) {
        case A_LET_EXP: { // from the book
            SEM_ExpType expty; // returning value of the type of this expression
            A_DecList d; // list of declarations, since we're in a let expression
            S_begin_scope(venv); // start the scopes, because we're adding new declarations
            S_begin_scope(tenv); // do we need to do that here? or are we going to do that in SEM_trans_dec?
            for (d = exp->u.let.decs; d; d=d->tail)
                SEM_trans_dec(venv, tenv, func, d->head); // iterate through the type declarations, adding each one (have to do this in transDec)
            expty = SEM_trans_exp(venv, tenv, func, exp->u.let.body); // now we evaluate the body
            if (!expty) {
                EM_error(exp->pos, "cannot establish type of in-clause");
                return make_SEM_ExpType(func, make_T_Void());
            }
            S_end_scope(tenv); // close the scopes, we're now out of the let expression entirely, outside "END"
            S_end_scope(venv);
            expty->exp = func;
            return expty;
        }
        case A_OP_EXP: { // another book example
            A_Oper oper = exp->u.op.oper; // get the operation
            SEM_ExpType left = SEM_trans_exp(venv, tenv, func, exp->u.op.left); // get the type on either side
            SEM_ExpType right = SEM_trans_exp(venv, tenv, func, exp->u.op.right); 
            if (oper == A_PLUS_OP || oper == A_MINUS_OP || A_TIMES_OP || A_DIVIDE_OP) { // error checking here
                if (left->type->kind != T_INT)
                    EM_error(exp->u.op.left->pos, "integer required");
                if (right->type->kind != T_INT)
                    EM_error(exp->u.op.right->pos, "integer required");
                return make_ExpType(func, make_T_Int()); // pretty sure this is the right function? prof. changed the names from the book
            }
        }
        case A_VAR_EXP: {
            // is an lvalue
            // need to translate the variable ie put it in venv, so pass to SEM_trans_var. Throw it into tran_var, return 
            // the ExpType it spits out
            SEM_ExpType exptype = SEM_trans_var(venv, tenv, func, exp->u.var);
            return exptype;
        }
        case A_INT_EXP: {
            // just has type, integer, so return make_T_Int();
            return make_ExpType(func, make_T_Int());
        }
        case A_STRING_EXP: {
            // ez type, just a string
            return make_ExpType(func, make_T_String());
        }
        case A_CALL_EXP: {
            // type of a call exp is the return type of the function
            // just a value in venv? so look up, if it is there, return that, if not, kill
            E_EnvEntry ret = (E_EnvEntry)S_look(venv, exp->u.call.func);
            if (ret == NULL) {
                EM_error(exp->pos, "function not defined");
            }
            return make_ExpType(func, ret->u.fun.result);
        }
        case A_NIL_EXP: {
            // has type NULL? or maybe INT and we catch the error somewhere else?
            return make_ExpType(func, make_T_Nil());
        }
        case A_RECORD_EXP: {
            // ID LBRACE records RBRACE
            // Do we need to check that the records are the right type? ****
            T_Type ret = (T_Type) S_look(tenv, exp->u.record.type);

            return make_ExpType(func, ret);
        }
        case A_SEQ_EXP: {
            // type is type of the last expression, so evaluate all the expressions and then return the last one
            A_ExpList seq = exp->u.seq;
            SEM_ExpType exptype = malloc(sizeof(*exptype));
            while (seq != NULL) {
                exptype = SEM_trans_exp(venv, tenv, func, seq->head);
                seq = seq->tail;
            }
            return exptype;
        }
        case A_ASSIGN_EXP: {
            // var := exp
            // just need to type check that both sides have the same type
            SEM_ExpType left = SEM_trans_var(venv, tenv, func, exp->u.assign.var);
            SEM_ExpType right = SEM_trans_exp(venv, tenv, func, exp->u.assign.exp);
            if (left != right) {
                EM_error(exp->pos, "assign statement does not share types");
            }
            return left;
        }
        case A_WHILE_EXP: {
           // throw error if test isn't an integer, otherwise return the value of body
           SEM_ExpType test = SEM_trans_exp(venv, tenv, func, exp->u.whilee.test);
           if ((void *) test != make_T_Int()) {
                EM_error(exp->pos, "condition must resolve to an integer");
           }
           return SEM_trans_exp(venv, tenv, func, exp->u.whilee.body);
        }
        case A_FOR_EXP: {
            //hi, lo need to be integers. Other than that, return the type of body
            SEM_ExpType lo = SEM_trans_exp(venv, tenv, func, exp->u.forr.lo);
            SEM_ExpType hi = SEM_trans_exp(venv, tenv, func, exp->u.forr.hi);
            if ((void *)lo != make_T_Int()) {
                EM_error(exp->u.forr.lo->pos, "low bound not an integer");
            }
            if ((void *)hi != make_T_Int()) {
                EM_error(exp->u.forr.hi->pos, "high bound not an integer");
            }
            // enter the frame and process decs
            S_begin_scope(tenv);
            S_begin_scope(venv);
            for (A_DecList declist = exp->u.let.decs; declist; declist = declist->tail) {
                SEM_trans_dec(venv, tenv, func, declist->head);
            }
            S_end_scope(tenv);
            S_end_scope(venv);
            return SEM_trans_exp(venv, tenv, func, exp->u.forr.body);
        }
        case A_BREAK_EXP: {
            // illegal while not in a loop, not sure we can deal with that here
            // just return NULL type?
            return make_ExpType(func, NULL);
        }
        case A_ARRAY_EXP: {
            // add the array type to venv?
            // ID [ exp ] of exp <-- make sure first exp is an integer?
            SEM_ExpType array_type = SEM_trans_exp(venv, tenv, func, exp->u.array.init);
            S_enter(venv, exp->u.array.type, array_type);
            return array_type;
        }
        // still have questions about the if statement here
        case A_IF_EXP: {
            // check that test is of type integer, and then tranlate then and elsee
            SEM_ExpType test = SEM_trans_exp(venv, tenv, func, exp->u.iff.test);
            if ((void *) test != make_T_Int()) {
                EM_error(exp->u.iff.test->pos, "condition needs to resolve to an integer");
            }
            // do these need to be the same type?
            SEM_ExpType then = SEM_trans_exp(venv, tenv, func, exp->u.iff.then);
            SEM_ExpType elsee = SEM_trans_exp(venv, tenv, func, exp->u.iff.elsee);
            // need to get rid of unused variable error so
            if (then != elsee) {
                EM_error(exp->u.iff.then->pos, "if statement needs to resolve to same type in then and else");
            }
            return then;
        }
    }
    return NULL;
}


void SEM_trans_dec(S_Table venv, S_Table tenv, TR_Function func, A_Dec dec) {
    switch(dec->kind) {
        case A_VAR_DEC: {
            SEM_ExpType init = SEM_trans_exp(venv, tenv, func, dec->u.var.init);
            if (dec->u.var.type != NULL && (void *)dec->u.var.type != (void *)init) {
                EM_error(dec->u.var.init->pos, "type mismatch in variable declaration");
            }
            // log the variable as the type of init, in case no type is given
            // assuming we are in scope, btw
            S_enter(venv, dec->u.var.var, init->type);
        }
        case A_TYPE_DEC_GROUP: {
            // iterate through all the type decs, translating the A_Type to T_Type and assigning their values into tenv
            // do we have to check if a type is already defined? or can we just redefine the type over it?
            if (dec->u.type->head->type->kind == A_RECORD_TYPE) {
                A_TypeDecList first_pass = dec->u.type;
                while (first_pass) {
                    // A_TypeDec head = first_pass->head;
                    
                }
            }
            else {
                A_TypeDec typedec = dec->u.type->head;
                while (typedec != NULL) {
                    // maybe check that the type isn't defined already
                    S_enter(tenv, typedec->name, SEM_trans_type(tenv, typedec->type));
                }
            }
        }
        case A_FUNCTION_DEC_GROUP: {
            // prof code
            for (A_FunDecList fdl = dec->u.function; fdl; fdl = fdl->tail) {
                A_FunDec fd = fdl->head;
                T_Type result_type;
                if (fd->result) {
                    result_type = S_look(tenv, fd->result);
                }
                else {
                    result_type = make_T_Void();
                }
                T_TypeList formal_types = SEM_make_formal_type_list(tenv, fd->params) // need
                S_enter(venv, fd->name, make_E_FunEntry(formal_types, result_type));
            }
            for (A_FunDecList fdl = dec->u.function; fdl; fdl = fdl->tail) {
                A_FunDec fd = fdl->head;
                TR_Function new_function = make_TR_Function(fd->name, make_F_Frame(func->frame->nesting_level + 1));
                TR_append_function(func, new_function);
                E_EnvEntry function_entry = S_look(venv, fd->name);
                S_begin_scope(venv);
                A_FieldList fields;
                T_TypeList formals;
                for (fields = fd->params, formals = function_entry->u.fun.formals; fields; fields = fields->tail, formals = formals->tail) {
                    TR_add_param(new_function, fields->head->name, formals->head);
                    S_enter(venv, fields->head->name, make_E_VarEntry(formals->head, func->frame->nesting_level, func->frame->end));
                }
                SEM_ExpType body_exp_type = SEM_trans_exp(venv, tenv, new_function, fd->body);
                S_end_scope(venv);
                if (!SEM_types_agree(function_entry->u.fun.result, body_exp_type->type)) {
                    EM_error("types do not agree");
                }
            }

            // end

            // TODO enter into new S_Table frame
            // first pass, putting function declarations in the venv
            A_FunDecList first_pass = dec->u.function;
            while (first_pass) {
                // enter each function declaration into the venv 
                // Given A_FieldList params --> T_TypeList formals
                T_Type return_type = S_look(tenv, first_pass->head->result);
                A_FieldList params = first_pass->head->params;
                T_TypeList formals = make_T_TypeList(NULL, NULL);
                while (params) {
                    // insert param into venv so the function knows it exists
                    T_Type params_type = S_look(tenv, params->head->type);
                    S_enter(venv, params->head->name, params_type);
                    formals = make_T_TypeList(params_type, formals);
                    params = params->tail;
                }
                S_enter(venv, first_pass->head->name, make_E_FunEntry(formals, return_type));
            } 
            // second pass -- evaluate all the bodies 
            A_FunDecList second_pass = dec->u.function;
            while (second_pass) {
                // enter params into scope and then execute bodies 
                S_begin_scope(tenv);
                S_begin_scope(venv);

                // setting up new frames
                TR_Function cur_func = malloc_checked(sizeof(*cur_func));
                F_Frame frame = malloc_checked(sizeof(*frame));
                cur_func->frame = frame;
                cur_func->frame->nesting_level = func->frame->nesting_level++;
                cur_func->name = second_pass->head->name;

                A_FieldList params = second_pass->head->params;
                while (params) {
                     T_Type params_type = S_look(tenv, params->head->type);
                     S_enter(venv, params->head->name, params_type);

                     // add param
                     TR_add_param(cur_func, params->head->name, params_type);
                     params = params->tail;
                }
                SEM_ExpType return_type = SEM_trans_exp(tenv, venv, func, second_pass->head->body);
                S_end_scope(tenv);
                S_end_scope(venv);
                // now enter function into venv with the updated type
                // by doing it again, will it shadow over the previous declaration?
                // looking up the previous one for the formals declaration...
                E_EnvEntry fun_entry = S_look(venv, second_pass->head->name);
                E_EnvEntry new_fun_entry = make_E_FunEntry(fun_entry->u.fun.formals, return_type->type);
                S_enter(venv, second_pass->head->name, new_fun_entry); 

                // now make function part of children
                TR_append_function(func, cur_func);
                second_pass = second_pass->tail;
            }
        }
    }
}

T_Type SEM_actual_type(S_Table tenv, T_Type type) {
    // chasing down type
    while (type->kind == T_NAME) {
            // look up the type in the name type decl, and iterate on it
            type = (T_Type)S_look(tenv, type->u.name.sym);
    }
    return type;
}

T_Type SEM_trans_type(S_Table tenv, A_Type type) {
    switch(type->kind) {
        case A_NAME_TYPE: {
            // S_Symbol name
            // have to get Symbol sym, T_Type type
            T_Type sym_type = S_look(tenv, type->u.name);
            if (sym_type == NULL) {
                // if not found
                EM_error(type->pos, "Type not found\n");
            }
            T_Type ret = malloc_checked(sizeof(*ret));
            ret->kind = T_NAME;
            ret->u.name.sym = type->u.name;
            ret->u.name.type = sym_type;
            return ret;
        }
        case A_RECORD_TYPE: {
            T_FieldList previous = NULL;
            A_FieldList list = type->u.record;
            while (list != NULL) {
                A_Field head = list->head;
                T_Type head_type = S_look(tenv, head->type);
                if (head_type == NULL) {
                    EM_error(type->pos, "Type not found while processing record\n");
                    return NULL;
                }
                // make T_Field
                T_Field tfield = malloc_checked(sizeof(*tfield));
                tfield->name = head->name;
                tfield->type = head_type;
                // now add tfield to tfieldlist
                T_FieldList tfieldlist = malloc_checked(sizeof(*tfieldlist));
                tfieldlist->head = tfield;
                tfieldlist->tail = previous;
                previous = tfieldlist;

                list = list->tail; // move one down
            }

            // now define the T_Type
            T_Type ret = malloc_checked(sizeof(*ret));
            ret->kind = T_RECORD;
            ret->u.record = previous;
            return ret;
        }
        case A_ARRAY_TYPE: {
            // S_Symbol array (the array type?)
            // T_Type array (also the array type)
            T_Type array_type = S_look(tenv, type->u.array);
            T_Type ret = malloc_checked(sizeof(*ret));
            if (array_type == NULL) {
                EM_error(type->pos, "did not get valid array type from lookup\n");
                return NULL;
            }
            ret->kind = T_ARRAY;
            ret->u.array = array_type;
            return ret;
        }
    }
    return NULL;
}

SEM_ExpType SEM_trans_prog(A_Exp prog) {
    S_Table venv = E_base_venv();
    S_Table tenv = E_base_tenv();
    // should use make_TR_Function with make_S_Symbol("main"), main_frame (main_frame from make_F_Frame)
    TR_Function main = malloc_checked(sizeof(*main));
    // should use make_F_frame
    F_Frame frame = malloc_checked(sizeof(*frame));
    S_Symbol name = make_S_Symbol("main");
    main->name = name;
    main->frame = frame;

    return SEM_trans_exp(venv, tenv, main, prog);
}
