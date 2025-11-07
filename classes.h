//
// Created by Silvitio on 05.11.2025.
//
#pragma once

#include <iostream>
#include <string>
#include <list>

using namespace std;

class StmtNode;
class SimpleStmtNode;
class CaseNode;

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
        EMPTY
    };

    static StmtNode* createSimple(SimpleStmtNode *simpleStmt);
    static StmtNode* createReturn(ExprListNode *exprList);
    static StmtNode* createBreak();
    static StmtNode* createContinue();
    static StmtNode* createBlock(StmtListNode *stmtList);
    static StmtNode* createIf(SimpleStmtNode *simpleStmt, ExprNode *condition, StmtNode *thenBranch, StmtNode *elseBranch);

protected:
    StmtType type;
    ExprListNode *returnExprList;
    StmtListNode *stmtList;
    SimpleStmtNode *simpleStmt;
    ExprNode *condition;
    StmtNode *thenBranch, *elseBranch;

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


