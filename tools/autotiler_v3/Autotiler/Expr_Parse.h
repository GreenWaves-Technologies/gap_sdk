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

extern void ParserSetNameType(int TypeMode);
extern ArgExpr_T *AddTypeNode(ArgExprOper_T Oper, int Qualifier, ArgExpr_T *Leaf);
extern ArgExpr_T *AddIdentNode(char *Ident, ArgExprOper_T Oper, char *SubOper, char *Space);
extern ArgExpr_T *AddIntNode(int Val);
extern ArgExpr_T *AddFloatNode(float Val);
extern ArgExpr_T *AddExprNode(ArgExprOper_T Oper, ArgExpr_T *Sel, ArgExpr_T *Left, ArgExpr_T *Right);

extern void ArgExprSetup();

typedef enum {
        T_UNDEF = 0,
        T_FLOAT = 1,
        T_INT = 2,
        T_ERROR = 3,
} EvalExprType_T;

typedef struct {
        EvalExprType_T  Type;
        unsigned int Invar;
        union {
                int Int;
                float Float;
        } V;
} EvalExpr_T;

extern ArgExpr_T *ParseArgExpr(char *String, ExprContext_T Ctxt);
extern int CheckArgExpr(ArgExpr_T *Expr, ExprContext_T Ctxt, EvalExpr_T *Eval);
extern char *ProduceArgExpr(ArgExpr_T *Expr, ExprContext_T Ctxt);
extern char *ArgExprImage(ArgExpr_T *Expr, ExprContext_T Ctxt, Kernel_T *Ker, CKernel_Arg_T *ArgVal, KernelIteratorT ItSpace, int *IsInvar);

extern ArgExpr_T *ArgExprTop;

#include "Expr_Parse.tab.h"
#endif
