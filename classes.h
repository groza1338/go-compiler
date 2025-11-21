//
// Created by Silvitio on 05.11.2025.
//
#pragma once

#include <iostream>
#include <string>
#include <list>

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
class DeclNode;
class TypeNameNode;
class ExprListNode;
class ValueNode;

class AstNode {
protected:
    static unsigned int maxId;
    unsigned int id;

    void appendDotNode(string &res) const;
    void appendDotEdge(string &res, const AstNode *child, const string &edgeLabel) const;

public:
    AstNode() {id = ++maxId;};
    virtual ~AstNode() = default;

    unsigned int getId() const {return id;};

    virtual string getDotLabel() const = 0;
    virtual string toDot() const = 0;
};

class ExprNode : public AstNode {
public:
    enum ExprType {
        NONE,
        ID,
        EXPR_IN_BRACKETS,
        LIT_VAL,
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

    static ExprNode* createIdentifier(ValueNode *value);
    static ExprNode* createLiteralVal(ValueNode *value);
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
    static ExprNode* createFunctionCall(ExprNode *operand, ExprListNode *args);

    ExprType getType() const;
    ValueNode* getIdentifier() const;
    ValueNode* getLiteral() const;
    ExprNode* getLeft() const;
    ExprNode* getRight() const;
    ExprNode* getOperand() const;
    ExprNode* getIndex() const;
    ExprListNode* getArgs() const;
    ExprNode* getLow() const;
    ExprNode* getHigh() const;
    ExprNode* getMax() const;

    string getDotLabel() const override;
    string toDot() const override;

protected:
    ExprType type;
    ValueNode *identifier;
    ValueNode *value;

    ExprNode *left;
    ExprNode *right;
    ExprNode *operand;
    ExprNode *index;
    ExprListNode *args;
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

    string getDotLabel() const override;
    string toDot() const override;

protected:
    ExprListNode(): AstNode() {exprs = nullptr;}
    list<ExprNode*> *exprs;
};

class StmtListNode : public AstNode {
public:
    static StmtListNode* createStmtList(StmtNode *stmt);
    static StmtListNode* addStmtToList(StmtListNode *list, StmtNode *stmt);

    list<StmtNode*>* getStmtList() const;

    string getDotLabel() const override;
    string toDot() const override;

protected:
    StmtListNode(): AstNode() {stmts = nullptr;}
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

    static StmtNode* createDecl(DeclNode *decl);
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

    string getDotLabel() const override;
    string toDot() const override;

protected:
    StmtType type;
    DeclNode *decl;
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

    string getDotLabel() const override;
    string toDot() const override;

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

    string getDotLabel() const override;
    string toDot() const override;

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

    string getDotLabel() const override;
    string toDot() const override;

protected:
    SimpleStmtType type;
    ExprNode *expr;
    ExprListNode *left;
    ExprListNode *right;

    SimpleStmtNode();
};

class IdListNode : public AstNode {
public:
    static IdListNode* createIdList(ValueNode *id);
    static IdListNode* addIdToList(IdListNode *list, ValueNode *id);

    list<ValueNode*>* getIdList() const;

    string getDotLabel() const override;
    string toDot() const override;

protected:
    list<ValueNode*> *ids;

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

    static TypeNode* createNamedType(TypeNameNode *name);
    static TypeNode* createArrayType(ExprNode *len, TypeNode *elemType);
    static TypeNode* createFuncType(SignatureNode *signature);
    static TypeNode* createSliceType(TypeNode *elemType);

    string getDotLabel() const override;
    string toDot() const override;

private:
    Kind kind;
    TypeNameNode *name;
    ExprNode *arrayLen;
    TypeNode *elemType;
    SignatureNode *signature;

    TypeNode();
};

class ParamDeclNode : public AstNode {
public:
    static ParamDeclNode* createParamDecl(IdListNode *ids, TypeNode* type);

    string getDotLabel() const override;
    string toDot() const override;

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

    string getDotLabel() const override;
    string toDot() const override;

protected:
    list<ParamDeclNode*> *paramList;

    ParamDeclListNode(): AstNode() {paramList = nullptr;};
};

class SignatureNode : public AstNode {
public:
    static SignatureNode* createSignature(ParamDeclListNode *paramList, ResultNode *result);

    string getDotLabel() const override;
    string toDot() const override;

protected:
    ParamDeclListNode *paramList;
    ResultNode *result;

    SignatureNode();
};

class ResultNode : public AstNode {
public:
    static ResultNode* createResult(ParamDeclListNode *paramList);
    static ResultNode* createResult(TypeNode *type);

    string getDotLabel() const override;
    string toDot() const override;

protected:
    ParamDeclListNode *paramList;
    TypeNode *type;

    ResultNode();
};

class VarSpecNode : public AstNode {
public:
    static VarSpecNode* createVarSpec(IdListNode *idList, TypeNode *type, ExprListNode *exprList);

    string getDotLabel() const override;
    string toDot() const override;

protected:
    IdListNode *idList;
    TypeNode *type;
    ExprListNode *exprList;

    VarSpecNode();
};

class VarSpecListNode : public AstNode {
public:
    static VarSpecListNode* createVarSpecList(VarSpecNode* var);
    static VarSpecListNode* addVarSpecToList(VarSpecListNode *list, VarSpecNode *elem);

    list<VarSpecNode*>* getList() const;

    string getDotLabel() const override;
    string toDot() const override;

protected:
    list<VarSpecNode*> *varList;

    VarSpecListNode();
};

class ConstSpecNode : public AstNode {
public:
    static ConstSpecNode* createConstSpec(IdListNode *idList, TypeNode *type, ExprListNode *exprList);

    string getDotLabel() const override;
    string toDot() const override;

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

    string getDotLabel() const override;
    string toDot() const override;

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

    string getDotLabel() const override;
    string toDot() const override;

protected:
    ConstSpecListNode *constSpecList;
    VarSpecListNode *varSpecList;

    DeclNode();
};

class FuncDeclNode : public AstNode {
public:
    static FuncDeclNode* createFuncDecl(ValueNode *id, SignatureNode *signature, StmtNode *body);

    string getDotLabel() const override;
    string toDot() const override;

protected:
    ValueNode *id;
    SignatureNode *signature;
    StmtNode *body;

    FuncDeclNode();
};

class TopLevelDeclNode : public AstNode {
public:
    static TopLevelDeclNode* createTopLevelDecl(DeclNode *decl);
    static TopLevelDeclNode* createTopLevelDecl(FuncDeclNode *funcDecl);

    string getDotLabel() const override;
    string toDot() const override;

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
    static PackageClauseNode* createNode(ValueNode *id);

protected:
    ValueNode *id;

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

    static ImportSpecNode* createSimple(ValueNode *import);
    static ImportSpecNode* createPoint(ValueNode *import);
    static ImportSpecNode* createNamed(ValueNode *alias, ValueNode *import);

protected:
    ImportType importType;
    ValueNode *import;
    ValueNode *alias;

    ImportSpecNode();
};

class ImportSpecListNode : public AstNode {
public:
    static ImportSpecListNode* createList(ImportSpecNode *elem);
    static ImportSpecListNode* addElemToList(ImportSpecListNode *elemList, ImportSpecNode *elem);

    list<ImportSpecNode*>* getList() const;

protected:
    list<ImportSpecNode*> *elemList;

    ImportSpecListNode();
};

class ImportDeclNode : public AstNode {
public:
    static ImportDeclNode* createNode(ImportSpecNode *import);
    static ImportDeclNode* createNode(ImportSpecListNode *importList);

protected:
    ImportSpecListNode *importList;

    ImportDeclNode();
};

class ImportDeclListNode : public AstNode {
public:
    static ImportDeclListNode* createList(ImportDeclNode *elem);
    static ImportDeclListNode* addElemToList(ImportDeclListNode *elemList, ImportDeclNode *elem);

    list<ImportDeclNode*>* getList() const;

protected:
    list<ImportDeclNode*> *elemList;

    ImportDeclListNode();
};

class ProgramNode : public AstNode {
public:
    static ProgramNode* createNode(PackageClauseNode *packageClause, ImportDeclListNode *importDeclList, TopLevelDeclListNode *topLevelDeclList);

protected:
    PackageClauseNode *packageClause;
    ImportDeclListNode *importDeclList;
    TopLevelDeclListNode *topLevelDeclList;

    ProgramNode();
};

class TypeNameNode : public AstNode {
public:
    enum PredefinedTypes {
        NONE,
        INT_64,
        FLOAT_64,
        BOOL,
        STRING,
        RUNE
    };

    static TypeNameNode* createTypeInt();
    static TypeNameNode* createTypeFloat();
    static TypeNameNode* createTypeBool();
    static TypeNameNode* createTypeString();
    static TypeNameNode* createTypeRune();

protected:
    PredefinedTypes type;

    TypeNameNode();
};

class ValueNode : public AstNode {
public:
    enum ValueType {
        NONE,
        LIT_INT,
        LIT_FLOAT,
        LIT_BOOL,
        LIT_STRING,
        LIT_RUNE,
    };

    static ValueNode* createInt(int value);
    static ValueNode* createFloat(float value);
    static ValueNode* createBool(bool value);
    static ValueNode* createString(string *value);
    static ValueNode* createRune(int value);

    ValueType getValueType() const;
    int getInt() const;
    float getFloat() const;
    bool getBool() const;
    string* getString() const;
    int getRune() const;

    string getDotLabel() const override;
    string toDot() const override;

protected:
    ValueType valueType;
    int intValue;
    float floatValue;
    bool boolValue;
    string *stringValue;

    ValueNode();
};