#include "traversal.h"
#include <assert.h>
#include <stdlib.h>

#undef DEBUG_PRINT

ATermList gTypeTable;

char * ofp_getArgNameStr(ATerm arg, char ** name)
{
   ATerm kind;
   assert(ATmatch(arg, "[<term>,<str>]", &kind, name));
   return *name;
}

ATerm ofp_getArgName(ATerm arg)
{
   ATerm kind, name;
   assert(ATmatch(arg, "[<term>,<term>]", &kind, &name));
   return name;
}

ATerm ofp_getArgKind(ATerm arg)
{
   ATerm kind, name;
   assert(ATmatch(arg, "[<term>,<term>]", &kind, &name));
   return kind;
}

ATbool ofp_isArgListKind(ATerm list)
{
   ATerm kind, name;
   assert(ATmatch(list, "[<term>,<term>]", &kind, &name));
   if (ATmatch(kind, "\"List\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isArgOptionKind(ATerm list)
{
   ATerm kind, name;
   assert(ATmatch(list, "[<term>,<term>]", &kind, &name));
   if (ATmatch(kind, "\"Option\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isArgOptionOptionKind(ATerm list)
{
   ATerm kind, name;
   assert(ATmatch(list, "[<term>,<term>]", &kind, &name));
   if (ATmatch(kind, "\"OptionOption\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isOptionOrKind(ATerm kind)
{
   if (ATmatch(kind, "\"Option\"") || ATmatch(kind, "\"Or\"")) {
      return ATtrue;
   }
   return ATfalse;
}

ATbool ofp_isStringType(ATerm term)
{
   if (ATmatch(term, "[\"String\"]")) {
      return ATtrue;
   }
   return ATfalse;
}

/**
 * Return a list of types associated with this name or None
 */
ATerm ofp_getTypeList(ATerm name, ATbool * isOptType)
{
   int i;

   for (i = 0; i <  ATgetLength(gTypeTable); i++) {
      ATerm typeName, typeList;
      ATbool matched = ATfalse;
      ATerm name_type = ATelementAt(gTypeTable, i);
      if (ATmatch(name_type, "Type(<term>,<term>)", &typeName, &typeList)) {
         *isOptType = ATfalse;
         matched = ATtrue;
      }
      else if (ATmatch(name_type, "OptType(<term>,<term>)", &typeName, &typeList)) {
         *isOptType = ATtrue;
         matched = ATtrue;
      }
      if (matched && ATisEqual(name, typeName)) {
         return typeList;
      }
   }

   *isOptType = ATfalse;
   return ATmake("None");
}

ATbool ofp_isTypeNameList(ATerm type)
{
   if (! ATmatch(type, "None")) {
      ATermList nameList = (ATermList) type;
      if (ATgetLength((ATermList) type) > 1) {
         return ATtrue;
      }
   }
   return ATfalse;
}

ATerm ofp_getArgType(ATerm term, ATbool * isOptType)
{
   int i;
   ATerm kind, name;

   if (ATmatch(term, "[<term>,<term>]", &kind, &name)) {
      // MATCHED (kind, name)
   }
   else {
      *isOptType = ATfalse;
      return ATmake("None");
   }

   for (i = 0; i <  ATgetLength(gTypeTable); i++) {
      ATerm typeName, typeList;
      ATbool matched = ATfalse;
      ATerm name_type = ATelementAt(gTypeTable, i);
      if (ATmatch(name_type, "Type(<term>,<term>)", &typeName, &typeList)) {
         matched = ATtrue;
         *isOptType = ATfalse;
      }
      else if (ATmatch(name_type, "OptType(<term>,<term>)", &typeName, &typeList)) {
         matched = ATtrue;
         *isOptType = ATtrue;
      }
      if (matched && ATisEqual(name, typeName)) {
         return typeList;
      }
   }

   *isOptType = ATfalse;
   return ATmake("None");
}

/** Make traversal calls for a production
 */
ATbool ofp_build_node_traversal(ATerm name, ATerm arg, ATerm kind)
{
   ATerm typeList;
   ATbool isOptType;
   char * root_name = "None";
   char * arg_name  = "None";

   arg_name = ofp_getArgNameStr(arg, &arg_name);
   typeList = ofp_getArgType(arg, &isOptType);

   assert(ATmatch(name, "<str>", &root_name));
   if (isOptType) {
      // Some optional productions lead to temporary terms that can be reduced to (Some(<term>)).
      // For example, ('PROGRAM' ProgramName?)? in EndProgramStmt produces
      // Some((Some(ProgramName("main")))). Get the argname from the type namelist.
      //
      assert(ATmatch(typeList, "[<str>]", &arg_name));
   }

   // handle primitive types and terminals
   //

   if (ATisEqual(ofp_getArgName(arg), ATmake("\"Ident\""))) {
      printf("::::::::::::found Ident:::::::: %s\n", ATwriteToString(arg));
      printf("      char * %s_val;\n", arg_name);
      printf("      if (ATmatch(%s_term.term, ", arg_name);
      printf("\"<str>\"");
      printf(", &%s_val)) {\n", arg_name);
      printf("         // MATCHED %s\n", arg_name);
      printf("      } else return ATfalse;\n\n");
   }

   else if (ofp_isStringType(typeList)) {
      printf("      char * %s_val;\n", arg_name);
      printf("      if (ATmatch(%s_term.term, ", arg_name);
      if (ofp_isArgOptionKind(arg)) printf("\"Some(<str>)\"");
      else                          printf("\"<str>\"");
      printf(", &%s_val)) {\n", arg_name);
      printf("         // MATCHED %s\n", arg_name);
      if (ofp_isArgOptionKind(arg))  printf("      }\n\n");
      else                           printf("      } else return ATfalse;\n\n");
   }

   // handle list type
   //
   else if (ofp_isArgListKind(arg)) {
      printf("      OFP_Traverse %s;\n", arg_name);
      printf("      ATermList %s_tail = (ATermList) ATmake(\"<term>\", %s_list.term);\n", arg_name, arg_name);
      printf("      while (! ATisEmpty(%s_tail)) {\n", arg_name);
      printf("         %s.term = ATgetFirst(%s_tail);\n", arg_name, arg_name);
      printf("         %s_tail = ATgetNext(%s_tail);\n", arg_name, arg_name);
      printf("         if (ofp_traverse_%s(%s.term, &%s)) {\n", arg_name, arg_name, arg_name);
      printf("            // MATCHED %s\n", arg_name);
      if (! ATmatch(kind, "\"Or\""))  printf("         } else return ATfalse;\n");
      else                            printf("         }\n");
      printf("      }\n");
   }

   // handle generic type
   //
   else {
      printf("      OFP_Traverse %s;\n", arg_name);
      printf("      if (ATmatch(%s_term.term, ", root_name);
      if (ofp_isArgOptionOptionKind(arg)) printf("\"Some((Some(<term>)))\"");
      else if (ofp_isArgOptionKind(arg))  printf("\"Some(<term>)\"");
      else                                printf("\"<term>\"");
      printf(", &%s.term)) {\n", arg_name);

      printf("         if (ofp_traverse_%s(%s.term, &%s)) {\n", arg_name, arg_name, arg_name);
      printf("            // MATCHED %s\n", arg_name);
      if (ATmatch(kind, "\"Or\""))  printf("            return ATtrue;\n");
      printf("         } else return ATfalse;\n");

      if (ofp_isOptionOrKind(kind))  printf("      }\n\n");
      else                           printf("      } else return ATfalse;\n\n");
   }

   return ATtrue;
}

ATbool ofp_traverse_FunType_arg(ATerm term, pOFP_Traverse FunType_arg)
{
   ATbool isOptType;

#ifdef DEBUG_PRINT
   printf("\nFunType_arg: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "ConstType(Sort(<str>,<term>))", &FunType_arg->pre, &FunType_arg->term)) {
      if (ATmatch(FunType_arg->term, "[SortNoArgs(<str>)]", &FunType_arg->post)) {
         // MATCHED FunType_arg_type
         ATerm type_name = ATmake("<str>", (char*)FunType_arg->post);
         ATerm type_list = ofp_getTypeList(type_name, &isOptType);
         if (isOptType) {
            FunType_arg->pre = "OptionOption";
         }

      } else return ATfalse;
   }
   else if (ATmatch(term, "ConstType(SortNoArgs(<str>))", &FunType_arg->post)) {
      // MATCHED FunType_arg_type
      ATerm type_name = ATmake("<str>", (char*)FunType_arg->post);
      ATerm type_list = ofp_getTypeList(type_name, &isOptType);

      FunType_arg->pre = "None";
      if (! ATmatch(type_list, "None")) {
         ATermList nameList = (ATermList) type_list;
         if (ATgetLength(nameList) > 1) {
            FunType_arg->pre = "Or";
         }
      }
   }
   else {
      return ATfalse;
   }
   FunType_arg->term = ATmake("[<str>,<str>]", (char*)FunType_arg->pre, (char*)FunType_arg->post);

   return ATtrue;
}

ATbool ofp_traverse_FunType_result(ATerm term, pOFP_Traverse FunType_result)
{
#ifdef DEBUG_PRINT
   printf("\nFunType_result: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "ConstType(SortNoArgs(<str>))", &FunType_result->post)) {
      // MATCHED FunType_result
      return ATtrue;
   }

   return ATfalse;
}

ATbool ofp_traverse_FunType(ATerm term, pOFP_Traverse FunType)
{
   int i;
   char * percent_s = "%s";
   char * comma = "";
   char * name = "None";
   ATermList args = (ATermList) ATmake("[]");

#ifdef DEBUG_PRINT
   printf("\nFunType: %s\n", ATwriteToString(term));
#endif

   OFP_Traverse FunType_args, FunType_result;
   if (ATmatch(term, "FunType(<term>,<term>)", &FunType_args.term, &FunType_result.term) ) {

   printf("---------args------- %s\n", ATwriteToString(FunType_args.term));
      if (ofp_traverse_FunType_result(FunType_result.term, &FunType_result)) {
         // MATCHED FunType_result
         name = (char*) FunType_result.post;
      } else return ATfalse;
      printf("---------name------- %s\n", name);

      ATermList FunType_args_tail = (ATermList) ATmake("<term>", FunType_args.term);
      while (! ATisEmpty(FunType_args_tail)) {
         OFP_Traverse FunType_arg;
         FunType_arg.term = ATgetFirst(FunType_args_tail);
         FunType_args_tail = ATgetNext(FunType_args_tail);

         if (ofp_traverse_FunType_arg(FunType_arg.term, &FunType_arg)) {
            // MATCHED FunType_arg
            args = ATappend(args, FunType_arg.term);
         } else return ATfalse;
      }

   }

   /** Output traversal function header information
    */
   printf("\n");
   printf("//========================================================================================\n");
   printf("// %s\n", name);
   printf("//----------------------------------------------------------------------------------------\n");
   printf("ATbool ofp_traverse_%s(ATerm term, pOFP_Traverse %s)\n", name, name);
   printf("{\n");
   printf("#ifdef DEBUG_PRINT\n");
   printf("   printf(\"%s: %s\\n\", ATwriteToString(term));\n", name, percent_s);
   printf("#endif\n\n");

   /** Declare input (args) to production 
    */
   comma = "";
   printf("   OFP_Traverse");
   for (i = 0; i <  ATgetLength(args); i++) {
      char * arg_name = "None";
      ATerm arg = ATelementAt(args, i);
      ATbool list_type = ofp_isArgListKind(arg);
      printf("%s %s", comma, ofp_getArgNameStr(arg, &arg_name));
      if (list_type) printf("_list");
      else           printf("_term");
      comma = ",";
   }
   printf(";\n");

   /** Traverse input (args) to production 
    */
   comma = "";
   // replace production name with the name given to the ATerm
   printf("   if (ATmatch(term, \"%s(", (char*) FunType->pre);
   for (i = 0; i <  ATgetLength(args); i++) {
      printf("%s<term>", comma);
      comma = ",";
   }
   printf(")\"");

   for (i = 0; i <  ATgetLength(args); i++) {
      char * arg_name = "None";
      ATerm arg = ATelementAt(args, i);
      ATbool list_type = ofp_isArgListKind(arg);

      printf(", &%s", ofp_getArgNameStr(arg, &arg_name));
      if (list_type) printf("_list");
      else           printf("_term");
      printf(".term");
   }
   printf(")) {\n\n");

   /** Call traversal for input (arg) to production 
    */
   for (i = 0; i <  ATgetLength(args); i++) {
      ATbool isOptType;
      ATerm arg = ATelementAt(args, i);
      ATerm type = ofp_getArgType(arg, &isOptType);
      ATerm kind = ofp_getArgKind(arg);
      ATerm name = ofp_getArgName(arg);
      printf(".....arg....... %s\n", ATwriteToString(arg));
      printf(".....type...... %s\n", ATwriteToString(type));
      printf(".....kind...... %s\n", ATwriteToString(kind));
      printf(".....name...... %s\n", ATwriteToString(name));

      if (ofp_isTypeNameList(type)) {
         int j;
         ATermList nameList = (ATermList) type;
         for (j = 0; j < ATgetLength(nameList); j++) {
            arg = ATmake("[<term>,<term>]", kind, ATelementAt(nameList, j));
            ofp_build_node_traversal(name, arg, kind);
         }
         // This is a production with or rules so one of the traversals must have
         // matched and returned ATtrue. So return ATfalse in case none matched.
         printf("      return ATfalse; /* for set of OR productions */\n");
      }
      else {
         printf("building::::::::: %s\n", ATwriteToString(ofp_getArgName(arg)));
         ofp_build_node_traversal(ofp_getArgName(arg), arg, kind);
      }
   }

   printf("\n      return ATtrue;\n");
   printf("   }\n");
   printf("\n   return ATfalse;\n");
   printf("}\n");

   return ATtrue;
}

ATbool ofp_traverse_OpDecl(ATerm term, pOFP_Traverse OpDecl)
{
#ifdef DEBUG_PRINT
   printf("\nOpDecl: %s\n", ATwriteToString(term));
#endif

   OFP_Traverse Name, FunType;
   if (ATmatch(term, "OpDecl(<term>,<term>)", &Name.term, &FunType.term) ) {

      char * String;
      if (ATmatch(Name.term, "<str>", &String)) {
         // MATCHED OpDecl name
         FunType.pre = String;
      }
      if (ofp_traverse_FunType(FunType.term, &FunType)) {
         // MATCHED FunType
      } else return ATfalse;

      return ATtrue;
   }

   return ATfalse;
}

ATbool ofp_traverse_OpDeclInj(ATerm term, pOFP_Traverse OpDeclInj)
{
   ATerm name, type, opt;
   int isOptType = 0;

   if (ATmatch(term, "OpDeclInj(<term>)", &OpDeclInj->term)) {
#ifdef DEBUG_PRINT
      printf("\nofp_traverse_OpDeclInj: %s\n", ATwriteToString(OpDeclInj->term));
#endif
      if (ATmatch(OpDeclInj->term, "FunType(<term>,<term>)", &type, &name)) {
         if (ATmatch(type, "[ConstType(Sort(<term>,<term>))]", &opt, &type)) {
            // MATCHED option object type
            if (ATmatch(type, "[SortNoArgs(<term>)]", &type) && ATisEqual(opt, ATmake("\"Option\""))) {
               isOptType = 1;
            } else return ATfalse;
         }
         else if (ATmatch(type, "[ConstType(SortNoArgs(<term>))]", &type)) {
            // MATCHED object type
         }
         if (ATmatch(name, "ConstType(SortNoArgs(<term>))", &name)) {
            // MATCHED object name
         } else return ATfalse;
      } else return ATfalse;

      if (isOptType) {
         OpDeclInj->term = ATmake("OptType(<term>,[<term>])", name, type);
      }
      else {
         OpDeclInj->term = ATmake("Type(<term>,[<term>])", name, type);
      }

      return ATtrue;
   }
   return ATfalse;
}

ATermList ofp_coalesceTypeTable(ATermList oldTable)
{
   // Assumes:
   //  1. Contains list of terms Type(<str>,<list>) or OptType(<str>,<list>)
   //      a. <str> is type name
   //      b. <list> is [type] of length 1
   //  2. Portions of table to be coalesced are in order
   //  3. If OptType must match "(Some(<term>))"
   //
   ATerm head;
   int isOptType;

   ATermList table = (ATermList) ATmake("[]");
   ATermList types = (ATermList) ATmake("[]");
   ATermList tail  = (ATermList) ATmake("<term>", oldTable);

   head = ATgetFirst(tail);
   tail = ATgetNext(tail);

   while (1) {
      ATerm headName, headType, next, nextName, nextType;

      if (ATisEmpty(tail)) next = ATmake("Type(None,[None])");
      else                 next = ATgetFirst(tail);

      if      ( ATmatch(head, "Type(<term>,[<term>])",    &headName, &headType) ) isOptType = 0;
      else if ( ATmatch(head, "OptType(<term>,[<term>])", &headName, &headType) ) isOptType = 1;
      else assert(0); 

      assert(    ATmatch(next, "Type(<term>,[<term>])",    &nextName, &nextType)
              || ATmatch(next, "OptType(<term>,[<term>])", &nextName, &nextType)
            );

      types = ATappend(types, headType);

      // check for need to coalesce
      if (! ATisEqual(headName, nextName)) {
         if (isOptType) {
            table = ATappend((ATermList)table, ATmake("OptType(<term>,<term>)", headName, types));
         } else {
            table = ATappend((ATermList)table, ATmake(   "Type(<term>,<term>)", headName, types));
         }
         types = (ATermList) ATmake("[]");
         if (ATisEmpty(tail)) break;
      }

      head = ATgetFirst(tail);
      tail = ATgetNext(tail);
   }

   return table;
}

ATbool ofp_traverse_Constructors(ATerm term, pOFP_Traverse Constructors)
{
#ifdef DEBUG_PRINT
   printf("\nConstructors: %s\n", ATwriteToString(term));
#endif

   OFP_Traverse OpDecl_list;
   if (ATmatch(term, "Constructors(<term>)", &OpDecl_list.term) ) {
      gTypeTable = (ATermList) ATmake("[]");

      /* First build the type table.  It is needed when matching productions.
       */
      ATermList OpDeclInj_tail = (ATermList) ATmake("<term>", OpDecl_list.term);
      while (! ATisEmpty(OpDeclInj_tail)) {
         OFP_Traverse OpDeclInj;
         OpDeclInj.term = ATgetFirst(OpDeclInj_tail);
         OpDeclInj_tail = ATgetNext(OpDeclInj_tail);

         if (ofp_traverse_OpDeclInj(OpDeclInj.term, &OpDeclInj)) {
            // MATCHED OpDeclInj
            gTypeTable = ATappend(gTypeTable, OpDeclInj.term);
         }
      }

      /* Coalesce the type table so there is one name per list of types
       */
      gTypeTable = ofp_coalesceTypeTable(gTypeTable);

      ATermList OpDecl_tail = (ATermList) ATmake("<term>", OpDecl_list.term);
      while (! ATisEmpty(OpDecl_tail)) {
         OFP_Traverse OpDecl;
         OpDecl.term = ATgetFirst(OpDecl_tail);
         OpDecl_tail = ATgetNext(OpDecl_tail);

         if (ofp_traverse_OpDecl(OpDecl.term, &OpDecl)) {
            // MATCHED OpDecl
         }
      }

      printf("\nTYPES: %s\n", ATwriteToString((ATerm) gTypeTable));

      return ATtrue;
   }

   return ATfalse;
}

ATbool ofp_traverse_Signature(ATerm term, pOFP_Traverse Signature)
{
#ifdef DEBUG_PRINT
   printf("\nSignature: %s\n", ATwriteToString(term));
#endif

   OFP_Traverse Constructors_list;
   if (ATmatch(term, "Signature(<term>)", &Constructors_list.term) ) {

      ATermList Constructors_tail = (ATermList) ATmake("<term>", Constructors_list.term);
      while (! ATisEmpty(Constructors_tail)) {
         OFP_Traverse Constructors;
         Constructors.term = ATgetFirst(Constructors_tail);
         Constructors_tail = ATgetNext(Constructors_tail);

         if (ofp_traverse_Constructors(Constructors.term, &Constructors)) {
            // MATCHED Constructors
         } else return ATfalse;
      }

      return ATtrue;
   }

   return ATfalse;
}

ATbool ofp_traverse_Module(ATerm term, pOFP_Traverse Module)
{
#ifdef DEBUG_PRINT
   printf("Module: %s\n", ATwriteToString(term));
#endif

   OFP_Traverse Name, Signature_list;
   if (ATmatch(term, "Module(<term>,<term>)", &Name.term, &Signature_list.term) ) {

      char * String;
      if (ATmatch(Name.term, "<str>", &String)) {
         // MATCHED module name
         printf("\nModule name: %s\n", String);
      }

      ATermList Signature_tail = (ATermList) ATmake("<term>", Signature_list.term);
      while (! ATisEmpty(Signature_tail)) {
         OFP_Traverse Signature;
         Signature.term = ATgetFirst(Signature_tail);
         Signature_tail = ATgetNext(Signature_tail);

         if (ofp_traverse_Signature(Signature.term, &Signature)) {
            // MATCHED Signature
         } else return ATfalse;
      }

      return ATtrue;
   }

   return ATfalse;
}
