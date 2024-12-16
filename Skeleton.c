/* File generated by the BNF Converter (bnfc 2.9.5). */

/*** Visitor Traversal Skeleton. ***/
/* This traverses the abstract syntax tree.
   To use, copy Skeleton.h and Skeleton.c to
   new files. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Skeleton.h"


/* LABEL/GOTO:
Para fazer a semantica do label/Goto vamos criar uma tabela de símbolos
Acho legal usar lista dinâmica nesse caso, o processo vai ser:
1- Inicializar a tabela
2- Adiciona uma label na tabela, retorna 1 se sucesso, 0 se duplicada
3- Por final, vai verifica se uma label existe na tabela
Dps entrar nos cases
*/

#define MAX_LABELS 1000
#define MAX_SYMBOLS 1000
#define MAX_FUNCTIONS 1000
#define MAX_PARAMS 100

typedef struct {
    int labels[MAX_LABELS]; 
    int count;              
} LabelTable;

LabelTable labelTable;

void initLabelTable() {
    labelTable.count = 0;
}

int addLabel(int label) {
    int i = 0;
    for (i = 0; i < labelTable.count; i++) {
        if (labelTable.labels[i] == label) {
            return 0; 
        }
    }
    labelTable.labels[labelTable.count++] = label;
    return 1; 
}

int labelExists(int label) {
    int i = 0 ;
    for (i = 0; i < labelTable.count; i++) {
        if (labelTable.labels[i] == label) {
            return 1; 
        }
    }
    return 0; 
}

void printLabelTable() {
    printf("\nTabela de Labels:\n");
    int i = 0;
    for ( i = 0; i < labelTable.count; i++) {
        printf("Label %d\n", labelTable.labels[i]);
    }
    if (labelTable.count == 0) {
        printf("Nenhuma label foi definida.\n");
    }
}

/* TABELA DE SÍMBOLOS PARA VARIÁVEIS: */

typedef struct {
    Ident ident;
    Type type; 
    char *value; 
} SymbolEntry;

typedef struct {
    Ident idents[MAX_SYMBOLS]; 
    Type types[MAX_SYMBOLS];   
    char* values[MAX_SYMBOLS];
    int count;
} SymbolTable;

SymbolTable symbolTable;

void initSymbolTable() {
    symbolTable.count = 0;
}

int symbolExists(Ident ident) {
    int i;
    for (i = 0; i < symbolTable.count; i++) {
        if (strcmp(symbolTable.idents[i], ident) == 0) {
            return 1;
        }
    }
    return 0;
}

Type getSymbolType(Ident ident) {
    int i;
    for (i = 0; i < symbolTable.count; i++) {
        if (strcmp(symbolTable.idents[i], ident) == 0) {
            return symbolTable.types[i];
        }
    }
    return NULL; 
}

char* getSymbolValue(Ident ident) {
    int i;
    for (i = 0; i < symbolTable.count; i++) {
        if (strcmp(symbolTable.idents[i], ident) == 0) {
            return symbolTable.values[i];
        }
    }
    return NULL;
}

int addSymbol(Ident ident, Type type) {
    if (symbolExists(ident)) {
        return 0; 
    }
    symbolTable.idents[symbolTable.count] = ident;
    symbolTable.types[symbolTable.count] = type;
    symbolTable.values[symbolTable.count] = NULL;
    symbolTable.count++;
    return 1;
}

int setSymbolValue(Ident ident, const char* value) {
    int i;
    for (i = 0; i < symbolTable.count; i++) {
        if (strcmp(symbolTable.idents[i], ident) == 0) {
            if (symbolTable.values[i]) {
                free(symbolTable.values[i]);
            }
            symbolTable.values[i] = (char*)malloc(strlen(value)+1);
            strcpy(symbolTable.values[i], value);
            return 1;
        }
    }
    return 0;
}

/* TABELA DE FUNÇÕES:  
   Vamos armazenar nome da função*/

typedef struct {
    Ident fname;
} FunctionEntry;

typedef struct {
    FunctionEntry functions[MAX_FUNCTIONS];
    int count;
} FunctionTable;

FunctionTable functionTable;

void initFunctionTable() {
    functionTable.count = 0;
}

int functionExists(Ident fname) {
    int i;
    for (i = 0; i < functionTable.count; i++) {
        if (strcmp(functionTable.functions[i].fname, fname) == 0) {
            return 1;
        }
    }
    return 0;
}

FunctionEntry* getFunctionEntry(Ident fname) {
    int i;
    for (i = 0; i < functionTable.count; i++) {
        if (strcmp(functionTable.functions[i].fname, fname) == 0) {
            return &functionTable.functions[i];
        }
    }
    return NULL;
}

int addFunction(Ident fname) {
    if (functionExists(fname)) {
        return 0; 
    }
    functionTable.functions[functionTable.count].fname = fname;
    functionTable.count++;
    return 1;
}

void printFunctionTable() {
    printf("\nTabela de Funções:\n");
    int i;
    for (i = 0; i < functionTable.count; i++) {
        printf("Função '%s'\n", functionTable.functions[i].fname);
    }
    if (functionTable.count == 0) {
        printf("Nenhuma função definida.\n");
    }
}

static const char* getTypeString(Type t) {
    if (!t) return "tipo_desconhecido";
    switch(t->kind) {
        case is_Type_int: return "int";
        case is_Type_string: return "string";
        case is_Type_float: return "float";
        case is_Type_double: return "double";
        case is_Type_char: return "char";
        case is_Type_interface: return "interface";
        case is_Type1:
        case is_Type2:
        case is_TypeIdent:
          return "complex_type";
        default:
          return "tipo_desconhecido";
    }
}

/* Função auxiliar para determinar se um tipo é numérico */
static int isNumericType(const char* t) {
    return (strcmp(t, "int")==0 || strcmp(t, "float")==0 || strcmp(t, "double")==0);
}

/* Inferência do tipo de uma expressão */
static const char* inferExpType(Exp e);

static const char* unifyNumericTypes(const char* t1, const char* t2) {
    if (!isNumericType(t1) || !isNumericType(t2)) return "tipo_desconhecido";
    if (strcmp(t1,"int")==0 && strcmp(t2,"int")==0) return "int";
    if (strcmp(t1,"double")==0 || strcmp(t2,"double")==0) return "double";
    if (strcmp(t1,"float")==0 && strcmp(t2,"float")==0) return "float";
    if ((strcmp(t1,"int")==0 && strcmp(t2,"float")==0) || 
        (strcmp(t2,"int")==0 && strcmp(t1,"float")==0))
        return "float";
    return "double";
}

static const char* inferExpType(Exp e) {
    if (!e) return "tipo_desconhecido";
    switch (e->kind) {
        case is_EIdent: {
            Type varType = getSymbolType(e->u.eident_.ident_);
            if (varType) return getTypeString(varType);
            else return "tipo_desconhecido";
        }
        case is_EVar: {
            Type varType = getSymbolType(e->u.evar_.ident_);
            if (varType) return getTypeString(varType);
            else return "tipo_desconhecido";
        }
        case is_EInt:
            return "int";
        case is_EStr:
            return "string";
        case is_EDouble:
            return "double";
        case is_EChar:
            return "char";
        case is_EAdd:
        case is_ESub:
        case is_EMul:
        case is_EDiv:
        {
            const char* t1 = inferExpType((e->kind == is_EAdd)? e->u.eadd_.exp_1 :
                                          (e->kind == is_ESub)? e->u.esub_.exp_1 :
                                          (e->kind == is_EMul)? e->u.emul_.exp_1 :
                                          e->u.ediv_.exp_1);
            const char* t2 = inferExpType((e->kind == is_EAdd)? e->u.eadd_.exp_2 :
                                          (e->kind == is_ESub)? e->u.esub_.exp_2 :
                                          (e->kind == is_EMul)? e->u.emul_.exp_2 :
                                          e->u.ediv_.exp_2);
            if (strcmp(t1,"tipo_desconhecido")==0 || strcmp(t2,"tipo_desconhecido")==0) return "tipo_desconhecido";
            if (!isNumericType(t1) || !isNumericType(t2)) return "tipo_desconhecido";
            return unifyNumericTypes(t1, t2);
        }
        case is_ELt:
        case is_Equal:
        case is_EDiff:
        {
            const char* t1 = (e->kind == is_ELt)? inferExpType(e->u.elt_.exp_1) :
                              (e->kind == is_Equal)? inferExpType(e->u.equal_.exp_1) :
                              inferExpType(e->u.ediff_.exp_1);
            const char* t2 = (e->kind == is_ELt)? inferExpType(e->u.elt_.exp_2) :
                              (e->kind == is_Equal)? inferExpType(e->u.equal_.exp_2) :
                              inferExpType(e->u.ediff_.exp_2);
            if (strcmp(t1,"tipo_desconhecido")==0 || strcmp(t2,"tipo_desconhecido")==0) return "tipo_desconhecido";
            return "int";
        }
        default:
            return "tipo_desconhecido";
    }
}

static void checkListExpTypes(ListExp listexp, const char* expectedType) {
    ListExp le = listexp;
    while (le != NULL) {
        const char* elemType = inferExpType(le->exp_);
        if (strcmp(elemType, "tipo_desconhecido") != 0 && strcmp(elemType, expectedType) != 0) {
            printf("Aviso: Tipo do elemento '%s' não corresponde ao tipo esperado '%s' na atribuição de array/matriz.\n", elemType, expectedType);
        }
        le = le->listexp_;
    }
}

static void checkBinaryExp(Exp left, Exp right, const char* opName) {
    const char* leftType = inferExpType(left);
    const char* rightType = inferExpType(right);

    if (strcmp(opName, "EAdd")==0 || strcmp(opName, "ESub")==0 || strcmp(opName, "EMul")==0 || strcmp(opName, "EDiv")==0) {
        if (!isNumericType(leftType) || !isNumericType(rightType)) {
            printf("Erro: Operação '%s' requer operandos numéricos. Encontrados '%s' e '%s'.\n", opName, leftType, rightType);
            return;
        }
        const char* unified = unifyNumericTypes(leftType, rightType);
        if (strcmp(unified, "tipo_desconhecido")==0) {
            printf("Erro: Operação '%s' entre tipos incompatíveis '%s' e '%s'.\n", opName, leftType, rightType);
        }
    }
    else if (strcmp(opName, "ELt")==0 || strcmp(opName, "Equal")==0 || strcmp(opName, "EDiff")==0) {
        if (strcmp(leftType, "tipo_desconhecido")==0 || strcmp(rightType, "tipo_desconhecido")==0) {
            printf("Erro: Comparação '%s' entre tipos desconhecidos.\n", opName);
        }
    }
}

void printSymbolTable() {
    printf("\nTabela de Símbolos:\n");
    int i;
    for (i = 0; i < symbolTable.count; i++) {
        printf("|Variável: '%s' | Tipo: '%s' | Valor: '%s'|\n", symbolTable.idents[i], getTypeString(symbolTable.types[i]),
               symbolTable.values[i] ? symbolTable.values[i] : "não atribuído");
    }
    if (symbolTable.count == 0) {
        printf("Nenhuma variável definida.\n");
    }
}

void visitProgram(Program p)
{
  switch(p->kind)
  {
  case is_Prog:
    printf("Começando a análise semântica\n");

    initLabelTable();
    initSymbolTable();
    initFunctionTable();

    visitListDecl(p->u.prog_.listdecl_);
    visitListStm(p->u.prog_.liststm_);
    visitListFunction(p->u.prog_.listfunction_);

    /* Imprimindo as tabelas */
    printLabelTable();
    printSymbolTable();
    printFunctionTable();

    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Program!\n");
    exit(1);
  }
}

int inFunctionContext = 0;
void visitFunction(Function p)
{
  switch(p->kind)
  {
  case is_Fun:
    {
      Ident fname = p->u.fun_.ident_;
      visitIdent(fname);

      /* Adiciona a função na tabela de funções */
      if (!addFunction(fname)) {
          printf("Erro: Função '%s' já declarada.\n", fname);
      } else {
          printf("Função '%s' declarada.\n", fname);
      }

      visitListStm(p->u.fun_.liststm_1);
      visitListStm(p->u.fun_.liststm_2);

      inFunctionContext = 1;

      visitListStm(p->u.fun_.liststm_1);
      visitListStm(p->u.fun_.liststm_2);

      inFunctionContext = 0;
    }
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Function!\n");
    exit(1);
  }
}

void visitDecl(Decl p)
{
  switch(p->kind)
  {
  case is_Dec:
    {
      Type declType = p->u.dec_.type_;
      visitType(p->u.dec_.type_);
      ListIdent lid = p->u.dec_.listident_;
      while (lid != 0) {
          visitIdent(lid->ident_);
          if (!addSymbol(lid->ident_, declType)) {
              printf("Erro: Variável '%s' já declarada.\n", lid->ident_);
          } else {
              printf("Variável '%s' adicionada com sucesso com tipo '%s'.\n", lid->ident_, getTypeString(declType));
          }
          lid = lid->listident_;
      }
    }
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Decl!\n");
    exit(1);
  }
}

void visitListFunction(ListFunction listfunction)
{
  while(listfunction  != 0)
  {
    visitFunction(listfunction->function_);
    listfunction = listfunction->listfunction_;
  }
}

void visitListStm(ListStm liststm)
{
  while(liststm  != 0)
  {
    visitStm(liststm->stm_);
    liststm = liststm->liststm_;
  }
}

void visitListDecl(ListDecl listdecl)
{
  while(listdecl  != 0)
  {
    visitDecl(listdecl->decl_);
    listdecl = listdecl->listdecl_;
  }
}

void visitListIdent(ListIdent listident)
{
  while(listident  != 0)
  {
    visitIdent(listident->ident_);
    listident = listident->listident_;
  }
}

void visitStm(Stm p)
{
  switch(p->kind)
  {
  case is_SDecl:
    visitDecl(p->u.sdecl_.decl_);
    break;
  case is_SExp:
    visitExp(p->u.sexp_.exp_);
    break;
  case is_SBlock:
    visitListStm(p->u.sblock_.liststm_);
    break;
  case is_SWhile:
    printf("Visitando comando While.\n");

    visitExp(p->u.swhile_.exp_);

    if (p->u.swhile_.exp_->kind != is_ELt &&   
        p->u.swhile_.exp_->kind != is_Equal &&  
        p->u.swhile_.exp_->kind != is_EDiff )
    {
        printf("Erro: Condição do 'While' inválida.\n");
        break;
    }

    printf("Condição válida. Executando o bloco do While.\n");
    visitStm(p->u.swhile_.stm_);
    break;
  case is_SFor:
    printf("Verificando o FOR \n");

    printf("Verificando comando de inicialização do FOR\n");
    if (p->u.sfor_.exp_1->kind != is_EAss &&  
        p->u.sfor_.exp_1->kind != is_EAssSimpl) 
    {
        printf("Erro: Inicialização do 'for' inválida.\n");
        break;
    }
    else {
      printf("Inicialização do for válida\n");
    }
    visitExp(p->u.sfor_.exp_1);

    printf("Verificando comando de condição do FOR\n");
    if (p->u.sfor_.exp_2->kind != is_ELt &&   
        p->u.sfor_.exp_2->kind != is_Equal &&  
        p->u.sfor_.exp_2->kind != is_EDiff )
    {
        printf("Erro: Condição do 'for' inválida.\n");
        break;
    }
    else {
      printf("Condição do for válida\n");
    }
    visitExp(p->u.sfor_.exp_2);

    printf("Verificando comando de atualização do FOR\n");
    if (p->u.sfor_.exp_3->kind != is_EIncr &&
        p->u.sfor_.exp_3->kind != is_EAssSimpl &&
        p->u.sfor_.exp_3->kind != is_EIncrWithoutemicolon) 
    {
        printf("Erro: Atualização do 'for' inválida.\n");
        break;
    }
    else {
      printf("Atualização do for válida\n");
    }
    visitExp(p->u.sfor_.exp_3);

    printf("Executando o bloco do FOR\n");
    visitStm(p->u.sfor_.stm_);
    break;
  case is_SIf:
    printf("Visitando comando IF.\n");

    visitExp(p->u.sif_.exp_);

    if (p->u.sif_.exp_->kind != is_ELt &&   
        p->u.sif_.exp_->kind != is_Equal &&  
        p->u.sif_.exp_->kind != is_EDiff )
    {
        printf("Erro: Condição do 'if' inválida.\n");
        break;
    }

    printf("Condição válida. Executando o bloco do IF.\n");
    visitListStm(p->u.sif_.liststm_);
    break;
  case is_SIfelse:
    printf("Visitando comando IF-ELSE.\n");

    if (p->u.sifelse_.exp_->kind != is_ELt &&   
        p->u.sifelse_.exp_->kind != is_Equal &&  
        p->u.sifelse_.exp_->kind != is_EDiff )
    {
      printf("Erro: Condição do 'if' inválida.\n");
      break;
    }
    printf("Condição válida. Executando o bloco do IF.\n");
    visitListStm(p->u.sifelse_.liststm_1);
    printf("Executando o bloco do ELSE.\n");
    visitListStm(p->u.sifelse_.liststm_2);
    break;
  case is_SReturn:
    printf("Visitando comando RETURN.\n");

    if (!inFunctionContext) {
        printf("Erro: Comando 'return' usado fora de uma função.\n");
        break;
    }

    visitExp(p->u.sreturn_.exp_);

    {
    const char* returnType = inferExpType(p->u.sreturn_.exp_);
    if (strcmp(returnType, "tipo_desconhecido") == 0) {
        printf("Aviso: Tipo da expressão retornada é desconhecido.\n");
    } else {
        printf("Expressão retornada com tipo: '%s'.\n", returnType);
    }
    }
    break;
  case is_SLabel:
    {
      printf("Visitando comando LABEL.\n");
      int label = p->u.slabel_.integer_ ;

      if (!addLabel(label)) {
          printf("Erro: Label %d já foi definida.\n", label);
      } else {
          printf("Label %d adicionada com sucesso.\n", label);
      }
      visitInteger(p->u.slabel_.integer_);
    }
    break;
  case is_SGoto:
    {
      printf("Visitando comando GOTO.\n");

      int gotoLabel = p->u.sgoto_.integer_;

      if (!labelExists(gotoLabel)) {
          printf("Erro: Label %d não definida antes do GOTO.\n", gotoLabel);
      } else {
          printf("GOTO para label %d validado com sucesso.\n", gotoLabel);
      }
      visitInteger(p->u.sgoto_.integer_);
    }
    break;
  case is_SLog:
    printf("Visitando comando LOG.\n");

    printf("Validando o argumento do LOG...\n");
    switch (p->u.slog_.exp_->kind)
    {
    case is_EIdent:
        printf("LOG: Variável detectada.\n");
        if (!symbolExists(p->u.slog_.exp_->u.eident_.ident_)) {
            printf("Erro: Variável '%s' não foi declarada antes do uso.\n", p->u.slog_.exp_->u.eident_.ident_);
        } else {
            printf("LOG válido.\n");
        }
        visitIdent(p->u.slog_.exp_->u.eident_.ident_);
        break;

    case is_EStr:
        printf("LOG: String detectada.\n");
        visitString(p->u.slog_.exp_->u.estr_.string_);
        printf("LOG válido.\n");
        break;

    case is_Call:
        printf("LOG: Função detectada.\n");
        visitIdent(p->u.slog_.exp_->u.call_.ident_);
        visitListExp(p->u.slog_.exp_->u.call_.listexp_);
        printf("LOG válido.\n");
        break;

    default:
        printf("Erro: Argumento inválido para o comando LOG.\n");
        break;
    }
    break;
  case is_SInterface:
    visitIdent(p->u.sinterface_.ident_);
    visitListExp(p->u.sinterface_.listexp_);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Stm!\n");
    exit(1);
  }
}

/* Função auxiliar para criar um string representando a expressão (para armazenar valor).
   Aqui, faremos de forma simples. Se for literal (int/str/double/char), convertemos para string.
   Se for operador, concatenamos representações simples.
   Isso é apenas ilustrativo. */
static char* expToString(Exp p) {
    if (!p) return strdup("desconhecido");
    switch(p->kind) {
        case is_EInt: {
            char buffer[50];
            sprintf(buffer, "%d", p->u.eint_.integer_);
            return strdup(buffer);
        }
        case is_EDouble: {
            char buffer[50];
            sprintf(buffer, "%f", p->u.edouble_.double_);
            return strdup(buffer);
        }
        case is_EStr:
            return strdup(p->u.estr_.string_);
        case is_EChar: {
            char buffer[10];
            sprintf(buffer, "%c", p->u.echar_.char_);
            return strdup(buffer);
        }
        case is_EIdent:
            return strdup(p->u.eident_.ident_);
        case is_EVar:
            return strdup(p->u.evar_.ident_);
        case is_EAdd: {
            char* left = expToString(p->u.eadd_.exp_1);
            char* right = expToString(p->u.eadd_.exp_2);
            char* res = malloc(strlen(left)+strlen(right)+2);
            sprintf(res, "%s+%s", left, right);
            free(left); free(right);
            return res;
        }
        case is_ESub: {
            char* left = expToString(p->u.esub_.exp_1);
            char* right = expToString(p->u.esub_.exp_2);
            char* res = malloc(strlen(left)+strlen(right)+2);
            sprintf(res, "%s-%s", left, right);
            free(left);free(right);
            return res;
        }
        case is_EMul: {
            char* left = expToString(p->u.emul_.exp_1);
            char* right = expToString(p->u.emul_.exp_2);
            char* res = malloc(strlen(left)+strlen(right)+2);
            sprintf(res, "%s*%s", left, right);
            free(left);free(right);
            return res;
        }
        case is_EAssSimpl: {
            char* left = strdup(p->u.easssimpl_.ident_);
            char* right = expToString(p->u.easssimpl_.exp_);
            char* res = malloc(strlen(left) + strlen(right) + 2);
            sprintf(res, "%s=%s", left, right);
            free(left);
            free(right);
            return res;
        }

        default:
            return strdup("expressao_complexa");
    }
}


void visitExp(Exp p)
{
  switch(p->kind)
  {
  case is_EAss:
    {
      Ident varName = p->u.eass_.ident_;
      Type varType = p->u.eass_.type_;
      const char* rightType = inferExpType(p->u.eass_.exp_);
      visitIdent(varName);
      visitType(varType);
      visitExp(p->u.eass_.exp_);

      if (symbolExists(varName)) {
          printf("Erro: Variável '%s' já declarada anteriormente.\n", varName);
      } else {
          addSymbol(varName, varType);
          printf("Variável '%s' declarada com tipo '%s'.\n", varName, getTypeString(varType));
      }

      if (strcmp(getTypeString(varType), rightType) != 0 && strcmp(rightType, "tipo_desconhecido") != 0) {
          printf("Aviso: Tipo da expressão '%s' não coincide com o tipo da variável '%s'.\n", rightType, getTypeString(varType));
      }

      char* valStr = expToString(p->u.eass_.exp_);
      setSymbolValue(varName, valStr);
      free(valStr);
    }
    break;
  case is_EIdent:
    {
      Ident varName = p->u.eident_.ident_;
      visitIdent(varName);
      if (!symbolExists(varName)) {
          printf("Erro: Variável '%s' não foi declarada antes do uso.\n", varName);
      }
    }
    break;
  case is_EAssSimpl:
    {
      Ident varName = p->u.easssimpl_.ident_; 
      const char* rightType = inferExpType(p->u.easssimpl_.exp_); 

      printf("Visitando atribuição simples: '%s'.\n", varName);

      visitIdent(varName);
      visitExp(p->u.easssimpl_.exp_);

      if (!symbolExists(varName)) {
          printf("Erro: Variável '%s' não foi declarada antes da atribuição.\n", varName);
      } else {
          Type declType = getSymbolType(varName);

          if (declType) {
              const char* leftType = getTypeString(declType); 
              printf("Variável '%s' tem tipo '%s'  \n", varName, leftType);

              if (strcmp(leftType, rightType) != 0 && strcmp(rightType, "tipo_desconhecido") != 0) {
                  printf("Aviso: Tipo da expressão '%s' não coincide com o tipo da variável '%s'\n", rightType, leftType);
              }
          } else {
              printf("Erro: Não foi possível determinar o tipo da variável '%s'\n", varName);
          }
      }

      char* valStr = expToString(p->u.easssimpl_.exp_);
      setSymbolValue(varName, valStr);
      free(valStr);
    }
    break;
  case is_EConst:
    visitIdent(p->u.econst_.ident_);
    visitType(p->u.econst_.type_);
    visitExp(p->u.econst_.exp_);
    break;
  case is_EPontMem:
    visitTypont(p->u.epontmem_.typont_);
    visitIdent(p->u.epontmem_.ident_);
    visitExp(p->u.epontmem_.exp_);
    break;
  case is_EPont:
    visitTypont(p->u.epont_.typont_);
    visitIdent(p->u.epont_.ident_);
    visitExp(p->u.epont_.exp_);
    break;
  case is_EAssArray:
    {
      Ident varName = p->u.eassarray_.ident_;
      Type varType = p->u.eassarray_.type_;
      visitIdent(varName);
      visitType(varType);
      visitListExp(p->u.eassarray_.listexp_);

      if (symbolExists(varName)) {
          printf("Erro: Variável '%s' já declarada anteriormente\n", varName);
      } else {
          if (!addSymbol(varName, varType)) {
              printf("Erro ao adicionar variável '%s'\n", varName);
          } else {
              printf("Variável '%s' declarada com tipo '%s' (array)\n", varName, getTypeString(varType));
          }
      }
      checkListExpTypes(p->u.eassarray_.listexp_, getTypeString(varType));
      setSymbolValue(varName, "[array]");
    }
    break;
  case is_EAssArraySim:
    {
      Ident varName = p->u.eassarraysim_.ident_;
      visitIdent(varName);
      visitExp(p->u.eassarraysim_.exp_1);
      visitExp(p->u.eassarraysim_.exp_2);

      if (!symbolExists(varName)) {
          printf("Erro: Variável (array) '%s' não foi declarada antes do uso\n", varName);
      } else {
          Type declType = getSymbolType(varName);
          if (declType) {
              const char* leftType = getTypeString(declType);
              const char* rightType = inferExpType(p->u.eassarraysim_.exp_2);
              if (strcmp(leftType, rightType) != 0 && strcmp(rightType, "tipo_desconhecido") != 0) {
                  printf("Aviso: Tipo do elemento atribuído '%s' não coincide com o tipo da variável '%s' (array)\n", rightType, leftType);
              }
          }
      }

      setSymbolValue(varName, "[array_modificado]");
    }
    break;
  case is_EAPositionInArray:
    visitIdent(p->u.eapositioninarray_.ident_1);
    visitInteger(p->u.eapositioninarray_.integer_1);
    visitIdent(p->u.eapositioninarray_.ident_2);
    visitInteger(p->u.eapositioninarray_.integer_2);
    break;
  case is_EAnotherArrayIndex:
    visitIdent(p->u.eanotherarrayindex_.ident_1);
    visitExp(p->u.eanotherarrayindex_.exp_1);
    visitIdent(p->u.eanotherarrayindex_.ident_2);
    visitExp(p->u.eanotherarrayindex_.exp_2);
    break;
  case is_EAssMatrix:
    {
      Ident varName = p->u.eassmatrix_.ident_;
      Type varType = p->u.eassmatrix_.type_;
      visitIdent(varName);
      visitType(varType);
      visitListExp(p->u.eassmatrix_.listexp_);

      if (symbolExists(varName)) {
          printf("Erro: Variável '%s' (matriz) já declarada anteriormente\n", varName);
      } else {
          if (!addSymbol(varName, varType)) {
              printf("Erro ao adicionar variável '%s'\n", varName);
          } else {
              printf("Variável '%s' declarada com tipo '%s' (matriz)\n", varName, getTypeString(varType));
          }
      }
      checkListExpTypes(p->u.eassmatrix_.listexp_, getTypeString(varType));
      setSymbolValue(varName, "[matriz]");
    }
    break;
  case is_EAssMatrixOp:
    {
      Ident varName = p->u.eassmatrixop_.ident_;
      Type varType = p->u.eassmatrixop_.type_;
      visitIdent(varName);
      visitType(varType);
      visitExp(p->u.eassmatrixop_.exp_);

      if (symbolExists(varName)) {
          printf("Erro: Variável '%s' (matriz) já declarada anteriormente\n", varName);
      } else {
          if (!addSymbol(varName, varType)) {
              printf("Erro ao adicionar variável '%s'.\n", varName);
          } else {
              printf("Variável '%s' declarada com tipo '%s' (matriz-op)\n", varName, getTypeString(varType));
          }
      }

      const char* rightType = inferExpType(p->u.eassmatrixop_.exp_);
      if (strcmp(getTypeString(varType), rightType) != 0 && strcmp(rightType, "tipo_desconhecido") != 0) {
          printf("Aviso: Tipo da expressão '%s' não coincide com o tipo da variável '%s'\n", rightType, getTypeString(varType));
      }
      setSymbolValue(varName, "[matriz_op]");
    }
    break;
  case is_EAssInterface:
    {
      Ident varName = p->u.eassinterface_.ident_;
      Type varType = p->u.eassinterface_.type_;
      visitIdent(varName);
      visitType(varType);
      visitListExp(p->u.eassinterface_.listexp_);

      if (symbolExists(varName)) {
          printf("Erro: Variável '%s' (interface) já declarada anteriormente\n", varName);
      } else {
          if (!addSymbol(varName, varType)) {
              printf("Erro ao adicionar variável '%s'\n", varName);
          } else {
              printf("Variável '%s' declarada com tipo '%s' (interface)\n", varName, getTypeString(varType));
          }
      }

      checkListExpTypes(p->u.eassinterface_.listexp_, getTypeString(varType));
      setSymbolValue(varName, "[interface]");
    }
    break;
  case is_EkeyDecl:
    {
      Ident varName = p->u.ekeydecl_.ident_;
      Type varType = p->u.ekeydecl_.type_;
      visitIdent(varName);
      visitType(varType);

      if (!addSymbol(varName, varType)) {
          printf("Erro: Variável '%s' já declarada\n", varName);
      } else {
          printf("Variável '%s' declarada com tipo '%s'\n", varName, getTypeString(varType));
      }
      setSymbolValue(varName, "não atribuído");
    }
    break;
  case is_EkeyDeclObj:
    visitIdent(p->u.ekeydeclobj_.ident_);
    visitExp(p->u.ekeydeclobj_.exp_);
    break;
  case is_EIncr:
    visitExp(p->u.eincr_.exp_);
    break;
  case is_EDecr:
    visitExp(p->u.edecr_.exp_);
    break;
  case is_EIncrWithoutemicolon:
    {
      Ident varName = p->u.eincrwithoutemicolon_.ident_;
      visitIdent(varName);
      if (!symbolExists(varName)) {
          printf("Erro: Variável '%s' não foi declarada antes do uso\n", varName);
      }
    }
    break;
  case is_EDecrWithoutemicolon:
    {
      Ident varName = p->u.edecrwithoutemicolon_.ident_;
      visitIdent(varName);
      if (!symbolExists(varName)) {
          printf("Erro: Variável '%s' não foi declarada antes do uso\n", varName);
      }
    }
    break;
  case is_ExpList:
    visitListExp(p->u.explist_.listexp_);
    break;
  case is_EMinor:
    visitIdent(p->u.eminor_.ident_);
    visitExp(p->u.eminor_.exp_1);
    visitExp(p->u.eminor_.exp_2);
    break;
  case is_EVequal:
    visitIdent(p->u.evequal_.ident_);
    visitExp(p->u.evequal_.exp_1);
    visitExp(p->u.evequal_.exp_2);
    break;
  case is_EVatri:
    visitIdent(p->u.evatri_.ident_1);
    visitIdent(p->u.evatri_.ident_2);
    visitExp(p->u.evatri_.exp_);
    break;
  case is_ELt:
    checkBinaryExp(p->u.elt_.exp_1, p->u.elt_.exp_2, "ELt");
    visitExp(p->u.elt_.exp_1);
    visitExp(p->u.elt_.exp_2);
    break;
  case is_Equal:
    checkBinaryExp(p->u.equal_.exp_1, p->u.equal_.exp_2, "Equal");
    visitExp(p->u.equal_.exp_1);
    visitExp(p->u.equal_.exp_2);
    break;
  case is_EDiff:
    checkBinaryExp(p->u.ediff_.exp_1, p->u.ediff_.exp_2, "EDiff");
    visitExp(p->u.ediff_.exp_1);
    visitExp(p->u.ediff_.exp_2);
    break;
  case is_EAdd:
    checkBinaryExp(p->u.eadd_.exp_1, p->u.eadd_.exp_2, "EAdd");
    visitExp(p->u.eadd_.exp_1);
    visitExp(p->u.eadd_.exp_2);
    break;
  case is_ESub:
    checkBinaryExp(p->u.esub_.exp_1, p->u.esub_.exp_2, "ESub");
    visitExp(p->u.esub_.exp_1);
    visitExp(p->u.esub_.exp_2);
    break;
  case is_EMul:
    checkBinaryExp(p->u.emul_.exp_1, p->u.emul_.exp_2, "EMul");
    visitExp(p->u.emul_.exp_1);
    visitExp(p->u.emul_.exp_2);
    break;
  case is_EDiv:
    checkBinaryExp(p->u.ediv_.exp_1, p->u.ediv_.exp_2, "EDiv");
    visitExp(p->u.ediv_.exp_1);
    visitExp(p->u.ediv_.exp_2);
    break;
  case is_Call:
    {
      Ident fname = p->u.call_.ident_;
      visitIdent(fname);
      visitListExp(p->u.call_.listexp_);

      /* Verifica se a função existe */
      if (!functionExists(fname)) {
          printf("Erro: Função '%s' chamada antes da declaração\n", fname);
      }
    }
    break;
  case is_EVar:
    {
      Ident varName = p->u.evar_.ident_;
      visitIdent(varName);
      if (!symbolExists(varName)) {
          printf("Erro: Variável '%s' não foi declarada antes do uso\n", varName);
      }
    }
    break;
  case is_EStr:
    visitString(p->u.estr_.string_);
    break;
  case is_EInt:
    printf("Expressão inteira detectada: ");
    visitInteger(p->u.eint_.integer_);
    visitInteger(p->u.eint_.integer_);
    break;
  case is_EChar:
    visitChar(p->u.echar_.char_);
    break;
  case is_EDouble:
    visitDouble(p->u.edouble_.double_);
    break;
  case is_EVet:
    visitIdent(p->u.evet_.ident_1);
    visitIdent(p->u.evet_.ident_2);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Exp!\n");
    exit(1);
  }
}

void visitListExp(ListExp listexp)
{
  while(listexp  != 0)
  {
    visitExp(listexp->exp_);
    listexp = listexp->listexp_;
  }
}

void visitType(Type p)
{
  switch(p->kind)
  {
  case is_Type1:
    visitType(p->u.type1_.type_);
    visitInteger(p->u.type1_.integer_);
    break;
  case is_Type_int:
    break;
  case is_Type2:
    visitString(p->u.type2_.string_);
    break;
  case is_Type_string:
    break;
  case is_Type_float:
    break;
  case is_Type_double:
    break;
  case is_Type_char:
    break;
  case is_Type_interface:
    break;
  case is_TypeIdent:
    visitIdent(p->u.typeident_.ident_);
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Type!\n");
    exit(1);
  }
}

void visitTypont(Typont p)
{
  switch(p->kind)
  {
  case is_TPonterio:
    break;

  default:
    fprintf(stderr, "Error: bad kind field when printing Typont!\n");
    exit(1);
  }
}

void visitIdent(Ident i)
{
  /* Code for Ident Goes Here */
}

void visitInteger(Integer i)
{
  /* Code for Integer Goes Here */
}

void visitDouble(Double d)
{
  /* Code for Double Goes Here */
}

void visitChar(Char c)
{
  /* Code for Char Goes Here */
}

void visitString(String s)
{
  /* Code for String Goes Here */
}
