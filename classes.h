//
// Created by Silvitio on 05.11.2025.
//
#pragma once

#include <iostream>
#include <string>
#include <list>

using namespace std;

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
        GREAT_OR_EQUAL,
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
    static ExprNode* createSummary(ExprNode* left, ExprNode* right);
    static ExprNode* createSubtraction(ExprNode* left, ExprNode* right);
    static ExprNode* createMultiplication(ExprNode* left, ExprNode* right);
    static ExprNode* createDivision(ExprNode* left, ExprNode* right);
    static ExprNode* createEqual(ExprNode* left, ExprNode* right);
    static ExprNode* createNotEqual(ExprNode* left, ExprNode* right);
    static ExprNode* createLess(ExprNode* left, ExprNode* right);
    static ExprNode* createGreater(ExprNode* left, ExprNode* right);
    static ExprNode* createLessOrEqual(ExprNode* left, ExprNode* right);
    static ExprNode* createGreaterOrEqual(ExprNode* left, ExprNode* right);
    static ExprNode* createAnd(ExprNode* left, ExprNode* right);
    static ExprNode* createOr(ExprNode* left, ExprNode* right);
    static ExprNode* createNot(ExprNode* operand);
    static ExprNode* createUnaryMinus(ExprNode* operand);
    static ExprNode* createElementAccess(ExprNode* operand, ExprNode* index);
    static ExprNode* createSlice(ExprNode* operand, ExprNode* low, ExprNode* high, ExprNode* max);
    static ExprNode* createFunctionCall(ExprNode* operand, list<ExprNode*> *args);

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
    list<ExprNode*>* args;
    ExprNode *sliceLow;
    ExprNode *sliceHigh;
    ExprNode *sliceMax;

    ExprNode(): AstNode() {
        type = NONE;
        identifier = "";
        intLiteral = 0;
        floatLiteral = 0;
        runeLiteral = 0;
        stringLiteral = "";
        boolLiteral = false;
        left = nullptr;
        right = nullptr;
        operand = nullptr;
        index = nullptr;
        args = nullptr;
        sliceLow = nullptr;
        sliceHigh = nullptr;
        sliceMax = nullptr;
    };
};

class ExprListNode : public AstNode {
public:
    static ExprListNode* createExprList(ExprNode *expr);
    static ExprListNode* addExprToList(ExprListNode *list, ExprNode *expr);

    list<ExprNode*>* getExprList() const;

protected:
    ExprListNode(): AstNode() {exprs = nullptr;};
    list<ExprNode*>* exprs;

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

    static StmtNode* createReturn(ExprListNode *exprList);
    static StmtNode* createBreak();
    static StmtNode* createContinue();

protected:
    StmtType type;
    ExprListNode *returnExprList;

    StmtNode(): AstNode() {
        type = NONE;
        returnExprList = nullptr;
    }
};
