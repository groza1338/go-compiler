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

static void addOuterArrayDim(SemanticType &type, int length) {
    type.arrayDims += 1;
    type.arrayLengths.insert(type.arrayLengths.begin(), length);
}

static void dropOuterArrayDim(SemanticType &type) {
    if (type.arrayDims <= 0) {
        return;
    }
    type.arrayDims -= 1;
    if (!type.arrayLengths.empty()) {
        type.arrayLengths.erase(type.arrayLengths.begin());
    }
}

static bool isOrderable(const SemanticType &type) {
    return type.isScalar() && (type.base == SemanticType::INT
        || type.base == SemanticType::FLOAT
        || type.base == SemanticType::RUNE
        || type.base == SemanticType::STRING);
}

static bool isComparableType(const SemanticType &type) {
    if (type.isError || type.base == SemanticType::UNKNOWN) {
        return true;
    }
    return type.sliceDims == 0;
}

static string comparisonTypeName(const SemanticType &type) {
    if (type.arrayDims > 0) {
        return "array";
    }
    if (type.sliceDims > 0) {
        return "slice";
    }
    return type.toString();
}

static string formatMultiValueTypeList(const vector<SemanticType> &types) {
    string out;
    for (size_t i = 0; i < types.size(); ++i) {
        if (i > 0) {
            out += ", ";
        }
        out += types[i].toString();
    }
    return out;
}

static string formatTypeList(const vector<SemanticType> &types) {
    return "(" + formatMultiValueTypeList(types) + ")";
}

static string quoteGoStringLiteral(const string &value) {
    string out = "\"";
    for (char c : value) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '\"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            default: out.push_back(c); break;
        }
    }
    out += "\"";
    return out;
}

static bool getLiteralTextAndType(ExprNode *expr, string &literalText, string &literalType) {
    if (!expr || expr->getType() != ExprNode::LIT_VAL) {
        return false;
    }
    ValueNode *lit = expr->getLiteral();
    if (!lit) {
        return false;
    }
    switch (lit->getValueType()) {
        case ValueNode::LIT_BOOL:
            literalText = lit->getBool() ? "true" : "false";
            literalType = "untyped bool constant";
            break;
        case ValueNode::LIT_STRING:
            if (lit->getString()) {
                literalText = quoteGoStringLiteral(*lit->getString());
            } else {
                literalText = "\"\"";
            }
            literalType = "untyped string constant";
            break;
        case ValueNode::LIT_INT:
            literalText = to_string(lit->getInt());
            literalType = "untyped int constant";
            break;
        case ValueNode::LIT_FLOAT:
            literalText = to_string(lit->getFloat());
            literalType = "untyped float constant";
            break;
        case ValueNode::LIT_RUNE:
            literalText = to_string(lit->getRune());
            literalType = "untyped rune constant";
            break;
        default:
            return false;
    }
    return true;
}

static bool buildLiteralReturnMismatch(ExprNode *expr, const SemanticType &expected, string &outMsg) {
    string literalText;
    string literalType;
    if (!getLiteralTextAndType(expr, literalText, literalType)) {
        return false;
    }
    outMsg = "cannot use " + literalText + " (" + literalType + ") as " + expected.toString()
        + " value in return statement";
    return true;
}

static bool buildLiteralConvertMismatch(ExprNode *expr, const SemanticType &target, string &outMsg) {
    string literalText;
    string literalType;
    if (!getLiteralTextAndType(expr, literalText, literalType)) {
        return false;
    }
    outMsg = "cannot convert " + literalText + " (" + literalType + ") to type " + target.toString();
    return true;
}

static string formatExprForGoMessage(ExprNode *expr) {
    if (!expr) {
        return "value";
    }
    string literalText;
    string literalType;
    if (getLiteralTextAndType(expr, literalText, literalType)) {
        return literalText;
    }
    if (expr->getType() == ExprNode::ID) {
        ValueNode *idVal = expr->getIdentifier();
        if (idVal && idVal->getString()) {
            return *idVal->getString();
        }
    }
    return expr->toString();
}

static bool buildReturnMismatch(ExprNode *expr, const SemanticType &actual, const SemanticType &expected, string &outMsg) {
    if (buildLiteralReturnMismatch(expr, expected, outMsg)) {
        return true;
    }
    string exprText = "value";
    if (expr) {
        exprText = expr->toString();
    }
    outMsg = "cannot use " + exprText + " (" + actual.toString() + ") as " + expected.toString()
        + " value in return statement";
    return true;
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
                if (operandType.arrayDims > 0) {
                    return isAddressableExpr(operand, ctx);
                }
                if (operandType.sliceDims > 0) {
                    return true;
                }
            }
            return false;
        case ExprNode::EXPR_IN_BRACKETS:
            return isAddressableExpr(expr->getOperand(), ctx);
        default:
            return false;
    }
}

static bool getFunctionCallResults(ExprNode *expr, SemanticContext &ctx, vector<SemanticType> &results) {
    results.clear();
    if (!expr || expr->getType() != ExprNode::FUNCTION_CALL) {
        return false;
    }
    ExprNode *operand = expr->getOperand();
    if (!operand || operand->getType() != ExprNode::ID) {
        return false;
    }
    ValueNode *idVal = operand->getIdentifier();
    if (!idVal || !idVal->getString()) {
        return false;
    }
    const string &name = *idVal->getString();
    SemanticContext::FunctionInfo fnInfo;
    bool found = ctx.lookupFunction(name, fnInfo);
    if (!found) {
        ctx.report("Unknown function: " + name);
    }

    vector<SemanticType> argTypes;
    if (ExprListNode *args = expr->getArgs()) {
        if (args->getExprList()) {
            for (ExprNode *arg : *args->getExprList()) {
                argTypes.push_back(arg ? arg->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN));
            }
        }
    }

    if (found) {
        if (argTypes.size() != fnInfo.params.size()) {
            string haveTypes = formatTypeList(argTypes);
            string wantTypes = formatTypeList(fnInfo.params);
            if (argTypes.size() > fnInfo.params.size()) {
                ctx.report("too many arguments in call to " + name + "\n\thave " + haveTypes + "\n\twant " + wantTypes);
            } else {
                ctx.report("not enough arguments in call to " + name + "\n\thave " + haveTypes + "\n\twant " + wantTypes);
            }
        } else {
            for (size_t i = 0; i < argTypes.size(); ++i) {
                if (!isAssignable(fnInfo.params[i], argTypes[i])) {
                    ctx.report("Function call argument type mismatch.");
                }
            }
        }
        results = fnInfo.results;
    }
    return found;
}

static bool checkPackageCallResults(ExprNode *expr, SemanticContext &ctx, vector<SemanticType> &results);

static void collectExprListTypes(ExprListNode *exprList, SemanticContext &ctx, vector<SemanticType> &outTypes) {
    outTypes.clear();
    if (!exprList || !exprList->getExprList()) {
        return;
    }
    auto &exprs = *exprList->getExprList();
    if (exprs.size() == 1) {
        ExprNode *expr = exprs.front();
        vector<SemanticType> results;
        if ((getFunctionCallResults(expr, ctx, results) || checkPackageCallResults(expr, ctx, results))
            && results.size() > 1) {
            outTypes = results;
            return;
        }
    }
    for (ExprNode *expr : exprs) {
        outTypes.push_back(expr ? expr->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN));
    }
}

static bool getRangeTypes(const SemanticType &rangeType, SemanticType &indexType, SemanticType &valueType) {
    indexType = SemanticType::makeBase(SemanticType::INT);
    if (rangeType.base == SemanticType::INT && rangeType.isScalar()) {
        valueType = SemanticType::makeBase(SemanticType::INT);
        return true;
    }
    if (rangeType.base == SemanticType::STRING && rangeType.isScalar()) {
        valueType = SemanticType::makeBase(SemanticType::RUNE);
        return true;
    }
    if (rangeType.arrayDims > 0) {
        valueType = rangeType;
        dropOuterArrayDim(valueType);
        return true;
    }
    if (rangeType.sliceDims > 0) {
        valueType = rangeType;
        valueType.sliceDims -= 1;
        return true;
    }
    return false;
}

static bool checkPackageCallResults(ExprNode *expr, SemanticContext &ctx, vector<SemanticType> &results) {
    results.clear();
    if (!expr || expr->getType() != ExprNode::FUNCTION_CALL) {
        return false;
    }
    ExprNode *callee = expr->getOperand();
    if (!callee || callee->getType() != ExprNode::SELECTOR) {
        return false;
    }
    ExprNode *pkgExpr = callee->getOperand();
    ValueNode *funcNameVal = callee->getIdentifier();
    if (!pkgExpr || pkgExpr->getType() != ExprNode::ID || !funcNameVal || !funcNameVal->getString()) {
        return false;
    }
    ValueNode *pkgNameVal = pkgExpr->getIdentifier();
    if (!pkgNameVal || !pkgNameVal->getString()) {
        return false;
    }
    const string &pkgName = *pkgNameVal->getString();
    const string &funcName = *funcNameVal->getString();
    string target = ctx.getImportTarget(pkgName);
    if (target.empty()) {
        return false;
    }

    if (target == "fmt") {
        if (funcName == "Print" || funcName == "Scan") {
            results = {SemanticType::makeBase(SemanticType::INT), SemanticType::makeBase(SemanticType::UNKNOWN)};
            return true;
        }
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
        if (!isId) {
            ctx.report("no new variables on left side of :=");
            string declTarget = leftExpr->toString();
            if (leftExpr->getType() == ExprNode::LIT_VAL) {
                ValueNode *lit = leftExpr->getLiteral();
                if (lit) {
                    if (lit->getValueType() == ValueNode::LIT_STRING && lit->getString()) {
                        declTarget = quoteGoStringLiteral(*lit->getString());
                    } else if (lit->getValueString()) {
                        declTarget = *lit->getValueString();
                    }
                }
            }
            ctx.report("invalid syntax tree: cannot declare " + declTarget);
        } else {
            ctx.report("Range assignment requires addressable identifiers.");
        }
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

static vector<SemanticType> collectParamTypes(ParamDeclListNode *paramList) {
    vector<SemanticType> types;
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

static string stripStringLiteral(const string &text) {
    if (text.size() < 2) {
        return text;
    }
    char first = text.front();
    char last = text.back();
    if ((first == '"' && last == '"') || (first == '`' && last == '`')) {
        return text.substr(1, text.size() - 2);
    }
    return text;
}

static string baseImportName(const string &pathLiteral) {
    string path = stripStringLiteral(pathLiteral);
    size_t pos = path.find_last_of('/');
    if (pos == string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

static SemanticType evalCompositeLiteralWithExpected(ExprNode *expr, const SemanticType &expected, SemanticContext &ctx) {
    SemanticType elemExpected = expected;
    if (elemExpected.arrayDims > 0) {
        dropOuterArrayDim(elemExpected);
    } else if (elemExpected.sliceDims > 0) {
        elemExpected.sliceDims -= 1;
    } else {
        return expected;
    }
    ExprListNode *elems = expr->getArrayElems();
    if (!elems || !elems->getExprList()) {
        return expected;
    }
    if (expected.arrayDims > 0 && !expected.arrayLengths.empty()) {
        int expectedLen = expected.arrayLengths.front();
        if (expectedLen >= 0 && elems->getExprList()->size() > static_cast<size_t>(expectedLen)) {
            ctx.report("Composite literal has more elements than expected array length.");
        }
    }
    for (ExprNode *elem : *elems->getExprList()) {
        if (!elem) {
            continue;
        }
        SemanticType elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
        if (elem->getType() == ExprNode::COMPOSITE_LIT) {
            elemType = evalCompositeLiteralWithExpected(elem, elemExpected, ctx);
        } else {
            elemType = elem->semantics(ctx);
        }
        if (!isAssignable(elemExpected, elemType)) {
            ctx.report("Composite literal element type mismatch.");
        }
    }
    return expected;
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

SemanticType SemanticType::makeBase(BaseType baseType) {
    SemanticType t;
    t.base = baseType;
    return t;
}

SemanticType SemanticType::makeError() {
    SemanticType t;
    t.isError = true;
    return t;
}

bool SemanticType::sameKind(const SemanticType &other) const {
    if (base != other.base || arrayDims != other.arrayDims || sliceDims != other.sliceDims) {
        return false;
    }
    size_t lenCount = min(arrayLengths.size(), other.arrayLengths.size());
    for (size_t i = 0; i < lenCount; ++i) {
        int leftLen = arrayLengths[i];
        int rightLen = other.arrayLengths[i];
        if (leftLen >= 0 && rightLen >= 0 && leftLen != rightLen) {
            return false;
        }
    }
    return true;
}

string SemanticType::toString() const {
    if (isError) return "error";
    string baseStr;
    switch (base) {
        case INT: baseStr = "int"; break;
        case FLOAT: baseStr = "float64"; break;
        case BOOL: baseStr = "bool"; break;
        case STRING: baseStr = "string"; break;
        case RUNE: baseStr = "rune"; break;
        default: baseStr = "unknown"; break;
    }
    string prefix;
    for (int i = 0; i < arrayDims; i++) {
        int len = -1;
        if (i < static_cast<int>(arrayLengths.size())) {
            len = arrayLengths[i];
        }
        if (len >= 0) {
            prefix += "[" + to_string(len) + "]";
        } else {
            prefix += "[?]";
        }
    }
    for (int i = 0; i < sliceDims; i++) prefix += "[]";
    return prefix + baseStr;
}

SemanticContext::SemanticContext() {
    scopes.emplace_back();
    usedScopes.emplace_back();
}

void SemanticContext::enterScope() {
    scopes.emplace_back();
    usedScopes.emplace_back();
}

void SemanticContext::exitScope() {
    if (scopes.size() > 1) {
        if (!returnStack.empty()) {
            vector<string> unusedMessages;
            for (const auto &entry : scopes.back()) {
                const string &name = entry.first;
                if (name == "_") {
                    continue;
                }
                if (usedScopes.back().count(name) == 0) {
                    unusedMessages.push_back("declared and not used: " + name);
                }
            }
            if (!unusedMessages.empty()) {
                errors.insert(errors.begin(), unusedMessages.begin(), unusedMessages.end());
            }
        }
        scopes.pop_back();
        usedScopes.pop_back();
    }
}

void SemanticContext::enterConstBlock() {
    inConstBlock = true;
    iotaValue = 0;
    constPrevExprs = nullptr;
}

void SemanticContext::exitConstBlock() {
    inConstBlock = false;
    constPrevExprs = nullptr;
}

bool SemanticContext::isDeclaredInCurrent(const string &name) const {
    return !scopes.empty() && scopes.back().count(name) != 0;
}

bool SemanticContext::lookup(const string &name, SemanticType &out) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            out = found->second;
            return true;
        }
    }
    return false;
}

void SemanticContext::declare(const string &name, const SemanticType &type) {
    if (!scopes.empty()) {
        scopes.back()[name] = type;
    }
}

void SemanticContext::markUsed(const string &name) {
    for (size_t i = scopes.size(); i-- > 0;) {
        if (scopes[i].count(name) != 0) {
            usedScopes[i].insert(name);
            break;
        }
    }
}

void SemanticContext::enterFunction(const FunctionReturnInfo &info) {
    returnStack.push_back(info);
}

void SemanticContext::exitFunction() {
    if (!returnStack.empty()) {
        returnStack.pop_back();
    }
}

const SemanticContext::FunctionReturnInfo * SemanticContext::currentReturn() const {
    if (returnStack.empty()) {
        return nullptr;
    }
    return &returnStack.back();
}

void SemanticContext::enterSwitch(const SemanticType &type, const string &exprText) {
    switchStack.push_back(type);
    switchExprTexts.push_back(exprText);
    switchCaseKeys.emplace_back();
    switchDefaultCounts.push_back(0);
}

void SemanticContext::exitSwitch() {
    if (!switchStack.empty()) {
        switchStack.pop_back();
    }
    if (!switchExprTexts.empty()) {
        switchExprTexts.pop_back();
    }
    if (!switchCaseKeys.empty()) {
        switchCaseKeys.pop_back();
    }
    if (!switchDefaultCounts.empty()) {
        switchDefaultCounts.pop_back();
    }
}

const SemanticType * SemanticContext::currentSwitchType() const {
    if (switchStack.empty()) {
        return nullptr;
    }
    return &switchStack.back();
}

string SemanticContext::currentSwitchExprText() const {
    if (switchExprTexts.empty()) {
        return "";
    }
    return switchExprTexts.back();
}

bool SemanticContext::registerSwitchCase(const string &exprText) {
    if (switchCaseKeys.empty()) {
        return false;
    }
    auto &set = switchCaseKeys.back();
    if (set.count(exprText) != 0) {
        return true;
    }
    set.insert(exprText);
    return false;
}

bool SemanticContext::registerSwitchDefault() {
    if (switchDefaultCounts.empty()) {
        return false;
    }
    int &count = switchDefaultCounts.back();
    count++;
    return count > 1;
}

void SemanticContext::enterLoop() {
    loopDepth++;
}

void SemanticContext::exitLoop() {
    if (loopDepth > 0) {
        loopDepth--;
    }
}

bool SemanticContext::inLoop() const {
    return loopDepth > 0;
}

bool SemanticContext::inSwitch() const {
    return !switchStack.empty();
}

bool SemanticContext::declareFunction(const string &name, const FunctionInfo &info) {
    if (functions.count(name) != 0) {
        report("Duplicate function: " + name);
        return false;
    }
    functions[name] = info;
    return true;
}

bool SemanticContext::lookupFunction(const string &name, FunctionInfo &out) const {
    auto it = functions.find(name);
    if (it == functions.end()) {
        return false;
    }
    out = it->second;
    return true;
}

bool SemanticContext::declareImport(const string &name, const string &target) {
    if (name.empty()) {
        return false;
    }
    if (importNames.count(name) != 0) {
        report("Duplicate import name: " + name);
        return false;
    }
    importNames.insert(name);
    importTargets[name] = target;
    return true;
}

bool SemanticContext::isImportName(const string &name) const {
    return importNames.count(name) != 0;
}

string SemanticContext::getImportTarget(const string &name) const {
    auto it = importTargets.find(name);
    if (it == importTargets.end()) {
        return "";
    }
    return it->second;
}

void SemanticContext::report(const string &message) {
    errors.push_back(message);
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

ExprNode* ExprNode::createCompositeLiteral(ExprListNode *elems) {
    ExprNode *node = new ExprNode();
    node->type = COMPOSITE_LIT;
    node->arrayElems = elems;
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
                ctx.report("cannot use _ as value");
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
                break;
            }
            SemanticType foundType;
            if (!ctx.lookup(name, foundType)) {
                ctx.report("undefined: " + name);
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            } else {
                semType = foundType;
                ctx.markUsed(name);
            }
            break;
        }
        case IOTA:
            if (!ctx.inConstBlock) {
                ctx.report("cannot use iota outside constant declaration");
            }
            semType = SemanticType::makeBase(SemanticType::INT);
            break;
        case LIT_VAL:
            semType = semanticTypeFromValue(value);
            break;
        case COMPOSITE_LIT: {
            SemanticType elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
            size_t elemCount = 0;
            if (arrayElems && arrayElems->getExprList() && !arrayElems->getExprList()->empty()) {
                elemCount = arrayElems->getExprList()->size();
                auto it = arrayElems->getExprList()->begin();
                elemType = (*it)->semantics(ctx);
                for (; it != arrayElems->getExprList()->end(); ++it) {
                    SemanticType t = (*it)->semantics(ctx);
                    if (!elemType.sameKind(t)) {
                        ctx.report("cannot use " + *(*it)->getLiteral()->getValueString() + " (" + t.toString() + ") " +
                            elemType.toString() + " value in composite literal");
                        elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
                        break;
                    }
                }
            }
            if (elemType.base == SemanticType::UNKNOWN) {
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            } else {
                semType = elemType;
                addOuterArrayDim(semType, static_cast<int>(elemCount));
            }
            break;
        }
        case ARRAY_LIT: {
            SemanticType elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
            int lenValue = -1;
            if (arrayElemType) {
                elemType = arrayElemType->getSemanticType();
            } else if (arrayElems && arrayElems->getExprList() && !arrayElems->getExprList()->empty()) {
                auto it = arrayElems->getExprList()->begin();
                if (*it && (*it)->getType() == COMPOSITE_LIT) {
                    elemType = (*it)->semantics(ctx);
                } else {
                    elemType = (*it)->semantics(ctx);
                }
            }

            if (arrayElems && arrayElems->getExprList()) {
                for (ExprNode *elem : *arrayElems->getExprList()) {
                    if (!elem) {
                        continue;
                    }
                    SemanticType t = SemanticType::makeBase(SemanticType::UNKNOWN);
                    if (elem->getType() == COMPOSITE_LIT && elemType.base != SemanticType::UNKNOWN) {
                        t = evalCompositeLiteralWithExpected(elem, elemType, ctx);
                    } else {
                        t = elem->semantics(ctx);
                    }
                    if (elemType.base != SemanticType::UNKNOWN && !elemType.sameKind(t)) {
                        ctx.report("cannot use " + *elem->getLiteral()->getValueString() + " (" + t.toString() + ") " +
                            elemType.toString() + " value in array literal");
                        elemType = SemanticType::makeBase(SemanticType::UNKNOWN);
                        break;
                    }
                }
            }

            if (arrayLen) {
                SemanticType lenType = arrayLen->semantics(ctx);
                if (!lenType.isError && lenType.base != SemanticType::INT) {
                    ctx.report("array length " + *arrayLen->getLiteral()->getValueString() + " must be integer");
                }
                if (arrayElems && arrayElems->getExprList() && arrayLen->getType() == LIT_VAL) {
                    ValueNode *lenVal = arrayLen->getLiteral();
                    if (lenVal && lenVal->getValueType() == ValueNode::LIT_INT) {
                        lenValue = lenVal->getInt();
                        size_t elemCount = arrayElems->getExprList()->size();
                        if (elemCount > static_cast<size_t>(lenVal->getInt())) {
                            ctx.report("array elements count " + to_string(elemCount) + "more than it size " + *lenVal->getValueString());
                        }
                    }
                }
            }

            if (elemType.base == SemanticType::UNKNOWN) {
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            } else {
                semType = elemType;
                addOuterArrayDim(semType, lenValue);
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
            if (arrayElems && arrayElems->getExprList()) {
                for (ExprNode *elem : *arrayElems->getExprList()) {
                    if (!elem) {
                        continue;
                    }
                    if (elem->getType() == COMPOSITE_LIT && elemType.base != SemanticType::UNKNOWN) {
                        evalCompositeLiteralWithExpected(elem, elemType, ctx);
                    } else {
                        SemanticType t = elem->semantics(ctx);
                        if (elemType.base != SemanticType::UNKNOWN && !elemType.sameKind(t)) {
                            ctx.report("cannot use " + *elem->getLiteral()->getValueString() + " (" + t.toString() + ") " +
                            elemType.toString() + " value in slice literal");
                        }
                    }
                }
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
            if (type != SUMMARY && leftType.isString() && rightType.isString()) {
                ctx.report("invalid operation: operator " + getDotLabel() + " not defined for variable of type string");
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
                break;
            }
            if (type == MODULO) {
                if (leftType.base != SemanticType::INT || rightType.base != SemanticType::INT || !leftType.isScalar() || !rightType.isScalar()) {
                    ctx.report("Modulo requires integer operands.");
                    semType = SemanticType::makeBase(SemanticType::UNKNOWN);
                    break;
                }
            } else if (!leftType.isNumeric() || !rightType.isNumeric() || !leftType.sameKind(rightType)) {
                ctx.report("invalid operation: " + toString() + " (mismatched operands types: " +
                    left->toString() + "of type " + leftType.toString() + " and " + right->toString() + " of type " + rightType.toString() + ")");
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
            string exprStr = (left ? left->toString() : "nil") + " " + getDotLabel() + " " + (right ? right->toString() : "nil");
            string typeName = comparisonTypeName(leftType);
            if (leftType.isError || rightType.isError) {
                semType = SemanticType::makeBase(SemanticType::BOOL);
                break;
            }
            if (!leftType.sameKind(rightType)) {
                ctx.report("invalid operation: " + exprStr + " (mismatched types " + leftType.toString() + " and " + rightType.toString() + ")");
            } else if ((type == EQUAL || type == NOT_EQUAL) && !isComparableType(leftType)) {
                if (leftType.sliceDims > 0) {
                    ctx.report("invalid operation: " + exprStr + " (slice can only be compared to nil)");
                } else {
                    ctx.report("invalid operation: " + exprStr + " (operator " + getDotLabel() + " not defined on " + typeName + ")");
                }
            } else if ((type != EQUAL && type != NOT_EQUAL) && !isOrderable(leftType)) {
                ctx.report("invalid operation: " + exprStr + " (operator " + getDotLabel() + " not defined on " + typeName + ")");
            }
            semType = SemanticType::makeBase(SemanticType::BOOL);
            break;
        }
        case AND:
        case OR: {
            SemanticType leftType = left ? left->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            SemanticType rightType = right ? right->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!leftType.isBool() || !rightType.isBool()) {
                ctx.report("invalid operation: " + toString() + " (mismatched operands types: " +
                    left->toString() + "of type " + leftType.toString() + " and " + right->toString() + " of type " + rightType.toString());
            }
            semType = SemanticType::makeBase(SemanticType::BOOL);
            break;
        }
        case NOT: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!operandType.isBool()) {
                ctx.report("invalid operation: operator " + getDotLabel() + " not defined on " + toString() +
                    " (value of type " + operandType.toString() + ")");
            }
            semType = SemanticType::makeBase(SemanticType::BOOL);
            break;
        }
        case UNARY_MINUS: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!operandType.isNumeric()) {
                ctx.report("invalid operation: operator " + getDotLabel() + " not defined on " + toString() +
                    " (value of type " + operandType.toString() + ")");
            }
            semType = operandType;
            break;
        }
        case ADDRESS_OF:
            if (!isAddressableExpr(operand, ctx)) {
                ctx.report("invalid operation: cannot take address of " + toString());
            }
            semType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            break;
        case ELEMENT_ACCESS: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            SemanticType indexType = index ? index->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (indexType.base != SemanticType::INT || !indexType.isScalar()) {
                bool reported = false;
                if (index && index->getType() == LIT_VAL) {
                    ValueNode *lit = index->getLiteral();
                    if (lit && lit->getValueType() == ValueNode::LIT_STRING && lit->getString()) {
                        ctx.report("cannot convert " + quoteGoStringLiteral(*lit->getString())
                            + " (untyped string constant) to type int");
                        reported = true;
                    }
                }
                if (!reported) {
                    ctx.report("invalid index access: " + toString() + " (index must be integer instead of "
                        + indexType.toString() + ")");
                }
            }
            if (operandType.arrayDims > 0) {
                semType = operandType;
                dropOuterArrayDim(semType);
            } else if (operandType.sliceDims > 0) {
                semType = operandType;
                semType.sliceDims -= 1;
            } else if (operandType.isString() && operandType.isScalar()) {
                semType = SemanticType::makeBase(SemanticType::INT);
            } else {
                ctx.report("cannot index " + toString() + " (variable of type " + operandType.toString() + ")");
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            }
            break;
        }
        case SLICE: {
            SemanticType operandType = operand ? operand->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            auto checkIndex = [&](ExprNode *expr, const string &label) {
                if (!expr) {
                    return;
                }
                SemanticType idxType = expr->semantics(ctx);
                if (idxType.base != SemanticType::INT || !idxType.isScalar()) {
                    ctx.report("cannot convert " + label + " of " + toString() + "(" + idxType.toString() + ") to integer");
                }
            };
            checkIndex(sliceLow, "low");
            checkIndex(sliceHigh, "high");
            checkIndex(sliceMax, "max");
            semType = operandType;
            if (operandType.arrayDims > 0) {
                dropOuterArrayDim(semType);
                semType.sliceDims = 1;
            } else if (operandType.sliceDims > 0 || (operandType.isString() && operandType.isScalar())) {
                if (operandType.isString() && operandType.isScalar() && sliceMax) {
                    ctx.report("invalid operation: 3-index slice of string " + operand->toString());
                }
            } else {
                ctx.report("cannot slice " + toString() + " (variable of type " + operandType.toString() + ")");
                semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            }
            break;
        }
        case FUNCTION_CALL:
            semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            {
                vector<SemanticType> results;
                if (getFunctionCallResults(this, ctx, results) || checkPackageCallResults(this, ctx, results)) {
                    if (results.size() == 1) {
                        semType = results[0];
                    } else if (results.size() > 1 && !ctx.allowMultiValue) {
                        string typeList = formatMultiValueTypeList(results);
                        ctx.report("multiple-value " + toString() + " (value of type (" + typeList + ")) in single-value context");
                        semType = SemanticType::makeError();
                    }
                } else {
                    if (operand) operand->semantics(ctx);
                    if (args) args->semantics(ctx);
                }
            }
            break;
        case SELECTOR:
            semType = SemanticType::makeBase(SemanticType::UNKNOWN);
            if (operand && operand->getType() == ID) {
                ValueNode *idVal = operand->getIdentifier();
                if (idVal && idVal->getString() && ctx.isImportName(*idVal->getString())) {
                    break;
                }
            }
            if (operand) {
                SemanticType operandType = operand->semantics(ctx);
                if (operandType.base != SemanticType::UNKNOWN) {
                    ctx.report(toString() + "undefined (type " + operandType.toString()
                        + " has no field of method " + *identifier->getValueString());
                }
            }
            break;
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

string ExprNode::toString() const {
    switch (type) {
        case ID: {
            if (!identifier) {
                return "nil";
            }
            if (identifier->getString()) {
                return *identifier->getString();
            }
            return identifier->getDotLabel();
        }
        case IOTA:
            return "iota";
        case EXPR_IN_BRACKETS:
            return "(" + (operand ? operand->toString() : "nil") + ")";
        case LIT_VAL:
            return value ? value->getDotLabel() : "nil";
        case COMPOSITE_LIT: {
            string out = "{";
            if (arrayElems && arrayElems->getExprList()) {
                bool first = true;
                for (ExprNode *expr : *arrayElems->getExprList()) {
                    if (!first) {
                        out += ", ";
                    }
                    out += expr ? expr->toString() : "nil";
                    first = false;
                }
            }
            out += "}";
            return out;
        }
        case ARRAY_LIT: {
            string lenStr = arrayLenAuto ? "..." : (arrayLen ? arrayLen->toString() : "nil");
            string elemTypeStr = arrayElemType ? arrayElemType->getSemanticType().toString() : "?";
            string out = "[" + lenStr + "]" + elemTypeStr + "{";
            if (arrayElems && arrayElems->getExprList()) {
                bool first = true;
                for (ExprNode *expr : *arrayElems->getExprList()) {
                    if (!first) {
                        out += ", ";
                    }
                    out += expr ? expr->toString() : "nil";
                    first = false;
                }
            }
            out += "}";
            return out;
        }
        case SLICE_LIT: {
            string elemTypeStr = arrayElemType ? arrayElemType->getSemanticType().toString() : "?";
            string out = "[]" + elemTypeStr + "{";
            if (arrayElems && arrayElems->getExprList()) {
                bool first = true;
                for (ExprNode *expr : *arrayElems->getExprList()) {
                    if (!first) {
                        out += ", ";
                    }
                    out += expr ? expr->toString() : "nil";
                    first = false;
                }
            }
            out += "}";
            return out;
        }
        case SUMMARY:
            return "(" + (left ? left->toString() : "nil") + " + " + (right ? right->toString() : "nil") + ")";
        case SUBTRACTION:
            return "(" + (left ? left->toString() : "nil") + " - " + (right ? right->toString() : "nil") + ")";
        case MULTIPLICATION:
            return "(" + (left ? left->toString() : "nil") + " * " + (right ? right->toString() : "nil") + ")";
        case DIVISION:
            return "(" + (left ? left->toString() : "nil") + " / " + (right ? right->toString() : "nil") + ")";
        case MODULO:
            return "(" + (left ? left->toString() : "nil") + " % " + (right ? right->toString() : "nil") + ")";
        case EQUAL:
            return "(" + (left ? left->toString() : "nil") + " == " + (right ? right->toString() : "nil") + ")";
        case NOT_EQUAL:
            return "(" + (left ? left->toString() : "nil") + " != " + (right ? right->toString() : "nil") + ")";
        case LESS:
            return "(" + (left ? left->toString() : "nil") + " < " + (right ? right->toString() : "nil") + ")";
        case GREATER:
            return "(" + (left ? left->toString() : "nil") + " > " + (right ? right->toString() : "nil") + ")";
        case LESS_OR_EQUAL:
            return "(" + (left ? left->toString() : "nil") + " <= " + (right ? right->toString() : "nil") + ")";
        case GREATER_OR_EQUAL:
            return "(" + (left ? left->toString() : "nil") + " >= " + (right ? right->toString() : "nil") + ")";
        case AND:
            return "(" + (left ? left->toString() : "nil") + " && " + (right ? right->toString() : "nil") + ")";
        case OR:
            return "(" + (left ? left->toString() : "nil") + " || " + (right ? right->toString() : "nil") + ")";
        case NOT:
            return "(!" + (operand ? operand->toString() : "nil") + ")";
        case UNARY_MINUS:
            return "(-" + (operand ? operand->toString() : "nil") + ")";
        case ADDRESS_OF:
            return "(&" + (operand ? operand->toString() : "nil") + ")";
        case ELEMENT_ACCESS:
            return (operand ? operand->toString() : "nil") + "[" + (index ? index->toString() : "nil") + "]";
        case SELECTOR: {
            string field = "nil";
            if (identifier) {
                if (identifier->getString()) {
                    field = *identifier->getString();
                } else {
                    field = identifier->getDotLabel();
                }
            }
            return (operand ? operand->toString() : "nil") + "." + field;
        }
        case SLICE: {
            string low = sliceLow ? sliceLow->toString() : "";
            string high = sliceHigh ? sliceHigh->toString() : "";
            string max = sliceMax ? sliceMax->toString() : "";
            string res = (operand ? operand->toString() : "nil") + "[" + low + ":" + high;
            if (sliceMax) {
                res += ":" + max;
            }
            res += "]";
            return res;
        }
        case FUNCTION_CALL: {
            string out = (operand ? operand->toString() : "nil") + "(";
            if (args && args->getExprList()) {
                bool first = true;
                for (ExprNode *expr : *args->getExprList()) {
                    if (!first) {
                        out += ", ";
                    }
                    out += expr ? expr->toString() : "nil";
                    first = false;
                }
            }
            out += ")";
            return out;
        }
        default:
            return "UNKNOWN_EXPR";
    }
}

string ExprNode::getDotLabel() const {
    switch (type) {
        case ID:                return "IDENTIFIER";
        case IOTA:              return "iota";
        case EXPR_IN_BRACKETS:  return "()";
        case LIT_VAL:           return "LIT_VAL";
        case COMPOSITE_LIT:     return "COMPOSITE_LIT";
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
                vector<SemanticType> exprTypes;
                list<ExprNode*> *exprNodes = nullptr;
                if (exprList) {
                    collectExprListTypes(exprList, ctx, exprTypes);
                    exprNodes = exprList->getExprList();
                }
                size_t exprCount = exprTypes.size();
                size_t retCount = retInfo->types.size();
                if (exprCount == 0) {
                    if (!retInfo->allowBareReturn) {
                        string want = "(";
                        for (size_t i = 0; i < retInfo->types.size(); ++i) {
                            if (i > 0) {
                                want += ", ";
                            }
                            want += retInfo->types[i].toString();
                        }
                        want += ")";
                        ctx.report("not enough return values\nhave ()\nwant " + want);
                    }
                } else {
                    if (exprCount != retCount) {
                        string have = "(";
                        for (size_t i = 0; i < exprTypes.size(); ++i) {
                            if (i > 0) {
                                have += ", ";
                            }
                            have += exprTypes[i].toString();
                        }
                        have += ")";
                        string want = "(";
                        for (size_t i = 0; i < retInfo->types.size(); ++i) {
                            if (i > 0) {
                                want += ", ";
                            }
                            want += retInfo->types[i].toString();
                        }
                        want += ")";
                        if (exprCount < retCount) {
                            ctx.report("not enough return values\nhave " + have + "\nwant " + want);
                        } else {
                            ctx.report("too many return values\nhave " + have + "\nwant " + want);
                        }
                    }
                    for (size_t i = 0; i < exprCount && i < retCount; ++i) {
                        if (!isAssignable(retInfo->types[i], exprTypes[i])) {
                            string msg;
                            ExprNode *exprNode = nullptr;
                            if (exprNodes) {
                                auto it = exprNodes->begin();
                                std::advance(it, static_cast<long>(i));
                                if (it != exprNodes->end()) {
                                    exprNode = *it;
                                }
                            }
                            buildReturnMismatch(exprNode, exprTypes[i], retInfo->types[i], msg);
                            ctx.report(msg);
                        }
                    }
                }
            } else {
                ctx.report("syntax error: non-declaration statement outside function body");
                if (exprList) exprList->semantics(ctx);
            }
            break;
        case BREAK:
            if (!ctx.inLoop() && !ctx.inSwitch()) {
                ctx.report("break is not in a loop, switch, or select");
            }
            break;
        case CONTINUE:
            if (!ctx.inLoop()) {
                ctx.report("continue is not in a loop");
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
                    ctx.report("non-boolean condition in if statement");
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
                string switchExprText;
                if (condition) {
                    switchType = condition->semantics(ctx);
                    switchExprText = formatExprForGoMessage(condition);
                }
                ctx.enterSwitch(switchType, switchExprText);
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
                    ctx.report("non-boolean condition in for statement");
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
                    ctx.report("non-boolean condition in for statement");
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
                        ctx.report("range clause permits at most two iteration variables");
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
                        string msg;
                        if (buildLiteralConvertMismatch(expr, *switchType, msg)) {
                            ctx.report(msg);
                        } else {
                            string switchExprText = ctx.currentSwitchExprText();
                            if (!switchExprText.empty()) {
                                string exprText = formatExprForGoMessage(expr);
                                ctx.report("invalid case " + exprText + " in switch on " + switchExprText
                                    + " (mismatched types " + exprType.toString() + " and " + switchType->toString() + ")");
                            } else {
                                ctx.report("Case type does not match switch expression type.");
                            }
                        }
                    } else if (switchType->base != SemanticType::UNKNOWN) {
                        string exprText = formatExprForGoMessage(expr);
                        if (ctx.registerSwitchCase(exprText)) {
                            ctx.report("duplicate case " + exprText + " in switch");
                        }
                    }
                }
            }
        } else {
            exprList->semantics(ctx);
        }
    } else {
        if (ctx.registerSwitchDefault()) {
            ctx.report("multiple defaults in switch");
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
            if (expr) {
                bool allowMultiValue = ctx.allowMultiValue;
                ctx.allowMultiValue = true;
                expr->semantics(ctx);
                ctx.allowMultiValue = allowMultiValue;
            }
            break;
        case INC:
        case DEC: {
            string opText = (type == INC) ? "++" : "--";
            string idName;
            if (isIdentifierExpr(expr, &idName) && idName == "_") {
                ctx.report("cannot use _ as value or type");
                break;
            }
            if (!isAddressableExpr(expr, ctx)) {
                ctx.report("cannot assign to " + expr->toString() + " (neither addressable nor a map index expression)");
                break;
            }
            SemanticType exprType = expr ? expr->semantics(ctx) : SemanticType::makeBase(SemanticType::UNKNOWN);
            if (!exprType.isNumeric()) {
                ctx.report("invalid operation: " + expr->toString() + opText + " (non-numeric type " + exprType.toString() + ")");
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
            vector<SemanticType> rightTypes;
            collectExprListTypes(right, ctx, rightTypes);
            if (leftExprs.size() != rightTypes.size()) {
                ctx.report("Assignment list size mismatch.");
            }
            auto itLeft = leftExprs.begin();
            int newCount = 0;
            size_t count = min(leftExprs.size(), rightTypes.size());
            for (size_t i = 0; i < count && itLeft != leftExprs.end(); ++i, ++itLeft) {
                ExprNode *leftExpr = *itLeft;
                SemanticType rightType = rightTypes[i];

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
                        ctx.report("undefined: " + idName);
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
                ctx.report("no new variables on left side of :=");
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
            int lenValue = -1;
            if (arrayLen && arrayLen->getType() == ExprNode::LIT_VAL) {
                ValueNode *lenVal = arrayLen->getLiteral();
                if (lenVal && lenVal->getValueType() == ValueNode::LIT_INT) {
                    lenValue = lenVal->getInt();
                }
            }
            addOuterArrayDim(elem, lenValue);
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
            ctx.markUsed(*id->getString());
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

ParamDeclListNode* SignatureNode::getParamList() const {
    return paramList;
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
    size_t idCount = idList->getIdList()->size();
    vector<SemanticType> exprTypes;
    if (exprList) {
        collectExprListTypes(exprList, ctx, exprTypes);
        if (exprTypes.size() != idCount) {
            ctx.report("VarSpec: initializer count does not match identifiers.");
        }
    }
    auto idIt = idList->getIdList()->begin();
    size_t exprIndex = 0;
    for (; idIt != idList->getIdList()->end(); ++idIt) {
        ValueNode *id = *idIt;
        if (isBlankIdentifier(id)) {
            if (exprIndex < exprTypes.size()) {
                ++exprIndex;
            }
            continue;
        }
        SemanticType initType = declaredType;
        if (exprIndex < exprTypes.size()) {
            SemanticType exprType = exprTypes[exprIndex++];
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
    ExprListNode *useExprList = exprList;
    if (!useExprList && ctx.inConstBlock && ctx.constPrevExprs) {
        useExprList = ctx.constPrevExprs;
    }
    if (exprList) {
        ctx.constPrevExprs = exprList;
    }
    size_t idCount = idList->getIdList()->size();
    vector<SemanticType> exprTypes;
    if (useExprList) {
        collectExprListTypes(useExprList, ctx, exprTypes);
        if (exprTypes.size() != idCount) {
            ctx.report("ConstSpec: initializer count does not match identifiers.");
        }
    }
    auto idIt = idList->getIdList()->begin();
    size_t exprIndex = 0;
    for (; idIt != idList->getIdList()->end(); ++idIt) {
        ValueNode *id = *idIt;
        if (isBlankIdentifier(id)) {
            if (exprIndex < exprTypes.size()) {
                ++exprIndex;
            }
            continue;
        }
        SemanticType initType = declaredType;
        if (exprIndex < exprTypes.size()) {
            SemanticType exprType = exprTypes[exprIndex++];
            if (declaredType.base != SemanticType::UNKNOWN && !isAssignable(declaredType, exprType)) {
                ctx.report("ConstSpec: type mismatch for initializer.");
            }
            if (declaredType.base == SemanticType::UNKNOWN) {
                initType = exprType;
            }
        } else {
            ctx.report("ConstSpec: missing initializer.");
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
    ctx.enterConstBlock();
    int iotaValue = 0;
    for (ConstSpecNode *spec : *specList) {
        ctx.iotaValue = iotaValue++;
        if (spec) spec->semantics(ctx);
    }
    ctx.exitConstBlock();
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
    local.errors.clear();
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

ValueNode* FuncDeclNode::getId() const {
    return id;
}

SignatureNode* FuncDeclNode::getSignature() const {
    return signature;
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

DeclNode* TopLevelDeclNode::getDecl() const {
    return decl;
}

FuncDeclNode* TopLevelDeclNode::getFuncDecl() const {
    return funcDecl;
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
    if (importType == POINT) {
        return;
    }
    if (importType == NAMED) {
        if (!alias || !alias->getString()) {
            return;
        }
        const string &name = *alias->getString();
        if (name == "_") {
            return;
        }
        string target;
        if (import && import->getString()) {
            target = baseImportName(*import->getString());
        }
        ctx.declareImport(name, target);
        return;
    }
    if (!import || !import->getString()) {
        return;
    }
    string name = baseImportName(*import->getString());
    if (name.empty()) {
        return;
    }
    ctx.declareImport(name, name);
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
    if (topLevelDeclList && topLevelDeclList->getList()) {
        for (TopLevelDeclNode *elem : *topLevelDeclList->getList()) {
            if (!elem) {
                continue;
            }
            FuncDeclNode *funcDecl = elem->getFuncDecl();
            if (!funcDecl || !funcDecl->getId() || !funcDecl->getId()->getString()) {
                continue;
            }
            const string &name = *funcDecl->getId()->getString();
            SemanticContext::FunctionInfo fnInfo;
            if (SignatureNode *sig = funcDecl->getSignature()) {
                fnInfo.params = collectParamTypes(sig->getParamList());
                bool allowBare = false;
                fnInfo.results = collectResultTypes(sig->getResult(), allowBare);
            }
            ctx.declareFunction(name, fnInfo);
        }
    }
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

string * ValueNode::getValueString() {
    switch (valueType) {
        case LIT_INT:
            return new string(to_string(intValue));
        case LIT_FLOAT:
            return new string(to_string(floatValue));
        case LIT_BOOL:
            return new string(boolValue ? "true" : "false");
        case LIT_STRING:
            return stringValue;
        case LIT_RUNE: {
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
            string runeStr = encodeRuneUtf8(static_cast<unsigned int>(intValue));
            if (runeStr.empty()) {
                return new string(to_string(intValue));
            }
            return new string(runeStr);
        }
        default:
            return nullptr;
    }
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
