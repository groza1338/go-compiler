//
// Created by Silvitio on 05.11.2025.
//

#include "classes.h"

unsigned int AstNode::maxId = 0;

void AstNode::appendDotNode(string &res) const {
    res += "node" + to_string(id) + " [label=\"" + getDotLabel() + "\"];\n";
}

void AstNode::appendDotEdge(string &res, const AstNode *child, const string &edgeLabel) const {
    if (!child) return;

    res += "node" + to_string(id) + " -> node" + to_string(child->getId());

    if (!edgeLabel.empty()) {
        res += " [label=\"" + edgeLabel + "\"]";
    }

    res += ";\n" + child->toDot();
}

ExprNode* ExprNode::createIdentifier(ValueNode *value) {
    ExprNode *node = new ExprNode();
    node->type = ID;
    node->identifier = value;
    return node;
}

ExprNode* ExprNode::createLiteralVal(ValueNode *value) {
    ExprNode *node = new ExprNode();
    node->type = LIT_VAL;
    node->value = value;
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

ExprNode* ExprNode::createFunctionCall(ExprNode *operand, ExprListNode *args) {
    ExprNode *node = new ExprNode();
    node->type = FUNCTION_CALL;
    node->operand = operand;
    node->args = args;
    return node;
}

ExprNode::ExprType ExprNode::getType() const {
    return type;
}

ValueNode* ExprNode::getIdentifier() const {
    return identifier;
}

ValueNode* ExprNode::getLiteral() const {
    return value;
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

ExprListNode* ExprNode::getArgs() const {
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

string ExprNode::getDotLabel() const {
    switch (type) {
        case ID:                return "IDENTIFIER";
        case EXPR_IN_BRACKETS:  return "()";
        case LIT_VAL:           return "LIT_VAL";
        case SUMMARY:           return "+";
        case SUBTRACTION:       return "-";
        case MULTIPLICATION:    return "*";
        case DIVISION:          return "/";
        case EQUAL:             return "==";
        case NOT_EQUAL:         return "!=";
        case LESS:              return "<";
        case GREATER:           return ">";
        case LESS_OR_EQUAL:     return "<=";
        case GREATER_OR_EQUAL:  return ">=";
        case AND:               return "&&";
        case OR:                return "||";
        case NOT:               return "!";
        case UNARY_MINUS:       return "-";
        case ELEMENT_ACCESS:    return "[i]";
        case SLICE:             return "[]";
        case FUNCTION_CALL:     return "func()";
        default:                return "UNKNOWN";
    }
}

string ExprNode::toDot() const {
    string result;

    appendDotNode(result);

    appendDotEdge(result, identifier, "id");
    appendDotEdge(result, value, "value");

    appendDotEdge(result, left, "left");
    appendDotEdge(result, right, "right");
    appendDotEdge(result, operand, "operand");
    appendDotEdge(result, index, "index");
    appendDotEdge(result, sliceLow, "sliceLow");
    appendDotEdge(result, sliceHigh, "sliceHigh");
    appendDotEdge(result, sliceMax, "sliceMax");
    appendDotEdge(result, args, "args");

    return result;
}

ExprNode::ExprNode(): AstNode() {
    type = NONE;
    identifier = nullptr;
    value = nullptr;
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

string ExprListNode::getDotLabel() const {
    return "EXPR_LIST";
}

string ExprListNode::toDot() const {
    string result;
    appendDotNode(result);

    if (exprs) {
        int i = 0;
        for (ExprNode *expr : *exprs) {
            appendDotEdge(result, expr, "expr_" + to_string(i++));
        }
    }

    return result;
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

string StmtListNode::getDotLabel() const {
    return "STMT_LIST";
}

string StmtListNode::toDot() const {
    string result;
    appendDotNode(result);

    if (stmts) {
        int i = 0;
        for (StmtNode *stmt : *stmts) {
            appendDotEdge(result, stmt, "stmt_" + to_string(i++));
        }
    }

    return result;
}

StmtNode* StmtNode::createDecl(DeclNode *decl) {
    StmtNode *node = new StmtNode();
    node->type = DECLARATION;
    node->decl = decl;
    return node;
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

string StmtNode::getDotLabel() const {
    switch (type) {
        case DECLARATION:   return "DECLARATION";
        case SIMPLE:        return "SIMPLE";
        case RETURN:        return "RETURN";
        case BREAK:         return "BREAK";
        case CONTINUE:      return "CONTINUE";
        case IF:            return "IF";
        case SWITCH:        return "SWITCH";
        case FOR:           return "FOR";
        case FOR_PARAM:     return "FOR_PARAM";
        case FOR_RANGE:     return "FOR_RANGE";
        case EMPTY:         return "EMPTY";
        default:            return "UNKNOWN";
    }
}

string StmtNode::toDot() const {
    string result;
    appendDotNode(result);

    appendDotEdge(result, decl, "decl");
    appendDotEdge(result, exprList, "expr_list");
    appendDotEdge(result, stmtList, "stmt_list");
    appendDotEdge(result, simpleStmt, "simple_stmt");
    appendDotEdge(result, condition, "condition");
    appendDotEdge(result, thenBranch, "then_branch");
    appendDotEdge(result, elseBranch, "else_branch");
    appendDotEdge(result, body, "body");
    appendDotEdge(result, caseList, "case_list");
    appendDotEdge(result, initStmt, "init_stmt");
    appendDotEdge(result, postStmt, "post_stmt");

    return result;
}

StmtNode::StmtNode(): AstNode() {
    type = NONE;
    decl = nullptr;
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

string CaseNode::getDotLabel() const {
    if (!exprList) {
        return "DEFAULT";
    } else {
        return "CASE";
    }
}

string CaseNode::toDot() const {
    string result;
    appendDotNode(result);

    appendDotEdge(result, exprList, "expr_list");
    appendDotEdge(result, stmtList, "stmt_list");

    return result;
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

string CaseListNode::getDotLabel() const {
    return "CASE_LIST";
}

string CaseListNode::toDot() const {
    string result;
    appendDotNode(result);

    if (caseList) {
        int i = 0;
        for (CaseNode *caseElem : *caseList) {
            appendDotEdge(result, caseElem, "branch_" + to_string(i++));
        }
    }

    return result;
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

string SimpleStmtNode::getDotLabel() const {
    switch (type) {
        case EXPR:              return "EXPR_STMT";
        case INC:               return "INC_STMT";
        case DEC:               return "DEC_STMT";
        case ASSIGN:            return "ASSIGN_STMT";
        case SHORT_VAR_DECL:    return "SHORT_VAR_DECL";
        default:                return "UNKNOWN";
    }
}

string SimpleStmtNode::toDot() const {
    string result;
    appendDotNode(result);

    appendDotEdge(result, expr, "expr");
    appendDotEdge(result, left, "left");
    appendDotEdge(result, right, "right");

    return result;
}

SimpleStmtNode::SimpleStmtNode(): AstNode() {
    type = NONE;
    expr = nullptr;
    left = nullptr;
    right = nullptr;
}

IdListNode* IdListNode::createIdList(ValueNode *id) {
    IdListNode *node = new IdListNode();
    node->ids = new list<ValueNode*>{id};
    return node;
}

IdListNode* IdListNode::addIdToList(IdListNode *list, ValueNode *id) {
    list->ids->push_back(id);
    return list;
}

list<ValueNode*>* IdListNode::getIdList() const {
    return ids;
}

string IdListNode::getDotLabel() const {
    return "ID_LIST";
}

string IdListNode::toDot() const {
    string res;
    appendDotNode(res);
    if (ids) {
        int i = 0;
        for (auto *v : *ids) {
            appendDotEdge(res, v, "id_" + to_string(i++));
        }
    }
    return res;
}

TypeNode* TypeNode::createNamedType(TypeNameNode *name) {
    TypeNode *node = new TypeNode();
    node->kind = NAMED;
    node->name = name;
    return node;
}

TypeNode* TypeNode::createArrayType(ExprNode *len, TypeNode *elemType) {
    TypeNode *node = new TypeNode();
    node->kind = ARRAY;
    node->arrayLen = len;
    node->elemType = elemType;
    return node;
}

TypeNode* TypeNode::createFuncType(SignatureNode *signature) {
    TypeNode *node = new TypeNode();
    node->kind = FUNC;
    node->signature = signature;
    return node;
}

TypeNode* TypeNode::createSliceType(TypeNode *elemType) {
    TypeNode *node = new TypeNode();
    node->kind = SLICE;
    node->elemType = elemType;
    return node;
}

string TypeNode::getDotLabel() const {
    switch (kind) {
        case NAMED: return "TYPE_NAMED";
        case ARRAY: return "TYPE_ARRAY";
        case SLICE: return "TYPE_SLICE";
        case FUNC: return "TYPE_FUNC";
        default: return "TYPE";
    }
}

string TypeNode::toDot() const {
    string res;
    appendDotNode(res);

    switch (kind) {
    case NAMED:
        appendDotEdge(res, name, "name");
        break;
    case ARRAY:
        appendDotEdge(res, arrayLen, "len");
        appendDotEdge(res, elemType, "elem");
        break;
    case SLICE:
        appendDotEdge(res, elemType, "elem");
        break;
    case FUNC:
        appendDotEdge(res, signature, "signature");
        break;
    default:
        break;
    }

    return res;
}

TypeNode::TypeNode(): AstNode() {
    kind = NONE;
    name = nullptr;
    arrayLen = nullptr;
    elemType = nullptr;
    signature = nullptr;
}

ParamDeclNode* ParamDeclNode::createParamDecl(IdListNode *ids, TypeNode *type) {
    ParamDeclNode *node = new ParamDeclNode();
    node->idList = ids;
    node->type = type;
    return node;
}

string ParamDeclNode::getDotLabel() const {
    return "PARAM_DECL";
}

string ParamDeclNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, idList, "ids");
    appendDotEdge(res, type, "type");
    return res;
}

ParamDeclNode::ParamDeclNode() {
    idList = nullptr;
    type = nullptr;
}

ParamDeclListNode* ParamDeclListNode::createParamDeclList(ParamDeclNode *param) {
    ParamDeclListNode *node = new ParamDeclListNode();
    node->paramList = new list<ParamDeclNode*>{param};
    return node;
}

ParamDeclListNode* ParamDeclListNode::addParamDeclToList(ParamDeclListNode *list, ParamDeclNode *param) {
    list->paramList->push_back(param);
    return list;
}

list<ParamDeclNode*>* ParamDeclListNode::getParamList() const {
    return paramList;
}

string ParamDeclListNode::getDotLabel() const {
    return "PARAM_DECL_LIST";
}

string ParamDeclListNode::toDot() const {
    string res;
    appendDotNode(res);
    if (paramList) {
        int i = 0;
        for (auto *p : *paramList) {
            appendDotEdge(res, p, "param_" + to_string(i++));
        }
    }
    return res;
}

SignatureNode* SignatureNode::createSignature(ParamDeclListNode *paramList, ResultNode *result) {
    SignatureNode *node = new SignatureNode();
    node->paramList = paramList;
    node->result = result;
    return node;
}

string SignatureNode::getDotLabel() const {
    return "SIGNATURE";
}

string SignatureNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, paramList, "params");
    appendDotEdge(res, result, "result");
    return res;
}

SignatureNode::SignatureNode(): AstNode() {
    paramList = nullptr;
    result = nullptr;
}

ResultNode* ResultNode::createResult(ParamDeclListNode *paramList) {
    ResultNode *node = new ResultNode();
    node->paramList = paramList;
    return node;
}

ResultNode* ResultNode::createResult(TypeNode *type) {
    ResultNode *node = new ResultNode();
    node->type = type;
    return node;
}

string ResultNode::getDotLabel() const {
    return "RESULT";
}

string ResultNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, paramList, "params");
    appendDotEdge(res, type, "type");
    return res;

}

ResultNode::ResultNode(): AstNode() {
    paramList = nullptr;
    type = nullptr;
}

VarSpecNode* VarSpecNode::createVarSpec(IdListNode *idList, TypeNode *type, ExprListNode *exprList) {
    VarSpecNode *node = new VarSpecNode();
    node->idList = idList;
    node->type = type;
    node->exprList = exprList;
    return node;
}

string VarSpecNode::getDotLabel() const {
    return "VAR_SPEC";
}

string VarSpecNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, idList, "ids");
    appendDotEdge(res, type, "type");
    appendDotEdge(res, exprList, "values");
    return res;
}

VarSpecNode::VarSpecNode() {
    idList = nullptr;
    type = nullptr;
    exprList = nullptr;
}

VarSpecListNode* VarSpecListNode::createVarSpecList(VarSpecNode *var) {
    VarSpecListNode *node = new VarSpecListNode();
    node->varList = new list<VarSpecNode*>{var};
    return node;
}

VarSpecListNode* VarSpecListNode::addVarSpecToList(VarSpecListNode *list, VarSpecNode *var) {
    list->varList->push_back(var);
    return list;
}

list<VarSpecNode*>* VarSpecListNode::getList() const {
    return varList;
}

string VarSpecListNode::getDotLabel() const {
    return "VAR_SPEC_LIST";
}

string VarSpecListNode::toDot() const {
    string res;
    appendDotNode(res);
    if (varList) {
        int i = 0;
        for (auto *v : *varList) {
            appendDotEdge(res, v, "spec_" + to_string(i++));
        }
    }
    return res;
}

VarSpecListNode::VarSpecListNode(): AstNode() {
    varList = nullptr;
}

ConstSpecNode* ConstSpecNode::createConstSpec(IdListNode *idList, TypeNode *type, ExprListNode *exprList) {
    ConstSpecNode *node = new ConstSpecNode();
    node->idList = idList;
    node->type = type;
    node->exprList = exprList;
    return node;
}

string ConstSpecNode::getDotLabel() const {
    return "CONST_SPEC";
}

string ConstSpecNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, idList, "ids");
    appendDotEdge(res, type, "type");
    appendDotEdge(res, exprList, "values");
    return res;
}

ConstSpecNode::ConstSpecNode(): AstNode() {
    idList = nullptr;
    type = nullptr;
    exprList = nullptr;
}

ConstSpecListNode* ConstSpecListNode::createConstSpecList(ConstSpecNode *spec) {
    ConstSpecListNode *node = new ConstSpecListNode();
    node->specList = new list<ConstSpecNode*>{spec};
    return node;
}

ConstSpecListNode* ConstSpecListNode::addConstSpecToList(ConstSpecListNode *list, ConstSpecNode *spec) {
    list->specList->push_back(spec);
    return list;
}

list<ConstSpecNode*>* ConstSpecListNode::getList() const {
    return specList;
}

string ConstSpecListNode::getDotLabel() const {
    return "CONST_SPEC_LIST";
}

string ConstSpecListNode::toDot() const {
    string res;
    appendDotNode(res);
    if (specList) {
        int i = 0;
        for (auto *s : *specList) {
            appendDotEdge(res, s, "spec_" + to_string(i++));
        }
    }
    return res;
}

ConstSpecListNode::ConstSpecListNode(): AstNode() {
    specList = nullptr;
}

DeclNode* DeclNode::createDecl(ConstSpecNode *constSpec) {
    DeclNode *node = new DeclNode();
    node->constSpecList = ConstSpecListNode::createConstSpecList(constSpec);
    return node;
}

DeclNode* DeclNode::createDecl(ConstSpecListNode *constSpecList) {
    DeclNode *node = new DeclNode();
    node->constSpecList = constSpecList;
    return node;
}

DeclNode* DeclNode::createDecl(VarSpecNode *varSpec) {
    DeclNode *node = new DeclNode();
    node->varSpecList = VarSpecListNode::createVarSpecList(varSpec);
    return node;
}

DeclNode* DeclNode::createDecl(VarSpecListNode *varSpecList) {
    DeclNode *node = new DeclNode();
    node->varSpecList = varSpecList;
    return node;
}

string DeclNode::getDotLabel() const {
    return "DECL";
}

string DeclNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, constSpecList, "consts");
    appendDotEdge(res, varSpecList, "vars");
    return res;

}

DeclNode::DeclNode(): AstNode() {
    constSpecList = nullptr;
    varSpecList = nullptr;
}

FuncDeclNode* FuncDeclNode::createFuncDecl(ValueNode *id, SignatureNode *signature, StmtNode *body) {
    FuncDeclNode *node = new FuncDeclNode();
    node->id = id;
    node->signature = signature;
    node->body = body;
    return node;
}

string FuncDeclNode::getDotLabel() const {
    return "FUNC_DECL";
}

string FuncDeclNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, id, "id");
    appendDotEdge(res, signature, "signature");
    appendDotEdge(res, body, "body");
    return res;
}

FuncDeclNode::FuncDeclNode(): AstNode() {
    id = nullptr;
    signature = nullptr;
    body = nullptr;
}

TopLevelDeclNode* TopLevelDeclNode::createTopLevelDecl(DeclNode *decl) {
    TopLevelDeclNode *node = new TopLevelDeclNode();
    node->decl = decl;
    return node;
}

TopLevelDeclNode* TopLevelDeclNode::createTopLevelDecl(FuncDeclNode *funcDecl) {
    TopLevelDeclNode *node = new TopLevelDeclNode();
    node->funcDecl = funcDecl;
    return node;
}

string TopLevelDeclNode::getDotLabel() const {
    return "TOP_LEVEL_DECL";
}

string TopLevelDeclNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, decl, "decl");
    appendDotEdge(res, funcDecl, "func");
    return res;
}

TopLevelDeclNode::TopLevelDeclNode(): AstNode() {
    decl = nullptr;
    funcDecl = nullptr;
}

TopLevelDeclListNode* TopLevelDeclListNode::createList(TopLevelDeclNode *elem) {
    TopLevelDeclListNode *node = new TopLevelDeclListNode();
    node->elemList = new list<TopLevelDeclNode*>{elem};
    return node;
}

TopLevelDeclListNode* TopLevelDeclListNode::addElemToList(TopLevelDeclListNode *elemList, TopLevelDeclNode *elem) {
    elemList->elemList->push_back(elem);
    return elemList;
}

list<TopLevelDeclNode*>* TopLevelDeclListNode::getList() const {
    return elemList;
}

string TopLevelDeclListNode::getDotLabel() const {
    return "TOP_LEVEL_DECL_LIST";
}

string TopLevelDeclListNode::toDot() const {
    string res;
    appendDotNode(res);
    if (elemList) {
        int i = 0;
        for (auto *e : *elemList) {
            appendDotEdge(res, e, "decl_" + to_string(i++));
        }
    }
    return res;
}

TopLevelDeclListNode::TopLevelDeclListNode(): AstNode() {
    elemList = nullptr;
}

PackageClauseNode* PackageClauseNode::createNode(ValueNode *id) {
    PackageClauseNode *node = new PackageClauseNode();
    node->id = id;
    return node;
}

string PackageClauseNode::getDotLabel() const {
    return "PACKAGE";
}

string PackageClauseNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, id, "id");
    return res;
}

PackageClauseNode::PackageClauseNode(): AstNode() {
    id = nullptr;
}

ImportSpecNode* ImportSpecNode::createSimple(ValueNode *import) {
    ImportSpecNode *node = new ImportSpecNode();
    node->importType = SIMPLE;
    node->import = import;
    return node;
}

ImportSpecNode* ImportSpecNode::createPoint(ValueNode *import) {
    ImportSpecNode *node = new ImportSpecNode();
    node->importType = POINT;
    node->import = import;
    return node;
}

ImportSpecNode* ImportSpecNode::createNamed(ValueNode *alias, ValueNode *import) {
    ImportSpecNode *node = new ImportSpecNode();
    node->importType = NAMED;
    node->alias = alias;
    node->import = import;
    return node;
}

string ImportSpecNode::getDotLabel() const {
    switch (importType) {
    case SIMPLE: return "IMPORT";
    case POINT:  return "IMPORT_POINT";
    case NAMED:  return "IMPORT_NAMED";
    default:     return "IMPORT_SPEC";
    }
}

string ImportSpecNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, import, "path");
    appendDotEdge(res, alias, "alias");
    return res;
}

ImportSpecNode::ImportSpecNode(): AstNode() {
    importType = NONE;
    import = nullptr;
    alias = nullptr;
}

ImportSpecListNode* ImportSpecListNode::createList(ImportSpecNode *elem) {
    ImportSpecListNode *node = new ImportSpecListNode();
    node->elemList = new list<ImportSpecNode*>{elem};
    return node;
}

ImportSpecListNode* ImportSpecListNode::addElemToList(ImportSpecListNode *elemList, ImportSpecNode *elem) {
    elemList->elemList->push_back(elem);
    return elemList;
}

list<ImportSpecNode*>* ImportSpecListNode::getList() const {
    return elemList;
}

string ImportSpecListNode::getDotLabel() const {
    return "IMPORT_SPEC_LIST";
}

string ImportSpecListNode::toDot() const {
    string res;
    appendDotNode(res);
    if (elemList) {
        int i = 0;
        for (auto *e : *elemList) {
            appendDotEdge(res, e, "spec_" + to_string(i++));
        }
    }
    return res;
}

ImportSpecListNode::ImportSpecListNode(): AstNode() {
    elemList = nullptr;
}

ImportDeclNode* ImportDeclNode::createNode(ImportSpecNode *import) {
    ImportDeclNode *node = new ImportDeclNode();
    node->importList = ImportSpecListNode::createList(import);
    return node;
}

ImportDeclNode* ImportDeclNode::createNode(ImportSpecListNode *importList) {
    ImportDeclNode *node = new ImportDeclNode();
    node->importList = importList;
    return node;
}

string ImportDeclNode::getDotLabel() const {
    return "IMPORT_DECL";
}

string ImportDeclNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, importList, "specs");
    return res;
}

ImportDeclNode::ImportDeclNode(): AstNode() {
    importList = nullptr;
}

ImportDeclListNode* ImportDeclListNode::createList(ImportDeclNode *elem) {
    ImportDeclListNode *node = new ImportDeclListNode();
    node->elemList = new list<ImportDeclNode*>{elem};
    return node;
}

ImportDeclListNode* ImportDeclListNode::addElemToList(ImportDeclListNode *elemList, ImportDeclNode *elem) {
    elemList->elemList->push_back(elem);
    return elemList;
}

list<ImportDeclNode*>* ImportDeclListNode::getList() const {
    return elemList;
}

string ImportDeclListNode::getDotLabel() const {
    return "IMPORT_DECL_LIST";
}

string ImportDeclListNode::toDot() const {
    string res;
    appendDotNode(res);
    if (elemList) {
        int i = 0;
        for (auto *e : *elemList) {
            appendDotEdge(res, e, "import_" + to_string(i++));
        }
    }
    return res;
}

ImportDeclListNode::ImportDeclListNode(): AstNode() {
    elemList = nullptr;
}

ProgramNode* ProgramNode::createNode(PackageClauseNode *packageClause, ImportDeclListNode *importDeclList,
    TopLevelDeclListNode *topLevelDeclList) {
    ProgramNode *node = new ProgramNode();
    node->packageClause = packageClause;
    node->importDeclList = importDeclList;
    node->topLevelDeclList = topLevelDeclList;
    return node;
}

string ProgramNode::getDotLabel() const {
    return "PROGRAM";
}

string ProgramNode::toDot() const {
    string res;
    appendDotNode(res);
    appendDotEdge(res, packageClause, "package");
    appendDotEdge(res, importDeclList, "imports");
    appendDotEdge(res, topLevelDeclList, "decls");
    return res;
}

ProgramNode::ProgramNode(): AstNode() {
    packageClause = nullptr;
    importDeclList = nullptr;
    topLevelDeclList = nullptr;
}

TypeNameNode* TypeNameNode::createTypeInt() {
    TypeNameNode *node = new TypeNameNode();
    node->type = INT_64;
    return node;
}

TypeNameNode* TypeNameNode::createTypeFloat() {
    TypeNameNode *node = new TypeNameNode();
    node->type = FLOAT_64;
    return node;
}

TypeNameNode* TypeNameNode::createTypeBool() {
    TypeNameNode *node = new TypeNameNode();
    node->type = BOOL;
    return node;
}

TypeNameNode* TypeNameNode::createTypeString() {
    TypeNameNode *node = new TypeNameNode();
    node->type = STRING;
    return node;
}

TypeNameNode* TypeNameNode::createTypeRune() {
    TypeNameNode *node = new TypeNameNode();
    node->type = RUNE;
    return node;
}

string TypeNameNode::getDotLabel() const {
    switch (type) {
    case INT_64:  return "int";
    case FLOAT_64:return "float64";
    case BOOL:    return "bool";
    case STRING:  return "string";
    case RUNE:    return "rune";
    default:      return "TYPE_NAME";
    }
}

string TypeNameNode::toDot() const {
    string res;
    appendDotNode(res);
    return res;
}

TypeNameNode::TypeNameNode() {
    type = NONE;
}

ValueNode* ValueNode::createInt(int value) {
    ValueNode *node = new ValueNode();
    node->valueType = LIT_INT;
    node->intValue = value;
    return node;
}

ValueNode* ValueNode::createFloat(float value) {
    ValueNode *node = new ValueNode();
    node->valueType = LIT_FLOAT;
    node->floatValue = value;
    return node;
}

ValueNode* ValueNode::createBool(bool value) {
    ValueNode *node = new ValueNode();
    node->valueType = LIT_BOOL;
    node->boolValue = value;
    return node;
}

ValueNode* ValueNode::createString(string *value) {
    ValueNode *node = new ValueNode();
    node->valueType = LIT_STRING;
    node->stringValue = value;
    return node;
}

ValueNode* ValueNode::createRune(int value) {
    ValueNode *node = new ValueNode();
    node->valueType = LIT_RUNE;
    node->intValue = value;
    return node;
}

ValueNode::ValueType ValueNode::getValueType() const {
    return valueType;
}

int ValueNode::getInt() const {
    return intValue;
}

float ValueNode::getFloat() const {
    return floatValue;
}

bool ValueNode::getBool() const {
    return boolValue;
}

string* ValueNode::getString() const {
    return stringValue;
}

int ValueNode::getRune() const {
    return intValue;
}

string ValueNode::getDotLabel() const {
    switch (valueType) {
        case LIT_INT:       return to_string(intValue);
        case LIT_FLOAT:     return to_string(floatValue);
        case LIT_RUNE:      return to_string(intValue);
        case LIT_STRING:    return *stringValue;
        case LIT_BOOL:      return boolValue ? "true" : "false";
        default:            return "UNKNOWN";
    }
}

string ValueNode::toDot() const {
    string result;
    appendDotNode(result);
    return result;
}

ValueNode::ValueNode(): AstNode() {
    valueType = NONE;
    intValue = 0;
    floatValue = 0;
    boolValue = false;
    stringValue = nullptr;
}
