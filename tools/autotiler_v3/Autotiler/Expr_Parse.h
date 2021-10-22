#ifndef __EXPR_PARSE_H__
#define __EXPR_PARSE_H__
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include  <stdint.h>
#include "AutoTilerLibTypes.h"
#include "AutoTilerLib.h"
#include "ModelCreate.h"


extern unsigned int Lineno;
int yylex(void);
void yyerror(const char*);
int yylex_destroy  (void);

extern void ParseExprColPosUpdate(char *Str);
extern ArgExpr_T *LastTypeNodeInList(ArgExpr_T *Expr);
extern ArgExpr_T *AddTypeNode(ArgExprOper_T Oper, int Qualifier, ArgExpr_T *Leaf);
extern ArgExpr_T *AddIdentNode(char *Ident, ArgExprOper_T Oper, char *SubOper, char *Space);
extern ArgExpr_T *AddIntNode(int Val);
extern ArgExpr_T *AddFloatNode(float Val);
extern ArgExpr_T *AddExprNode(ArgExprOper_T Oper, ArgExpr_T *Sel, ArgExpr_T *Left, ArgExpr_T *Right);
extern ArgExpr_T *AddExprIdentNode(ArgExpr_T *ExprIdent, ArgExprOper_T Oper, ArgExpr_T *ExprSubOper, ArgExpr_T *ExprSpace);


extern void ArgExprSetup();

typedef enum {
        T_UNDEF = 0,
        T_FLOAT = 1,
        T_INT = 2,
	T_IDENT = 3,
        T_ERROR = 4,
} EvalExprType_T;

typedef struct {
        EvalExprType_T  Type;
        unsigned int Invar;
        union {
                int Int;
                float Float;
		NameT *Ident;
        } V;
	NameT *CArg;
	NameT *KArg;
} EvalExpr_T;

extern ArgExpr_T *ParseArgExpr(char *String, ExprContext_T Ctxt);
extern int CheckArgExpr(ArgExpr_T *Expr, Kernel_T *KerCtxt, ExprContext_T Ctxt, int BindCount, CKernel_Arg_T **Formal, ArgBindingDescr_T **Actual, EvalExpr_T *Eval);
extern char *ProduceArgExpr(ArgExpr_T *Expr, ExprContext_T Ctxt);
extern char *ArgExprImage(ArgExpr_T *Expr, ExprContext_T Ctxt, Kernel_T *Ker, CKernel_Arg_T *ArgVal, KernelIteratorT ItSpace, int *IsInvar);
extern char *GraphArgExprImage(ArgExpr_T *Expr, GraphNode_T *Node);

extern ArgExpr_T *ArgExprTop;

#include "Expr_Parse.tab.h"
#endif
