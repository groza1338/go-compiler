//
// Created by Silvitio on 05.11.2025.
//

#include "classes.h"

unsigned int AstNode::maxId = 0;

ExprNode* ExprNode::createIdentifier(const string &value) {
    ExprNode *node = new ExprNode();
    node->type = ID;
    node->identifier = value;
    return node;
}

ExprNode* ExprNode::createIntLiteral(int value) {
    ExprNode *node = new ExprNode();
    node->type = INT_LITERAL;
    node->intLiteral = value;
    return node;
}

ExprNode* ExprNode::createFloatLiteral(float value) {
    ExprNode *node = new ExprNode();
    node->type = FLOAT_LITERAL;
    node->floatLiteral = value;
    return node;
}

ExprNode* ExprNode::createRuneLiteral(int value) {
    ExprNode *node = new ExprNode();
    node->type = RUNE_LITERAL;
    node->runeLiteral = value;
    return node;
}

ExprNode* ExprNode::createStringLiteral(const string &value) {
    ExprNode *node = new ExprNode();
    node->type = STRING_LITERAL;
    node->stringLiteral = value;
    return node;
}

ExprNode* ExprNode::createBoolLiteral(bool value) {
    ExprNode *node = new ExprNode();
    node->type = BOOL_LITERAL;
    node->boolLiteral = value;
    return node;
}

ExprNode* ExprNode::createSummary(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = SUMMARY;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createSubtraction(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = SUBTRACTION;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createMultiplication(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = MULTIPLICATION;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createDivision(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = DIVISION;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createEqual(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = EQUAL;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createNotEqual(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = NOT_EQUAL;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createLess(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = LESS;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createGreater(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = GREATER;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createLessOrEqual(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = LESS_OR_EQUAL;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createGreaterOrEqual(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = GREAT_OR_EQUAL;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createAnd(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = AND;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createOr(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = OR;
    node->left = left;
    node->right = right;
    return node;
}

ExprNode* ExprNode::createNot(ExprNode *operand) {
    ExprNode *node = new ExprNode();
    node->type = NOT;
    node->operand = operand;
    return node;
}

ExprNode* ExprNode::createUnaryMinus(ExprNode *operand) {
    ExprNode *node = new ExprNode();
    node->type = UNARY_MINUS;
    node->operand = operand;
    return node;
}

ExprNode* ExprNode::createElementAccess(ExprNode *operand, ExprNode *index) {
    ExprNode *node = new ExprNode();
    node->type = ELEMENT_ACCESS;
    node->operand = operand;
    node->index = index;
    return node;
}

ExprNode* ExprNode::createSlice(ExprNode *operand, ExprNode *low, ExprNode *high, ExprNode *max) {
    ExprNode *node = new ExprNode();
    node->type = SLICE;
    node->operand = operand;
    node->sliceLow = low;
    node->sliceHigh = high;
    node->sliceMax = max;
    return node;
}

ExprNode* ExprNode::createFunctionCall(ExprNode *operand, list<ExprNode *> *args) {
    ExprNode *node = new ExprNode();
    node->type = FUNCTION_CALL;
    node->operand = operand;
    node->args = args;
    return node;
}

ExprNode::ExprType ExprNode::getType() const {
    return type;
}

string ExprNode::getIdentifier() const {
    return identifier;
}

int ExprNode::getIntLiteral() const {
    return intLiteral;
}

float ExprNode::getFloatLiteral() const {
    return floatLiteral;
}

int ExprNode::getRuneLiteral() const {
    return runeLiteral;
}

string ExprNode::getStringLiteral() const {
    return stringLiteral;
}

bool ExprNode::getBoolLiteral() const {
    return boolLiteral;
}

ExprNode* ExprNode::getLeft() const {
    return left;
}

ExprNode* ExprNode::getRight() const {
    return right;
}

ExprNode* ExprNode::getOperand() const {
    return operand;
}

ExprNode* ExprNode::getIndex() const {
    return index;
}

list<ExprNode*>* ExprNode::getArgs() const {
    return args;
}

ExprNode* ExprNode::getLow() const {
    return sliceLow;
}

ExprNode* ExprNode::getHigh() const {
    return sliceHigh;
}

ExprNode* ExprNode::getMax() const {
    return sliceMax;
}

ExprListNode* ExprListNode::createExprList(ExprNode *expr) {
    ExprListNode *node = new ExprListNode();
    node->exprs = new list<ExprNode*>{expr};
    return node;
}

ExprListNode* ExprListNode::addExprToList(ExprListNode *list, ExprNode *expr) {
    list->exprs->push_back(expr);
    return list;
}

list<ExprNode*>* ExprListNode::getExprList() const {
    return exprs;
}

StmtListNode* StmtListNode::createStmtList(StmtNode *stmt) {
    StmtListNode *node = new StmtListNode();
    node->stmts = new list<StmtNode*>{stmt};
    return node;
}

StmtListNode* StmtListNode::addStmtToList(StmtListNode *list, StmtNode *stmt) {
    list->stmts->push_back(stmt);
    return list;
}

list<StmtNode*>* StmtListNode::getStmtList() const {
    return stmts;
}

StmtNode* StmtNode::createReturn(ExprListNode *exprList) {
    StmtNode *node = new StmtNode();
    node->type = RETURN;
    node->returnExprList = exprList;
    return node;
}

StmtNode* StmtNode::createBreak() {
    StmtNode *node = new StmtNode();
    node->type = BREAK;
    return node;
}

StmtNode* StmtNode::createContinue() {
    StmtNode *node = new StmtNode();
    node->type = CONTINUE;
    return node;
}

StmtNode* StmtNode::createBlock(StmtListNode *stmtList) {
    StmtNode *node = new StmtNode();
    node->type = BLOCK;
    node->stmtList = stmtList;
    return node;
}
