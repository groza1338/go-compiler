#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <unordered_map>
#include <optional>
#include <list>
#include "golang_parser.hpp"

#include <jvm/class.h>
#include <jvm/method.h>
#include <jvm/attribute-code.h>
#include <jvm/descriptor-method.h>
#include <jvm/descriptor-field.h>

namespace fs = std::filesystem;
using namespace jvm;

extern FILE *yyin;

extern int yyparse();

extern ProgramNode *root;

namespace {
    struct LocalVarInfo {
        SemanticType type;
        uint16_t index = 0;
    };

    class BytecodeGenerator {
    public:
        BytecodeGenerator()
            : clazz("Main", "java/lang/Object")
        {
            clazz.addFlag(Class::ACC_PUBLIC);
            clazz.addFlag(Class::ACC_SUPER);

            DescriptorMethod mainDesc(std::nullopt, {DescriptorField("java/lang/String", 1)});
            mainMethod = clazz.getOrCreateMethod("main", mainDesc);
            mainMethod->addFlag(Method::ACC_PUBLIC);
            mainMethod->addFlag(Method::ACC_STATIC);
            code = mainMethod->getCodeAttribute();

            systemOut = clazz.getOrCreateFieldrefConstant(
                "java/lang/System",
                "out",
                DescriptorField("java/io/PrintStream")
            );
        }

        void generate(ProgramNode *program) {
            if (!program) {
                return;
            }
            FuncDeclNode *mainFunc = findMain(program);
            if (!mainFunc) {
                return;
            }
            nextLocalIndex = 1; // slot 0 = String[] args
            generateFunctionBody(mainFunc->getBody());
            *code << code->ReturnVoid();
        }

        void writeTo(const fs::path &outPath) {
            fs::create_directories(outPath.parent_path());
            std::ofstream out(outPath, std::ios::binary);
            clazz.writeTo(out);
        }

    private:
        Class clazz;
        Method *mainMethod = nullptr;
        AttributeCode *code = nullptr;
        ConstantFieldref *systemOut = nullptr;
        std::unordered_map<std::string, LocalVarInfo> locals;
        uint16_t nextLocalIndex = 0;

        FuncDeclNode* findMain(ProgramNode *program) {
            TopLevelDeclListNode *declList = program->getTopLevelDeclList();
            if (!declList || !declList->getList()) {
                return nullptr;
            }
            for (TopLevelDeclNode *elem : *declList->getList()) {
                if (!elem) {
                    continue;
                }
                FuncDeclNode *func = elem->getFuncDecl();
                if (!func || !func->getId()) {
                    continue;
                }
                ValueNode *id = func->getId();
                if (id->getString() && *id->getString() == "main") {
                    return func;
                }
            }
            return nullptr;
        }

        void generateFunctionBody(StmtNode *body) {
            if (!body) {
                return;
            }
            if (body->getType() == StmtNode::BLOCK) {
                StmtListNode *list = body->getStmtList();
                if (list && list->getStmtList()) {
                    for (StmtNode *stmt : *list->getStmtList()) {
                        generateStmt(stmt);
                    }
                }
            } else {
                generateStmt(body);
            }
        }

        void generateStmt(StmtNode *stmt) {
            if (!stmt) {
                return;
            }
            switch (stmt->getType()) {
                case StmtNode::DECLARATION:
                    generateDecl(stmt->getDecl());
                    break;
                case StmtNode::SIMPLE:
                    generateSimple(stmt->getSimpleStmt());
                    break;
                case StmtNode::BLOCK: {
                    StmtListNode *list = stmt->getStmtList();
                    if (list && list->getStmtList()) {
                        for (StmtNode *elem : *list->getStmtList()) {
                            generateStmt(elem);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void generateDecl(DeclNode *decl) {
            if (!decl) {
                return;
            }
            if (VarSpecListNode *varList = decl->getVarSpecList()) {
                generateVarSpecList(varList);
            }
            if (ConstSpecListNode *constList = decl->getConstSpecList()) {
                generateConstSpecList(constList);
            }
        }

        void generateVarSpecList(VarSpecListNode *list) {
            if (!list || !list->getList()) {
                return;
            }
            for (VarSpecNode *spec : *list->getList()) {
                generateVarSpec(spec);
            }
        }

        void generateConstSpecList(ConstSpecListNode *list) {
            if (!list || !list->getList()) {
                return;
            }
            for (ConstSpecNode *spec : *list->getList()) {
                generateConstSpec(spec);
            }
        }

        void generateVarSpec(VarSpecNode *spec) {
            if (!spec) {
                return;
            }
            IdListNode *ids = spec->getIdList();
            if (!ids || !ids->getIdList()) {
                return;
            }
            ExprListNode *exprs = spec->getExprList();
            auto *exprList = exprs ? exprs->getExprList() : nullptr;
            TypeNode *typeNode = spec->getType();
            auto exprIt = exprList ? exprList->begin() : list<ExprNode*>::iterator();

            for (ValueNode *id : *ids->getIdList()) {
                if (!id || !id->getString()) {
                    continue;
                }
                ExprNode *expr = (exprList && exprIt != exprList->end()) ? *exprIt : nullptr;
                if (exprList && exprIt != exprList->end()) {
                    ++exprIt;
                }
                SemanticType semType = typeNode ? typeNode->getSemanticType() : inferExprType(expr);
                uint16_t slot = allocateLocal(*id->getString(), semType);
                if (expr) {
                    if (!emitExpr(expr)) {
                        continue;
                    }
                } else {
                    emitDefaultValue(semType);
                }
                emitStore(semType, slot);
            }
        }

        void generateConstSpec(ConstSpecNode *spec) {
            if (!spec) {
                return;
            }
            IdListNode *ids = spec->getIdList();
            if (!ids || !ids->getIdList()) {
                return;
            }
            ExprListNode *exprs = spec->getExprList();
            auto *exprList = exprs ? exprs->getExprList() : nullptr;
            TypeNode *typeNode = spec->getType();
            auto exprIt = exprList ? exprList->begin() : list<ExprNode*>::iterator();

            for (ValueNode *id : *ids->getIdList()) {
                if (!id || !id->getString()) {
                    continue;
                }
                ExprNode *expr = (exprList && exprIt != exprList->end()) ? *exprIt : nullptr;
                if (exprList && exprIt != exprList->end()) {
                    ++exprIt;
                }
                SemanticType semType = typeNode ? typeNode->getSemanticType() : inferExprType(expr);
                uint16_t slot = allocateLocal(*id->getString(), semType);
                if (expr) {
                    if (!emitExpr(expr)) {
                        continue;
                    }
                } else {
                    emitDefaultValue(semType);
                }
                emitStore(semType, slot);
            }
        }

        void generateSimple(SimpleStmtNode *stmt) {
            if (!stmt) {
                return;
            }
            if (stmt->getType() != SimpleStmtNode::EXPR) {
                return;
            }
            ExprNode *expr = stmt->getExpr();
            if (expr) {
                generatePrint(expr);
            }
        }

        void generatePrint(ExprNode *expr) {
            if (!expr || expr->getType() != ExprNode::FUNCTION_CALL) {
                return;
            }
            ExprNode *callee = expr->getOperand();
            ExprListNode *args = expr->getArgs();
            if (!callee || !args || !args->getExprList()) {
                return;
            }
            if (callee->getType() != ExprNode::SELECTOR) {
                return;
            }
            ExprNode *pkgExpr = callee->getOperand();
            ValueNode *fnNameVal = callee->getIdentifier();
            if (!pkgExpr || pkgExpr->getType() != ExprNode::ID || !fnNameVal || !fnNameVal->getString()) {
                return;
            }
            ValueNode *pkgNameVal = pkgExpr->getIdentifier();
            if (!pkgNameVal || !pkgNameVal->getString()) {
                return;
            }
            if (*pkgNameVal->getString() != "fmt" || *fnNameVal->getString() != "Print") {
                return;
            }
            for (ExprNode *arg : *args->getExprList()) {
                if (!arg) {
                    continue;
                }
                SemanticType argType = inferExprType(arg);
                *code << code->GetStatic(systemOut);
                if (!emitExpr(arg)) {
                    continue;
                }
                ConstantMethodref *printMethod = getPrintMethod(argType);
                if (printMethod) {
                    *code << code->InvokeVirtual(printMethod);
                }
            }
        }

        ConstantMethodref* getPrintMethod(const SemanticType &type) {
            switch (type.base) {
                case SemanticType::FLOAT:
                    return clazz.getOrCreateMethodrefConstant(
                        "java/io/PrintStream",
                        "print",
                        DescriptorMethod(std::nullopt, {DescriptorField(Descriptor::Double)})
                    );
                case SemanticType::STRING:
                    return clazz.getOrCreateMethodrefConstant(
                        "java/io/PrintStream",
                        "print",
                        DescriptorMethod(std::nullopt, {DescriptorField("java/lang/String")})
                    );
                case SemanticType::BOOL:
                    return clazz.getOrCreateMethodrefConstant(
                        "java/io/PrintStream",
                        "print",
                        DescriptorMethod(std::nullopt, {DescriptorField(Descriptor::Boolean)})
                    );
                case SemanticType::INT:
                case SemanticType::RUNE:
                default:
                    return clazz.getOrCreateMethodrefConstant(
                        "java/io/PrintStream",
                        "print",
                        DescriptorMethod(std::nullopt, {DescriptorField(Descriptor::Int)})
                    );
            }
        }

        uint16_t allocateLocal(const std::string &name, const SemanticType &type) {
            auto it = locals.find(name);
            if (it != locals.end()) {
                return it->second.index;
            }
            uint16_t slot = nextLocalIndex;
            locals[name] = {type, slot};
            nextLocalIndex += (type.base == SemanticType::FLOAT) ? 2 : 1;
            return slot;
        }

        void emitDefaultValue(const SemanticType &type) {
            switch (type.base) {
                case SemanticType::FLOAT:
                    *code << code->PushDouble(0.0);
                    break;
                case SemanticType::STRING:
                    *code << code->PushString("");
                    break;
                case SemanticType::BOOL:
                    *code << code->PushInt(0);
                    break;
                case SemanticType::RUNE:
                case SemanticType::INT:
                default:
                    *code << code->PushInt(0);
                    break;
            }
        }

        bool emitExpr(ExprNode *expr) {
            if (!expr) {
                return false;
            }
            switch (expr->getType()) {
                case ExprNode::LIT_VAL:
                    return emitLiteral(expr->getLiteral());
                case ExprNode::ID: {
                    ValueNode *id = expr->getIdentifier();
                    if (!id || !id->getString()) {
                        return false;
                    }
                    auto it = locals.find(*id->getString());
                    if (it == locals.end()) {
                        return false;
                    }
                    emitLoad(it->second.type, it->second.index);
                    return true;
                }
                default:
                    return false;
            }
        }

        bool emitLiteral(ValueNode *literal) {
            if (!literal) {
                return false;
            }
            switch (literal->getValueType()) {
                case ValueNode::LIT_INT:
                    *code << code->PushInt(literal->getInt());
                    return true;
                case ValueNode::LIT_FLOAT:
                    *code << code->PushDouble(literal->getFloat());
                    return true;
                case ValueNode::LIT_BOOL:
                    *code << code->PushInt(literal->getBool() ? 1 : 0);
                    return true;
                case ValueNode::LIT_STRING:
                    if (literal->getString()) {
                        *code << code->PushString(*literal->getString());
                    } else {
                        *code << code->PushString("");
                    }
                    return true;
                case ValueNode::LIT_RUNE:
                    *code << code->PushInt(literal->getRune());
                    return true;
                default:
                    return false;
            }
        }

        void emitLoad(const SemanticType &type, uint16_t index) {
            switch (type.base) {
                case SemanticType::FLOAT:
                    *code << code->LoadDouble(index);
                    break;
                case SemanticType::STRING:
                    *code << code->LoadReference(index);
                    break;
                case SemanticType::BOOL:
                case SemanticType::RUNE:
                case SemanticType::INT:
                default:
                    *code << code->LoadInt(index);
                    break;
            }
        }

        void emitStore(const SemanticType &type, uint16_t index) {
            switch (type.base) {
                case SemanticType::FLOAT:
                    *code << code->StoreDouble(index);
                    break;
                case SemanticType::STRING:
                    *code << code->StoreReference(index);
                    break;
                case SemanticType::BOOL:
                case SemanticType::RUNE:
                case SemanticType::INT:
                default:
                    *code << code->StoreInt(index);
                    break;
            }
        }

        SemanticType inferExprType(ExprNode *expr) {
            if (!expr) {
                return SemanticType::makeBase(SemanticType::UNKNOWN);
            }
            if (expr->getType() == ExprNode::LIT_VAL) {
                ValueNode *lit = expr->getLiteral();
                if (!lit) {
                    return SemanticType::makeBase(SemanticType::UNKNOWN);
                }
                switch (lit->getValueType()) {
                    case ValueNode::LIT_INT: return SemanticType::makeBase(SemanticType::INT);
                    case ValueNode::LIT_FLOAT: return SemanticType::makeBase(SemanticType::FLOAT);
                    case ValueNode::LIT_BOOL: return SemanticType::makeBase(SemanticType::BOOL);
                    case ValueNode::LIT_STRING: return SemanticType::makeBase(SemanticType::STRING);
                    case ValueNode::LIT_RUNE: return SemanticType::makeBase(SemanticType::RUNE);
                    default: break;
                }
            }
            if (expr->getType() == ExprNode::ID) {
                ValueNode *id = expr->getIdentifier();
                if (id && id->getString()) {
                    auto it = locals.find(*id->getString());
                    if (it != locals.end()) {
                        return it->second.type;
                    }
                }
            }
            return SemanticType::makeBase(SemanticType::UNKNOWN);
        }
    };
}
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];

    yyin = fopen(inputFile.c_str(), "r");
    if (!yyin) {
        cout << ("Could not open input file: '" + inputFile + "'");
        return 1;
    }

    int parse_result = yyparse();

    if (parse_result != 0) {
        cout << ("Parsing failed with code: '" + std::to_string(parse_result) + "'");
        return 1;
    }

    if (!root) {
        cout << ("No parse tree generated");
        return 1;
    }

    SemanticContext semCtx;
    root->semantics(semCtx);
    const char *semDir = std::getenv("SEMANTIC_OUT_DIR");
    fs::path semPath = semDir && *semDir
        ? fs::path(semDir) / (fs::path(inputFile).filename().string() + ".sem.txt")
        : fs::path(inputFile + ".sem.txt");

    if (!semCtx.errors.empty()) {
        fs::create_directories(semPath.parent_path());
        std::ofstream semOut(semPath, std::ios::trunc);
        for (const auto &err : semCtx.errors) {
            semOut << "Semantic error: " << err << "\n";
        }
    } else {
        std::filesystem::remove(semPath);
    }

    const char *rawEnv = std::getenv("PRINT_RAW_AST");
    bool wantRaw = rawEnv && *rawEnv == '1';
    bool wantTyped = !wantRaw && semCtx.errors.empty();
    if (wantRaw || wantTyped) {
        AstNode::setShowTypes(wantTyped);
        cout << "digraph AST {\n";
        cout << root->toDot();
        cout << "}\n";
    }

    if (semCtx.errors.empty()) {
        fs::path outDir = fs::path("generated_classes");
        fs::path outPath = outDir / "Main.class";
        BytecodeGenerator generator;
        generator.generate(root);
        generator.writeTo(outPath);
    }

    return 0;
}
