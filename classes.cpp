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
    node->type = GREATER_OR_EQUAL;
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

ExprNode::ExprNode(): AstNode() {
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

StmtNode* StmtNode::createSimple(SimpleStmtNode *simpleStmt) {
    StmtNode *node = new StmtNode();
    node->type = SIMPLE;
    node->simpleStmt = simpleStmt;
    return node;
}

StmtNode* StmtNode::createReturn(ExprListNode *exprList) {
    StmtNode *node = new StmtNode();
    node->type = RETURN;
    node->exprList = exprList;
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

StmtNode* StmtNode::createIf(SimpleStmtNode *simpleStmt, ExprNode *condition, StmtNode *thenBranch, StmtNode *elseBranch) {
    StmtNode *node = new StmtNode();
    node->type = IF;
    node->simpleStmt = simpleStmt;
    node->condition = condition;
    node->thenBranch = thenBranch;
    node->elseBranch = elseBranch;
    return node;
}

StmtNode* StmtNode::createSwitch(SimpleStmtNode *simpleStmt, ExprNode *condition, CaseListNode *cases) {
    StmtNode *node = new StmtNode();
    node->type = SWITCH;
    node->simpleStmt = simpleStmt;
    node->condition = condition;
    node->caseList = cases;
    return node;
}

StmtNode* StmtNode::createFor(ExprNode *condition, StmtNode *body) {
    StmtNode *node = new StmtNode();
    node->type = FOR;
    node->condition = condition;
    node->body = body;
    return node;
}

StmtNode* StmtNode::createFor(SimpleStmtNode *initStmt, ExprNode *condition, SimpleStmtNode *postStmt, StmtNode *body) {
    StmtNode *node = new StmtNode();
    node->type = FOR_PARAM;
    node->initStmt = initStmt;
    node->condition = condition;
    node->postStmt = postStmt;
    node->body = body;
    return node;
}

StmtNode * StmtNode::createFor(ExprListNode *exprList, ExprNode *expr, StmtNode *body) {
    StmtNode *node = new StmtNode();
    node->type = FOR_RANGE;
    node->exprList = exprList;
    node->condition = expr;
    node->body = body;
    return node;
}

StmtNode::StmtNode(): AstNode() {
    type = NONE;
    exprList = nullptr;
    stmtList = nullptr;
    simpleStmt = nullptr;
    condition = nullptr;
    thenBranch = nullptr;
    elseBranch = nullptr;
    body = nullptr;
    caseList = nullptr;
    initStmt = nullptr;
    postStmt = nullptr;
}

CaseNode* CaseNode::createCase(ExprListNode *exprList, StmtListNode *stmtList) {
    CaseNode* node = new CaseNode();
    node->exprList = exprList;
    node->stmtList = stmtList;
    return node;
}

ExprListNode* CaseNode::getExprList() const {
    return exprList;
}

StmtListNode* CaseNode::getStmtList() const {
    return stmtList;
}

CaseNode::CaseNode() : AstNode() {
    exprList = nullptr;
    stmtList = nullptr;
}

CaseListNode* CaseListNode::createCaseList(CaseNode *inCase) {
    CaseListNode *node = new CaseListNode();
    node->caseList = new list<CaseNode*>{inCase};
    return node;
}

CaseListNode* CaseListNode::addCaseToList(CaseListNode *list, CaseNode *inCase) {
    list->caseList->push_back(inCase);
    return list;
}

list<CaseNode*>* CaseListNode::getCaseList() const {
    return caseList;
}

SimpleStmtNode* SimpleStmtNode::createExpr(ExprNode *expr) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = EXPR;
    node->expr = expr;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createInc(ExprNode *expr) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = INC;
    node->expr = expr;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createDec(ExprNode *expr) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = DEC;
    node->expr = expr;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createAssign(ExprListNode *left, ExprListNode *right) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = ASSIGN;
    node->left = left;
    node->right = right;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createShortVarDecl(ExprListNode *left, ExprListNode *right) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = SHORT_VAR_DECL;
    node->left = left;
    node->right = right;
    return node;
}

SimpleStmtNode::SimpleStmtType SimpleStmtNode::getType() const {
    return type;
}

ExprNode* SimpleStmtNode::getExpr() const {
    return expr;
}

ExprListNode* SimpleStmtNode::getLeft() const {
    return left;
}

ExprListNode* SimpleStmtNode::getRight() const {
    return right;
}

SimpleStmtNode::SimpleStmtNode(): AstNode() {
    type = NONE;
    expr = nullptr;
    left = nullptr;
    right = nullptr;
}
