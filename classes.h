//
// Created by Silvitio on 05.11.2025.
//
#pragma once

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "classes.h"

using namespace std;

class StmtNode;
class SimpleStmtNode;
class CaseNode;
class CaseListNode;
class ParamDeclNode;
class ParamDeclListNode;
class ResultNode;
class SignatureNode;
class VarSpecNode;
class ConstSpecNode;

class AstNode {
protected:
    static unsigned int maxId;
    unsigned int id;

public:
    AstNode() {id = ++maxId;};
    unsigned int getId() const {return id;};
};

class ExprNode : public AstNode {
public:
    enum ExprType {
        NONE,
        ID,
        EXPR_IN_BRACKETS,
        INT_LITERAL,
        FLOAT_LITERAL,
        RUNE_LITERAL,
        STRING_LITERAL,
        BOOL_LITERAL,
        SUMMARY,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        EQUAL,
        NOT_EQUAL,
        LESS,
        GREATER,
        LESS_OR_EQUAL,
        GREATER_OR_EQUAL,
        AND,
        OR,
        NOT,
        UNARY_MINUS,
        ELEMENT_ACCESS,
        SLICE,
        FUNCTION_CALL
    };

    static ExprNode* createIdentifier(const string &value);
    static ExprNode* createIntLiteral(int value);
    static ExprNode* createFloatLiteral(float value);
    static ExprNode* createRuneLiteral(int value);
    static ExprNode* createStringLiteral(const string &value);
    static ExprNode* createBoolLiteral(bool value);
    static ExprNode* createSummary(ExprNode *left, ExprNode *right);
    static ExprNode* createSubtraction(ExprNode *left, ExprNode *right);
    static ExprNode* createMultiplication(ExprNode *left, ExprNode *right);
    static ExprNode* createDivision(ExprNode *left, ExprNode *right);
    static ExprNode* createEqual(ExprNode *left, ExprNode *right);
    static ExprNode* createNotEqual(ExprNode *left, ExprNode *right);
    static ExprNode* createLess(ExprNode *left, ExprNode *right);
    static ExprNode* createGreater(ExprNode *left, ExprNode *right);
    static ExprNode* createLessOrEqual(ExprNode *left, ExprNode *right);
    static ExprNode* createGreaterOrEqual(ExprNode *left, ExprNode *right);
    static ExprNode* createAnd(ExprNode *left, ExprNode *right);
    static ExprNode* createOr(ExprNode *left, ExprNode *right);
    static ExprNode* createNot(ExprNode *operand);
    static ExprNode* createUnaryMinus(ExprNode *operand);
    static ExprNode* createElementAccess(ExprNode *operand, ExprNode *index);
    static ExprNode* createSlice(ExprNode *operand, ExprNode *low, ExprNode *high, ExprNode *max);
    static ExprNode* createFunctionCall(ExprNode *operand, list<ExprNode*> *args);

    ExprType getType() const;
    string getIdentifier() const;
    int getIntLiteral() const;
    float getFloatLiteral() const;
    int getRuneLiteral() const;
    string getStringLiteral() const;
    bool getBoolLiteral() const;
    ExprNode* getLeft() const;
    ExprNode* getRight() const;
    ExprNode* getOperand() const;
    ExprNode* getIndex() const;
    list<ExprNode*>* getArgs() const;
    ExprNode* getLow() const;
    ExprNode* getHigh() const;
    ExprNode* getMax() const;

protected:
    ExprType type;
    string identifier;
    int intLiteral;
    float floatLiteral;
    int runeLiteral;
    string stringLiteral;
    bool boolLiteral;

    ExprNode *left;
    ExprNode *right;
    ExprNode *operand;
    ExprNode *index;
    list<ExprNode*> *args;
    ExprNode *sliceLow;
    ExprNode *sliceHigh;
    ExprNode *sliceMax;

    ExprNode();
};

class ExprListNode : public AstNode {
public:
    static ExprListNode* createExprList(ExprNode *expr);
    static ExprListNode* addExprToList(ExprListNode *list, ExprNode *expr);

    list<ExprNode*>* getExprList() const;

protected:
    ExprListNode(): AstNode() {exprs = nullptr;};
    list<ExprNode*> *exprs;

};

class StmtListNode : public AstNode {
public:
    static StmtListNode* createStmtList(StmtNode *stmt);
    static StmtListNode* addStmtToList(StmtListNode *list, StmtNode *stmt);

    list<StmtNode*>* getStmtList() const;

protected:
    StmtListNode(): AstNode() {stmts = nullptr;};
    list<StmtNode*> *stmts;
};

class StmtNode : public AstNode {
public:
    enum StmtType {
        NONE,
        DECLARATION,
        SIMPLE,
        RETURN,
        BREAK,
        CONTINUE,
        BLOCK,
        IF,
        SWITCH,
        FOR,
        FOR_PARAM,
        FOR_RANGE,
        EMPTY
    };

    static StmtNode* createSimple(SimpleStmtNode *simpleStmt);
    static StmtNode* createReturn(ExprListNode *exprList);
    static StmtNode* createBreak();
    static StmtNode* createContinue();
    static StmtNode* createBlock(StmtListNode *stmtList);
    static StmtNode* createIf(SimpleStmtNode *simpleStmt, ExprNode *condition, StmtNode *thenBranch, StmtNode *elseBranch);
    static StmtNode* createSwitch(SimpleStmtNode *simpleStmt, ExprNode *condition, CaseListNode *cases);
    static StmtNode* createFor(ExprNode *condition, StmtNode *body);
    static StmtNode* createFor(SimpleStmtNode *initStmt, ExprNode *condition, SimpleStmtNode *postStmt, StmtNode *body);
    static StmtNode* createFor(ExprListNode *exprList, ExprNode *expr, StmtNode *body);

protected:
    StmtType type;
    ExprListNode *exprList;
    StmtListNode *stmtList;
    SimpleStmtNode *simpleStmt;
    ExprNode *condition;
    StmtNode *thenBranch, *elseBranch;
    StmtNode *body;
    CaseListNode *caseList;
    SimpleStmtNode *initStmt;
    SimpleStmtNode *postStmt;

    StmtNode();
};

class CaseNode : public AstNode {
public:
    static CaseNode* createCase(ExprListNode *exprList, StmtListNode *stmtList);

    ExprListNode* getExprList() const;
    StmtListNode* getStmtList() const;

protected:
    ExprListNode *exprList;
    StmtListNode *stmtList;

    CaseNode();
};

class CaseListNode : public AstNode {
public:
    static CaseListNode* createCaseList(CaseNode *inCase);
    static CaseListNode* addCaseToList(CaseListNode *list, CaseNode *inCase);

    list<CaseNode*>* getCaseList() const;

protected:
    list<CaseNode*> *caseList;

    CaseListNode(): AstNode() {caseList = nullptr;};
};

class SimpleStmtNode : public AstNode {
public:
    enum SimpleStmtType {
        NONE,
        EXPR,
        INC,
        DEC,
        ASSIGN,
        SHORT_VAR_DECL
    };

    static SimpleStmtNode* createExpr(ExprNode *expr);
    static SimpleStmtNode* createInc(ExprNode *expr);
    static SimpleStmtNode* createDec(ExprNode *expr);
    static SimpleStmtNode* createAssign(ExprListNode *left, ExprListNode *right);
    static SimpleStmtNode* createShortVarDecl(ExprListNode *left, ExprListNode *right);

    SimpleStmtType getType() const;
    ExprNode* getExpr() const;
    ExprListNode* getLeft() const;
    ExprListNode* getRight() const;

protected:
    SimpleStmtType type;
    ExprNode *expr;
    ExprListNode *left;
    ExprListNode *right;

    SimpleStmtNode();
};

class IdListNode : public AstNode {
public:
    static IdListNode* createIdList(string *id);
    static IdListNode* addIdToList(IdListNode *list, string *id);

    list<string*>* getIdList() const;

protected:
    list<string*> *ids;

    IdListNode(): AstNode() {ids = nullptr;};
};

class TypeNode : public AstNode {
public:
    enum Kind {
        NONE,
        NAMED,
        ARRAY,
        SLICE,
        FUNC
    };

    static TypeNode* createNamedType(string *name);
    static TypeNode* createArrayType(ExprNode *len, TypeNode *elemType);
    static TypeNode* createFuncType(SignatureNode *signature);
    static TypeNode* createSliceType(TypeNode *elemType);

private:
    Kind kind;
    string *name;
    ExprNode *arrayLen;
    TypeNode *elemType;
    SignatureNode *signature;

    TypeNode();
};

class ParamDeclNode : public AstNode {
public:
    static ParamDeclNode* createParamDecl(IdListNode *ids, TypeNode* type);

protected:
    IdListNode *idList;
    TypeNode* type;

    ParamDeclNode();
};

class ParamDeclListNode : public AstNode {
public:
    static ParamDeclListNode* createParamDeclList(ParamDeclNode* param);
    static ParamDeclListNode* addParamDeclToList(ParamDeclListNode *list, ParamDeclNode *param);

    list<ParamDeclNode*>* getParamList() const;

protected:
    list<ParamDeclNode*> *paramList;

    ParamDeclListNode(): AstNode() {paramList = nullptr;};
};

class SignatureNode : public AstNode {
public:
    static SignatureNode* createSignature(ParamDeclListNode *paramList, ResultNode *result);

protected:
    ParamDeclListNode *paramList;
    ResultNode *result;

    SignatureNode();
};

class ResultNode : public AstNode {
public:
    static ResultNode* createResult(ParamDeclListNode *paramList);
    static ResultNode* createResult(TypeNode *type);

protected:
    ParamDeclListNode *paramList;
    TypeNode *type;

    ResultNode();
};

class VarSpecNode : public AstNode {
public:
    static VarSpecNode* createVarSpec(IdListNode *idList, TypeNode *type, ExprNode *exprList);

protected:
    IdListNode *idList;
    TypeNode *type;
    ExprNode *expr;

    VarSpecNode();
};

class VarSpecListNode : public AstNode {
public:
    static VarSpecListNode* createVarSpecList(VarSpecNode* var);
    static VarSpecListNode* addVarSpecToList(VarSpecListNode *list, VarSpecNode *elem);

    list<VarSpecNode*>* getList() const;

protected:
    list<VarSpecNode*> *varList;

    VarSpecListNode();
};

class ConstSpecNode : public AstNode {
public:
    static ConstSpecNode* createConstSpec(IdListNode *idList, TypeNode *type, ExprListNode *exprList);

protected:
    IdListNode *idList;
    TypeNode *type;
    ExprListNode *exprList;

    ConstSpecNode();
};

class ConstSpecListNode : public AstNode {
public:
    static ConstSpecListNode* createConstSpecList(ConstSpecNode* spec);
    static ConstSpecListNode* addConstSpecToList(ConstSpecListNode *list, ConstSpecNode *spec);

    list<ConstSpecNode*>* getList() const;

protected:
    list<ConstSpecNode*> *specList;

    ConstSpecListNode();
};

class DeclNode : public AstNode {
public:
    static DeclNode* createDecl(ConstSpecNode *constSpec);
    static DeclNode* createDecl(ConstSpecListNode *constSpecList);
    static DeclNode* createDecl(VarSpecNode *varSpec);
    static DeclNode* createDecl(VarSpecListNode *varSpecList);

protected:
    ConstSpecListNode *constSpecList;
    VarSpecListNode *varSpecList;

    DeclNode();
};

class FuncDeclNode : public AstNode {
public:
    static FuncDeclNode* createFuncDecl(string *id, SignatureNode *signature, StmtNode *body);

protected:
    string *id;
    SignatureNode *signature;
    StmtNode *body;

    FuncDeclNode();
};

class TopLevelDeclNode : public AstNode {
public:
    static TopLevelDeclNode* createTopLevelDecl(DeclNode *decl);
    static TopLevelDeclNode* createTopLevelDecl(FuncDeclNode *funcDecl);

protected:
    DeclNode *decl;
    FuncDeclNode *funcDecl;

    TopLevelDeclNode();
};

class TopLevelDeclListNode : public AstNode {
public:
    static TopLevelDeclListNode* createList(TopLevelDeclNode *elem);
    static TopLevelDeclListNode* addElemToList(TopLevelDeclListNode *elemList, TopLevelDeclNode *elem);

    list<TopLevelDeclNode*>* getList() const;

protected:
    list<TopLevelDeclNode*> *elemList;

    TopLevelDeclListNode();
};

class PackageClauseNode : public AstNode {
public:
    static PackageClauseNode* createNode(string *id);

protected:
    string *id;

    PackageClauseNode();
};

class ImportSpecNode : public AstNode {
public:
    enum ImportType {
        NONE,
        SIMPLE,
        POINT,
        NAMED
    };

    static ImportSpecNode* createSimple(string *import);
    static ImportSpecNode* createPoint(string *import);
    static ImportSpecNode* createNamed(string *alias, string *import);

protected:
    ImportType importType;
    string *import;
    string *alias;

    ImportSpecNode();
};
