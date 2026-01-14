//
// Created by Silvitio on 05.11.2025.
//

#include "classes.h"

unsigned int AstNode::maxId = 0;

static bool isBlankIdentifier(const ValueNode *id) {
    if (!id || !id->getString()) {
        return false;
    }
    return *id->getString() == "_";
}

static SemanticType semanticTypeFromValue(ValueNode *value) {
    if (!value) {
        return SemanticType::makeBase(SemanticType::UNKNOWN);
    }
    switch (value->getValueType()) {
        case ValueNode::LIT_INT: return SemanticType::makeBase(SemanticType::INT);
        case ValueNode::LIT_FLOAT: return SemanticType::makeBase(SemanticType::FLOAT);
        case ValueNode::LIT_BOOL: return SemanticType::makeBase(SemanticType::BOOL);
        case ValueNode::LIT_STRING: return SemanticType::makeBase(SemanticType::STRING);
        case ValueNode::LIT_RUNE: return SemanticType::makeBase(SemanticType::RUNE);
        default: return SemanticType::makeBase(SemanticType::UNKNOWN);
    }
}

static bool isAssignable(const SemanticType &left, const SemanticType &right) {
    if (left.isError || right.isError) {
        return true;
    }
    return left.sameKind(right);
}

static bool isIdentifierExpr(ExprNode *expr, string *outName) {
    if (!expr || expr->getType() != ExprNode::ID) {
        return false;
    }
    ValueNode *idVal = expr->getIdentifier();
    if (!idVal || !idVal->getString()) {
        return false;
    }
    if (outName) {
        *outName = *idVal->getString();
    }
    return true;
}

static bool isAddressableExpr(ExprNode *expr, SemanticContext &ctx) {
    if (!expr) {
        return false;
    }
    switch (expr->getType()) {
        case ExprNode::ID:
            return true;
        case ExprNode::ELEMENT_ACCESS:
            if (ExprNode *operand = expr->getOperand()) {
                SemanticType operandType = operand->semantics(ctx);
                if (operandType.isString() && operandType.isScalar()) {
                    return false;
                }
            }
            return true;
        case ExprNode::EXPR_IN_BRACKETS:
            return isAddressableExpr(expr->getOperand(), ctx);
        default:
            return false;
    }
}

static bool getRangeTypes(const SemanticType &rangeType, SemanticType &indexType, SemanticType &valueType) {
    indexType = SemanticType::makeBase(SemanticType::INT);
    if (rangeType.base == SemanticType::STRING && rangeType.isScalar()) {
        valueType = SemanticType::makeBase(SemanticType::RUNE);
        return true;
    }
    if (rangeType.arrayDims > 0) {
        valueType = rangeType;
        valueType.arrayDims -= 1;
        return true;
    }
    if (rangeType.sliceDims > 0) {
        valueType = rangeType;
        valueType.sliceDims -= 1;
        return true;
    }
    return false;
}

static void checkRangeTarget(ExprNode *leftExpr, const SemanticType &expectedType, SemanticContext &ctx) {
    if (!leftExpr || expectedType.base == SemanticType::UNKNOWN) {
        return;
    }
    string idName;
    bool isId = isIdentifierExpr(leftExpr, &idName);
    if (isId && idName == "_") {
        return;
    }
    if (!isAddressableExpr(leftExpr, ctx)) {
        leftExpr->semantics(ctx);
        ctx.report("Range assignment requires addressable identifiers.");
        return;
    }
    SemanticType leftType = SemanticType::makeBase(SemanticType::UNKNOWN);
    if (isId) {
        SemanticType found;
        if (!ctx.lookup(idName, found)) {
            ctx.declare(idName, expectedType);
            return;
        }
        leftType = found;
    } else {
        leftType = leftExpr->semantics(ctx);
    }
    if (!isAssignable(leftType, expectedType)) {
        ctx.report("Range assignment type mismatch.");
    }
}

static vector<SemanticType> collectResultTypes(ResultNode *result, bool &allowBareReturn) {
    vector<SemanticType> types;
    allowBareReturn = false;
    if (!result) {
        return types;
    }
    if (TypeNode *typeNode = result->getType()) {
        types.push_back(typeNode->getSemanticType());
        return types;
    }
    ParamDeclListNode *paramList = result->getParamList();
    if (!paramList || !paramList->getParamList()) {
        return types;
    }
    for (ParamDeclNode *param : *paramList->getParamList()) {
        if (!param) {
            continue;
        }
        TypeNode *paramTypeNode = param->getType();
        if (!paramTypeNode) {
            continue;
        }
        SemanticType paramType = paramTypeNode->getSemanticType();
        IdListNode *ids = param->getIdList();
        if (ids && ids->getIdList() && !ids->getIdList()->empty()) {
            allowBareReturn = true;
            for (ValueNode *id : *ids->getIdList()) {
                if (id) {
                    types.push_back(paramType);
                }
            }
        } else {
            types.push_back(paramType);
        }
    }
    return types;
}

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

ExprNode* ExprNode::createIota() {
    ExprNode *node = new ExprNode();
    node->type = IOTA;
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

ExprNode* ExprNode::createModulo(ExprNode *left, ExprNode *right) {
    ExprNode *node = new ExprNode();
    node->type = MODULO;
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

ExprNode* ExprNode::createAddressOf(ExprNode *operand) {
    ExprNode *node = new ExprNode();
    node->type = ADDRESS_OF;
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

ExprNode* ExprNode::createSelector(ExprNode *operand, ValueNode *field) {
    ExprNode *node = new ExprNode();
    node->type = SELECTOR;
    node->operand = operand;
    node->identifier = field;
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

ExprNode* ExprNode::createArrayLiteral(TypeNode *elemType, ExprNode *len, ExprListNode *elems, bool lenAuto) {
    ExprNode *node = new ExprNode();
    node->type = ARRAY_LIT;
    node->arrayElemType = elemType;
    node->arrayLen = len;
    node->arrayElems = elems;
    node->arrayLenAuto = lenAuto;
    if (node->arrayLen == nullptr && node->arrayElems != nullptr && node->arrayElems->getExprList() != nullptr) {
        int elemCount = static_cast<int>(node->arrayElems->getExprList()->size());
        node->arrayLen = ExprNode::createLiteralVal(ValueNode::createInt(elemCount));
    }
    return node;
}

ExprNode* ExprNode::createSliceLiteral(TypeNode *elemType, ExprListNode *elems) {
    ExprNode *node = new ExprNode();
    node->type = SLICE_LIT;
    node->arrayElemType = elemType;
    node->arrayElems = elems;
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

TypeNode* ExprNode::getArrayElemType() const {
    return arrayElemType;
}

ExprNode* ExprNode::getArrayLen() const {
    return arrayLen;
}

ExprListNode* ExprNode::getArrayElems() const {
    return arrayElems;
}

bool ExprNode::isArrayLenAuto() const {
    return arrayLenAuto;
}

SemanticType ExprNode::semantics(SemanticContext &ctx) {
    switch (type) {
        case ID: {
            if (!identifier || !identifier->getString()) {
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
                break;
            }
            const string &name = *identifier->getString();
            if (name == "_") {
                ctx.report("Blank identifier can only be used on the left side of assignment.");
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
                break;
            }
            SemanticType foundType;
            if (!ctx.lookup(name, foundType)) {
                ctx.report("Unknown identifier: " + name);
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            } else {
                semType = foundType;
            }
            break;
        }
        case IOTA:
            semType = SemanticType::makeBase(SemanticType::INT);
            break;
        case LIT_VAL:
            semType = semanticTypeFromValue(value);
            break;
        case ARRAY_LIT: {
            SemanticType elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
            if (arrayElemType) {
                elemType = arrayElemType->getSemanticType();
            } else if (arrayElems && arrayElems->getExprList() && !arrayElems->getExprList()->empty()) {
                auto it = arrayElems->getExprList()->begin();
                elemType = (*it)->semantics(ctx);
                for (; it != arrayElems->getExprList()->end(); ++it) {
                    SemanticType t = (*it)->semantics(ctx);
                    if (!elemType.sameKind(t)) {
                        ctx.report("Array literal has inconsistent element types.");
                        elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
                        break;
                    }
                }
            }

            if (arrayLen) {
                SemanticType lenType = arrayLen->semantics(ctx);
                if (!lenType.isError && lenType.base != SemanticType::INT) {
                    ctx.report("Array length must be int.");
                }
                if (arrayElems && arrayElems->getExprList() && arrayLen->getType() == LIT_VAL) {
                    ValueNode *lenVal = arrayLen->getLiteral();
                    if (lenVal && lenVal->getValueType() == ValueNode::LIT_INT) {
                        size_t elemCount = arrayElems->getExprList()->size();
                        if (elemCount > static_cast<size_t>(lenVal->getInt())) {
                            ctx.report("Array literal has more elements than its length.");
                        }
                    }
                }
            }

            if (elemType.base == SemanticType::UNKNOWN) {
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            } else {
                semType = elemType;
                semType.arrayDims += 1;
            }
            break;
        }
        case SLICE_LIT: {
            SemanticType elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
            if (arrayElemType) {
                elemType = arrayElemType->getSemanticType();
            } else if (arrayElems && arrayElems->getExprList() && !arrayElems->getExprList()->empty()) {
                elemType = arrayElems->getExprList()->front()->semantics(ctx);
            }
            semType = elemType;
            semType.sliceDims += 1;
            break;
        }
        case SUMMARY:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
        case MODULO: {
            SemanticType leftType = left ? left->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            SemanticType rightType = right ? right->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (type == SUMMARY && leftType.isString() && rightType.isString()) {
                semType = SemanticType::makeBase(SemanticType::STRING);
                break;
            }
            if (!leftType.isNumeric() || !rightType.isNumeric() || !leftType.sameKind(rightType)) {
                ctx.report("Numeric operator requires operands of the same numeric type.");
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
                break;
            }
            semType = leftType;
            break;
        }
        case EQUAL:
        case NOT_EQUAL:
        case LESS:
        case GREATER:
        case LESS_OR_EQUAL:
        case GREATER_OR_EQUAL: {
            SemanticType leftType = left ? left->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            SemanticType rightType = right ? right->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!leftType.sameKind(rightType) || !leftType.isScalar()) {
                ctx.report("Comparison requires scalar operands of the same type.");
            }
            semType = SemanticType::makeBase(SemanticType::BOOL);
            break;
        }
        case AND:
        case OR: {
            SemanticType leftType = left ? left->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            SemanticType rightType = right ? right->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!leftType.isBool() || !rightType.isBool()) {
                ctx.report("Logical operator requires boolean operands.");
            }
            semType = SemanticType::makeBase(SemanticType::BOOL);
            break;
        }
        case NOT: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!operandType.isBool()) {
                ctx.report("Logical NOT requires boolean operand.");
            }
            semType = SemanticType::makeBase(SemanticType::BOOL);
            break;
        }
        case UNARY_MINUS: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!operandType.isNumeric()) {
                ctx.report("Unary minus requires numeric operand.");
            }
            semType = operandType;
            break;
        }
        case ADDRESS_OF:
            semType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            break;
        case ELEMENT_ACCESS: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            SemanticType indexType = index ? index->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (indexType.base != SemanticType::INT || !indexType.isScalar()) {
                ctx.report("Index expression must be int.");
            }
            if (operandType.arrayDims > 0) {
                semType = operandType;
                semType.arrayDims -= 1;
            } else if (operandType.sliceDims > 0) {
                semType = operandType;
                semType.sliceDims -= 1;
            } else if (operandType.isString() && operandType.isScalar()) {
                semType = SemanticType::makeBase(SemanticType::INT);
            } else {
                ctx.report("Indexing requires array or slice operand.");
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            }
            break;
        }
        case SLICE: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            semType = operandType;
            if (operandType.arrayDims > 0) {
                semType.arrayDims -= 1;
                semType.sliceDims = 1;
            }
            break;
        }
        case FUNCTION_CALL:
        case SELECTOR:
        case EXPR_IN_BRACKETS:
        default:
            semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            if (operand) {
                operand->semantics(ctx);
            }
            break;
    }

    return semType;
}

string ExprNode::getDotLabel() const {
    switch (type) {
        case ID:                return "IDENTIFIER";
        case IOTA:              return "iota";
        case EXPR_IN_BRACKETS:  return "()";
        case LIT_VAL:           return "LIT_VAL";
        case ARRAY_LIT:
            return arrayLenAuto ? "ARRAY_LIT_AUTO" : "ARRAY_LIT";
        case SLICE_LIT:         return "SLICE_LIT";
        case SUMMARY:           return "+";
        case SUBTRACTION:       return "-";
        case MULTIPLICATION:    return "*";
        case DIVISION:          return "/";
        case MODULO:            return "%";
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
        case ADDRESS_OF:        return "&";
        case ELEMENT_ACCESS:    return "[i]";
        case SELECTOR:          return ".";
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
    appendDotEdge(result, arrayElemType, "elem_type");
    appendDotEdge(result, arrayLen, "len");
    appendDotEdge(result, arrayElems, "elems");

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
    arrayElemType = nullptr;
    arrayLen = nullptr;
    arrayElems = nullptr;
    arrayLenAuto = false;
    semType = SemanticType::makeBase(SemanticType::UNKNOWN);
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

void ExprListNode::semantics(SemanticContext &ctx) {
    if (!exprs) {
        return;
    }
    for (ExprNode *expr : *exprs) {
        if (expr) {
            expr->semantics(ctx);
        }
    }
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
        case BLOCK:         return "BLOCK";
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
    appendDotEdge(result, initStmt, "init_stmt");
    appendDotEdge(result, condition, "condition");
    appendDotEdge(result, postStmt, "post_stmt");
    appendDotEdge(result, thenBranch, "then_branch");
    appendDotEdge(result, elseBranch, "else_branch");
    appendDotEdge(result, body, "body");
    appendDotEdge(result, caseList, "case_list");

    return result;
}

void StmtNode::semantics(SemanticContext &ctx) {
    switch (type) {
        case DECLARATION:
            if (decl) decl->semantics(ctx);
            break;
        case SIMPLE:
            if (simpleStmt) simpleStmt->semantics(ctx);
            break;
        case RETURN:
            if (const auto *retInfo = ctx.currentReturn()) {
                list<ExprNode*> *exprs = exprList ? exprList->getExprList() : nullptr;
                vector<SemanticType> exprTypes;
                if (exprs) {
                    for (ExprNode *expr : *exprs) {
                        exprTypes.push_back(expr ? expr->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN));
                    }
                }
                size_t exprCount = exprTypes.size();
                size_t retCount = retInfo->types.size();
                if (retCount == 0) {
                    if (exprCount > 0) {
                        ctx.report("Return values not allowed in function with no return type.");
                    }
                } else if (exprCount == 0) {
                    if (!retInfo->allowBareReturn) {
                        ctx.report("Return values required.");
                    }
                } else {
                    if (exprCount != retCount) {
                        ctx.report("Return value count mismatch.");
                    }
                    for (size_t i = 0; i < exprCount && i < retCount; ++i) {
                        if (!isAssignable(retInfo->types[i], exprTypes[i])) {
                            ctx.report("Return type mismatch.");
                        }
                    }
                }
            } else {
                if (exprList) exprList->semantics(ctx);
            }
            break;
        case BREAK:
            if (!ctx.inLoop() && !ctx.inSwitch()) {
                ctx.report("Break not within loop or switch.");
            }
            break;
        case CONTINUE:
            if (!ctx.inLoop()) {
                ctx.report("Continue not within loop.");
            }
            break;
        case BLOCK:
            ctx.enterScope();
            if (stmtList && stmtList->getStmtList()) {
                for (StmtNode *stmt : *stmtList->getStmtList()) {
                    if (stmt) stmt->semantics(ctx);
                }
            }
            ctx.exitScope();
            break;
        case IF:
            ctx.enterScope();
            if (simpleStmt) simpleStmt->semantics(ctx);
            if (condition) {
                SemanticType condType = condition->semantics(ctx);
                if (!condType.isBool()) {
                    ctx.report("If condition must be bool.");
                }
            }
            if (thenBranch) thenBranch->semantics(ctx);
            if (elseBranch) elseBranch->semantics(ctx);
            ctx.exitScope();
            break;
        case SWITCH:
            {
                ctx.enterScope();
                if (simpleStmt) simpleStmt->semantics(ctx);
                SemanticType switchType = SemanticType::makeBase(SemanticType::BOOL);
                if (condition) {
                    switchType = condition->semantics(ctx);
                }
                ctx.enterSwitch(switchType);
                if (caseList) caseList->semantics(ctx);
                ctx.exitSwitch();
                ctx.exitScope();
            }
            break;
        case FOR:
            ctx.enterScope();
            ctx.enterLoop();
            if (condition) {
                SemanticType condType = condition->semantics(ctx);
                if (!condType.isBool()) {
                    ctx.report("For condition must be bool.");
                }
            }
            if (body) body->semantics(ctx);
            ctx.exitLoop();
            ctx.exitScope();
            break;
        case FOR_PARAM:
            ctx.enterScope();
            ctx.enterLoop();
            if (initStmt) initStmt->semantics(ctx);
            if (condition) {
                SemanticType condType = condition->semantics(ctx);
                if (!condType.isBool()) {
                    ctx.report("For condition must be bool.");
                }
            }
            if (postStmt) postStmt->semantics(ctx);
            if (body) body->semantics(ctx);
            ctx.exitLoop();
            ctx.exitScope();
            break;
        case FOR_RANGE:
            ctx.enterScope();
            ctx.enterLoop();
            {
                SemanticType rangeType = condition ? condition->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
                SemanticType indexType = SemanticType::makeBase(SemanticType::UNKNOWN);
                SemanticType valueType = SemanticType::makeBase(SemanticType::UNKNOWN);
                if (!getRangeTypes(rangeType, indexType, valueType)) {
                    ctx.report("Range requires array, slice, or string.");
                }
                if (exprList && exprList->getExprList()) {
                    auto &targets = *exprList->getExprList();
                    size_t count = targets.size();
                    if (count > 2) {
                        ctx.report("Range assignment requires at most two variables.");
                    }
                    auto it = targets.begin();
                    if (count >= 1 && it != targets.end()) {
                        checkRangeTarget(*it, indexType, ctx);
                        ++it;
                    }
                    if (count >= 2 && it != targets.end()) {
                        checkRangeTarget(*it, valueType, ctx);
                    }
                }
            }
            if (body) body->semantics(ctx);
            ctx.exitLoop();
            ctx.exitScope();
            break;
        default:
            break;
    }
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

void CaseNode::semantics(SemanticContext &ctx) {
    if (exprList) {
        if (const SemanticType *switchType = ctx.currentSwitchType()) {
            if (exprList->getExprList()) {
                for (ExprNode *expr : *exprList->getExprList()) {
                    SemanticType exprType = expr ? expr->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
                    if (switchType->base != SemanticType::UNKNOWN && !isAssignable(*switchType, exprType)) {
                        ctx.report("Case type does not match switch expression type.");
                    }
                }
            }
        } else {
            exprList->semantics(ctx);
        }
    }
    if (stmtList && stmtList->getStmtList()) {
        for (StmtNode *stmt : *stmtList->getStmtList()) {
            if (stmt) stmt->semantics(ctx);
        }
    }
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

void CaseListNode::semantics(SemanticContext &ctx) {
    if (!caseList) {
        return;
    }
    for (CaseNode *caseElem : *caseList) {
        if (caseElem) caseElem->semantics(ctx);
    }
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

SimpleStmtNode* SimpleStmtNode::createAddAssign(ExprListNode *left, ExprListNode *right) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = ADD_ASSIGN;
    node->left = left;
    node->right = right;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createSubAssign(ExprListNode *left, ExprListNode *right) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = SUB_ASSIGN;
    node->left = left;
    node->right = right;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createMulAssign(ExprListNode *left, ExprListNode *right) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = MUL_ASSIGN;
    node->left = left;
    node->right = right;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createDivAssign(ExprListNode *left, ExprListNode *right) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = DIV_ASSIGN;
    node->left = left;
    node->right = right;
    return node;
}

SimpleStmtNode* SimpleStmtNode::createModAssign(ExprListNode *left, ExprListNode *right) {
    SimpleStmtNode *node = new SimpleStmtNode();
    node->type = MOD_ASSIGN;
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
        case ADD_ASSIGN:        return "ADD_ASSIGN_STMT";
        case SUB_ASSIGN:        return "SUB_ASSIGN_STMT";
        case MUL_ASSIGN:        return "MUL_ASSIGN_STMT";
        case DIV_ASSIGN:        return "DIV_ASSIGN_STMT";
        case MOD_ASSIGN:        return "MOD_ASSIGN_STMT";
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

void SimpleStmtNode::semantics(SemanticContext &ctx) {
    switch (type) {
        case EXPR:
            if (expr) expr->semantics(ctx);
            break;
        case INC:
        case DEC: {
            string idName;
            if (isIdentifierExpr(expr, &idName) && idName == "_") {
                ctx.report("Increment/decrement cannot use blank identifier.");
                break;
            }
            if (!isAddressableExpr(expr, ctx)) {
                ctx.report("Increment/decrement requires addressable operand.");
                break;
            }
            SemanticType exprType = expr ? expr->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!exprType.isNumeric()) {
                ctx.report("Increment/decrement requires numeric expression.");
            }
            break;
        }
        case ASSIGN:
        case ADD_ASSIGN:
        case SUB_ASSIGN:
        case MUL_ASSIGN:
        case DIV_ASSIGN:
        case MOD_ASSIGN:
        case SHORT_VAR_DECL: {
            if (!left || !right || !left->getExprList() || !right->getExprList()) {
                break;
            }
            auto &leftExprs = *left->getExprList();
            auto &rightExprs = *right->getExprList();
            if (leftExprs.size() != rightExprs.size()) {
                ctx.report("Assignment list size mismatch.");
            }
            auto itLeft = leftExprs.begin();
            auto itRight = rightExprs.begin();
            int newCount = 0;
            for (; itLeft != leftExprs.end() && itRight != rightExprs.end(); ++itLeft, ++itRight) {
                ExprNode *leftExpr = *itLeft;
                ExprNode *rightExpr = *itRight;
                SemanticType rightType = rightExpr ? rightExpr->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);

                string idName;
                bool isId = isIdentifierExpr(leftExpr, &idName);
                if (type == SHORT_VAR_DECL) {
                    if (!isId) {
                        ctx.report("Short variable declaration requires identifiers.");
                        continue;
                    }
                    if (idName == "_") {
                        continue;
                    }
                    if (!ctx.isDeclaredInCurrent(idName)) {
                        ctx.declare(idName, rightType);
                        newCount++;
                    } else {
                        SemanticType existing;
                        if (ctx.lookup(idName, existing) && !isAssignable(existing, rightType)) {
                            ctx.report("Cannot assign to " + idName + " from " + rightType.toString());
                        }
                    }
                    continue;
                }

                if (!isAddressableExpr(leftExpr, ctx)) {
                    if (leftExpr) {
                        leftExpr->semantics(ctx);
                    }
                    ctx.report("Left side of assignment must be addressable.");
                    continue;
                }

                if (isId && idName == "_") {
                    if (type != ASSIGN) {
                        ctx.report("Blank identifier cannot be used in compound assignment.");
                    }
                    continue;
                }

                SemanticType leftType = SemanticType::makeBase(SemanticType::UNKNOWN);
                if (isId) {
                    SemanticType found;
                    if (!ctx.lookup(idName, found)) {
                        ctx.report("Assignment to undeclared identifier: " + idName);
                    } else {
                        leftType = found;
                    }
                } else if (leftExpr) {
                    leftType = leftExpr->semantics(ctx);
                }

                if (type == ASSIGN) {
                    if (leftType.base != SemanticType::UNKNOWN && !isAssignable(leftType, rightType)) {
                        if (isId) {
                            ctx.report("Type mismatch in assignment to " + idName);
                        } else {
                            ctx.report("Type mismatch in assignment.");
                        }
                    }
                    continue;
                }

                if (type == ADD_ASSIGN) {
                    bool okString = leftType.isString() && rightType.isString();
                    bool okNumeric = leftType.isNumeric() && rightType.isNumeric() && leftType.sameKind(rightType);
                    if (!okString && !okNumeric) {
                        ctx.report("Add assignment requires matching string or numeric operands.");
                    }
                } else {
                    if (!leftType.isNumeric() || !rightType.isNumeric() || !leftType.sameKind(rightType)) {
                        ctx.report("Compound assignment requires matching numeric operands.");
                    }
                }
            }
            if (type == SHORT_VAR_DECL && newCount == 0) {
                ctx.report("Short variable declaration requires at least one new variable.");
            }
            break;
        }
        default:
            break;
    }
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
        appendDotEdge(res, elemType, "elem_type");
        break;
    case SLICE:
        appendDotEdge(res, elemType, "elem_type");
        break;
    case FUNC:
        appendDotEdge(res, signature, "signature");
        break;
    default:
        break;
    }

    return res;
}

SemanticType TypeNode::getSemanticType() const {
    switch (kind) {
        case NAMED: {
            if (!name) {
                return SemanticType::makeBase(SemanticType::UNKNOWN);
            }
            switch (name->getType()) {
                case TypeNameNode::INT_64: return SemanticType::makeBase(SemanticType::INT);
                case TypeNameNode::FLOAT_64: return SemanticType::makeBase(SemanticType::FLOAT);
                case TypeNameNode::BOOL: return SemanticType::makeBase(SemanticType::BOOL);
                case TypeNameNode::STRING: return SemanticType::makeBase(SemanticType::STRING);
                case TypeNameNode::RUNE: return SemanticType::makeBase(SemanticType::RUNE);
                default: return SemanticType::makeBase(SemanticType::UNKNOWN);
            }
        }
        case ARRAY: {
            SemanticType elem = elemType ? elemType->getSemanticType() : SemanticType::makeBase(SemanticType::UNKNOWN);
            elem.arrayDims += 1;
            return elem;
        }
        case SLICE: {
            SemanticType elem = elemType ? elemType->getSemanticType() : SemanticType::makeBase(SemanticType::UNKNOWN);
            elem.sliceDims += 1;
            return elem;
        }
        default:
            return SemanticType::makeBase(SemanticType::UNKNOWN);
    }
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

void ParamDeclNode::semantics(SemanticContext &ctx) {
    if (!type) {
        return;
    }
    SemanticType paramType = type->getSemanticType();
    if (!idList || !idList->getIdList()) {
        return;
    }
    for (ValueNode *id : *idList->getIdList()) {
        if (isBlankIdentifier(id)) {
            continue;
        }
        if (id && id->getString()) {
            ctx.declare(*id->getString(), paramType);
        }
    }
}

IdListNode* ParamDeclNode::getIdList() const {
    return idList;
}

TypeNode* ParamDeclNode::getType() const {
    return type;
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

void ParamDeclListNode::semantics(SemanticContext &ctx) {
    if (!paramList) {
        return;
    }
    for (ParamDeclNode *param : *paramList) {
        if (param) param->semantics(ctx);
    }
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

void SignatureNode::semantics(SemanticContext &ctx) {
    if (paramList) paramList->semantics(ctx);
    if (result) result->semantics(ctx);
}

ResultNode* SignatureNode::getResult() const {
    return result;
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

void ResultNode::semantics(SemanticContext &ctx) {
    if (paramList) paramList->semantics(ctx);
    if (type) type->getSemanticType();
}

ParamDeclListNode* ResultNode::getParamList() const {
    return paramList;
}

TypeNode* ResultNode::getType() const {
    return type;
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

void VarSpecNode::semantics(SemanticContext &ctx) {
    if (!idList || !idList->getIdList()) {
        return;
    }
    SemanticType declaredType = type ? type->getSemanticType() : SemanticType::makeBase(SemanticType::UNKNOWN);
    list<ExprNode*> *exprs = exprList ? exprList->getExprList() : nullptr;
    size_t exprCount = exprs ? exprs->size() : 0;
    size_t idCount = idList->getIdList()->size();
    if (exprs && exprCount != idCount) {
        ctx.report("VarSpec: initializer count does not match identifiers.");
    }
    auto idIt = idList->getIdList()->begin();
    auto exprIt = exprs ? exprs->begin() : list<ExprNode*>::iterator();
    for (; idIt != idList->getIdList()->end(); ++idIt) {
        ValueNode *id = *idIt;
        if (isBlankIdentifier(id)) {
            if (exprs && exprIt != exprs->end()) {
                (*exprIt)->semantics(ctx);
                ++exprIt;
            }
            continue;
        }
        SemanticType initType = declaredType;
        if (exprs && exprIt != exprs->end()) {
            SemanticType exprType = (*exprIt)->semantics(ctx);
            ++exprIt;
            if (declaredType.base != SemanticType::UNKNOWN && !isAssignable(declaredType, exprType)) {
                ctx.report("VarSpec: type mismatch for initializer.");
            }
            if (declaredType.base == SemanticType::UNKNOWN) {
                initType = exprType;
            }
        } else if (declaredType.base == SemanticType::UNKNOWN) {
            ctx.report("VarSpec: missing type and initializer.");
            initType = SemanticType::makeBase(SemanticType::UNKNOWN);
        }
        if (id && id->getString()) {
            ctx.declare(*id->getString(), initType);
        }
    }
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

void VarSpecListNode::semantics(SemanticContext &ctx) {
    if (!varList) {
        return;
    }
    for (VarSpecNode *varElem : *varList) {
        if (varElem) varElem->semantics(ctx);
    }
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

void ConstSpecNode::semantics(SemanticContext &ctx) {
    if (!idList || !idList->getIdList()) {
        return;
    }
    SemanticType declaredType = type ? type->getSemanticType() : SemanticType::makeBase(SemanticType::UNKNOWN);
    list<ExprNode*> *exprs = exprList ? exprList->getExprList() : nullptr;
    size_t exprCount = exprs ? exprs->size() : 0;
    size_t idCount = idList->getIdList()->size();
    if (exprs && exprCount != idCount) {
        ctx.report("ConstSpec: initializer count does not match identifiers.");
    }
    auto idIt = idList->getIdList()->begin();
    auto exprIt = exprs ? exprs->begin() : list<ExprNode*>::iterator();
    for (; idIt != idList->getIdList()->end(); ++idIt) {
        ValueNode *id = *idIt;
        if (isBlankIdentifier(id)) {
            if (exprs && exprIt != exprs->end()) {
                (*exprIt)->semantics(ctx);
                ++exprIt;
            }
            continue;
        }
        SemanticType initType = declaredType;
        if (exprs && exprIt != exprs->end()) {
            SemanticType exprType = (*exprIt)->semantics(ctx);
            ++exprIt;
            if (declaredType.base != SemanticType::UNKNOWN && !isAssignable(declaredType, exprType)) {
                ctx.report("ConstSpec: type mismatch for initializer.");
            }
            if (declaredType.base == SemanticType::UNKNOWN) {
                initType = exprType;
            }
        } else if (declaredType.base == SemanticType::UNKNOWN) {
            ctx.report("ConstSpec: missing type and initializer.");
            initType = SemanticType::makeBase(SemanticType::UNKNOWN);
        }
        if (id && id->getString()) {
            ctx.declare(*id->getString(), initType);
        }
    }
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

void ConstSpecListNode::semantics(SemanticContext &ctx) {
    if (!specList) {
        return;
    }
    for (ConstSpecNode *spec : *specList) {
        if (spec) spec->semantics(ctx);
    }
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

void DeclNode::semantics(SemanticContext &ctx) {
    if (constSpecList) {
        constSpecList->semantics(ctx);
    }
    if (varSpecList) {
        varSpecList->semantics(ctx);
    }
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

void FuncDeclNode::semantics(SemanticContext &ctx) {
    SemanticContext local = ctx;
    local.enterScope();
    if (signature) signature->semantics(local);
    SemanticContext::FunctionReturnInfo retInfo;
    if (signature) {
        retInfo.types = collectResultTypes(signature->getResult(), retInfo.allowBareReturn);
    }
    local.enterFunction(retInfo);
    if (body) body->semantics(local);
    local.exitFunction();
    ctx.errors.insert(ctx.errors.end(), local.errors.begin(), local.errors.end());
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

void TopLevelDeclNode::semantics(SemanticContext &ctx) {
    if (decl) decl->semantics(ctx);
    if (funcDecl) funcDecl->semantics(ctx);
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

void TopLevelDeclListNode::semantics(SemanticContext &ctx) {
    if (!elemList) {
        return;
    }
    for (TopLevelDeclNode *elem : *elemList) {
        if (elem) elem->semantics(ctx);
    }
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

void ImportSpecNode::semantics(SemanticContext &ctx) {
    (void)ctx;
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

void ImportSpecListNode::semantics(SemanticContext &ctx) {
    if (!elemList) {
        return;
    }
    for (ImportSpecNode *spec : *elemList) {
        if (spec) spec->semantics(ctx);
    }
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

void ImportDeclNode::semantics(SemanticContext &ctx) {
    if (importList) importList->semantics(ctx);
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

void ImportDeclListNode::semantics(SemanticContext &ctx) {
    if (!elemList) {
        return;
    }
    for (ImportDeclNode *decl : *elemList) {
        if (decl) decl->semantics(ctx);
    }
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

void ProgramNode::semantics(SemanticContext &ctx) {
    if (importDeclList) importDeclList->semantics(ctx);
    if (topLevelDeclList) topLevelDeclList->semantics(ctx);
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

TypeNameNode::PredefinedTypes TypeNameNode::getType() const {
    return type;
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
    auto escapeString = [](const string &src) {
        string out;
        out.reserve(src.size());
        for (char c : src) {
            switch (c) {
                case '\\': out += "\\\\"; break;
                case '\"': out += "\\\""; break;
                case '\n': out += "\\n"; break;
                case '\t': out += "\\t"; break;
                default: out.push_back(c); break;
            }
        }
        return out;
    };
    auto encodeRuneUtf8 = [](unsigned int codepoint) {
        string out;
        if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
            return out;
        }
        if (codepoint <= 0x7F) {
            out.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7FF) {
            out.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint <= 0xFFFF) {
            out.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        return out;
    };

    switch (valueType) {
        case LIT_INT:       return "int64: " + to_string(intValue);
        case LIT_FLOAT:     return "float64: " + to_string(floatValue);
        case LIT_RUNE: {
            string runeStr = encodeRuneUtf8(static_cast<unsigned int>(intValue));
            if (runeStr.empty()) {
                return "rune: " + to_string(intValue);
            }
            return "rune: " + escapeString(runeStr);
        }
        case LIT_STRING:    return "string: " + escapeString(*stringValue);
        case LIT_BOOL:      return string("bool: ") + (boolValue ? "true" : "false");
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
