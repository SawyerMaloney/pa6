#### Preguntas
1. How to type check field vars
    - Check that type of lookup is record
    - in T_Type, there is a record variant that has a fieldlist (list of names and types)
    - Loop through record names, return type, return error if it does not exist and moveon 
2. Is my subscript var right?
    - Look up variable, make sure that it exists and has array type **important, need to do this**
    - Look up trans_exp for the exp
        - has to be T_Int
        - can use "kind" to check
3. make_ExpType --> too simple? -- maybe I have to be translating to T_Type before passing in --> correct
    - Good
4. Checking type equality? NULL type?
    - It depends
    - True with record types
    - Might match against all nil_types
    - Helper function in semant.c to check types using .kind field of T_Type
    - The exception to the way I'm doing it is nil
    - Name type
        - Will have some other type in the type field, chase down until you get to a non-name type
        - Function ActualType -- passed into SEM_types_agree
        - Except in decs and function processing, for mutually dependent
        - For type declarations
            - Go through the whole list of type declarations in a group
            - enter each one tdl->head->name, get type from trans_type, these are name not actual types
            - Go through the list again, doing one level of look up for any record type with a name type
                - If it doesn't exist, aka name has not been resolved yet, then the field is unresolved
                - Check that a definitional cycle where you might not get a type to a sign 
            - for functions, go through the declarations before, then go through all the bodies, so the typing are in the symbol tables before you go through the functions
5. Is my syntax/strategy for the lookup correct?
6. Record_exp --> do we need to be iterating through the records and type checking them?
7. Break?
8. Array statement --> since not a definition, do we just type check the values against the type? Or return the array type?
9. Are we always calling SEM_trans_type to get T_Type from A_Type?
10. Do we have to check that types aren't already declared?
11. What does trans_type even look like? **Probably want this one answered**
    - I think just use the T_Type definition to translate the given information from the tenv and A_Type 


#### Notes
1. Check canvas for fixing declarations in function and stuff
2. In CALL_EXP, need to check typing of all the arguments (does this entail calling trans_exp on them?)
3. Something about name types and chasing down types?
4. We need to implement the case switch for all operations btw

#### Mutually recursive function calls and types
- Do a first pass, putting recursive functions in the table with a 'hole'
- SEM_actual_type
    - Iterate down a chain of name variables, until you get to a real (aka not named) type
- Mutual recursion in types have to be sequential in declarations
- For record declarations:
    - Check that if you can chase the name type down, then that the type agrees. Otherwise, make a name type
        - Look up type
        - If it is in the symbol table, great, you have an actual type listing, so you can check that you've got the right type
        - If it isn't, then make a name type with NULL typing and put it in the FieldList for the record type
            - This so that the next time you come around, you can update the record type and see if you have an actual type defined somewhere
            - Update the field in the record as the other records as well as a name type with a field name and NULL as it's type
                - It's okay that the type is NULL, we don't need to resolve all of the types of all the fields of 'that field'--> being the record 
- For function declarations
    - Make a function header in venv
        - A listing under every function in the group, consisting of function return and formals type
            - All you need for type checking because you have all the types of all the bodies of all the functoins
            - Just matching that the arguments match the types of what was passed in, and the body expression matches the output
        - Type check the body because it may have a call to some function in the same group
