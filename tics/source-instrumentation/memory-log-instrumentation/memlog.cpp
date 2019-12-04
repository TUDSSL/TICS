/*------------------------------------------------------------------------------
Memory logging instrumentation

Copyright (c) 2019 Vito Kortbeek (v.kortbeek-1@tudelft.nl)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------*/

#include <string>
using namespace std;

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory MatcherSampleCategory("Matcher Sample");

#define GLOBAL_WRITE    "VMEM_WR"
#define GLOBAL_WRITE_PI "VMEM_WR_PI" // post inctrement
#define GLOBAL_WRITE_PD "VMEM_WR_PD" // post decrement
#define PTR_WRITE       "VMEM_WR"

void LogInstrumentation(string type, string original, string instrumented, string location)
{
    llvm::errs() << ">>> Instrumented " << type
        << " \'" << original << "\' "
        << "-> \'" << instrumented << "\' "
        << "at: " << location << "\n";
}

bool excludeFromInstrumentation(const Decl *d)
{
    const RawComment* rc = d->getASTContext().getRawCommentForDeclNoCache(d);
    //llvm::errs() << "Decl: ";
    //llvm::errs() << d;
    //llvm::errs() << "\n";
    if (rc) {
        ASTContext& ctx = d->getASTContext();
        //SourceManager& sm = ctx.getSourceManager();

        //std::string raw = rc->getRawText(sm);
        std::string brief = rc->getBriefText(ctx);

        llvm::errs() << "////Comment: " << brief << "\n";
    }

    return false;
}

class PointerDereferenceAssignHandler : public MatchFinder::MatchCallback {
    public:
        PointerDereferenceAssignHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

        virtual void run(const MatchFinder::MatchResult &Result) {
            const UnaryOperator *PtrRef = Result.Nodes.getNodeAs<UnaryOperator>("ptrRefAssign");

            llvm::errs() << "Ref Assign expr: ";
            llvm::errs() << PtrRef;
            llvm::errs() << "\n";

            // Write and opening bracket
            Rewrite.InsertTextBefore(PtrRef->getBeginLoc(), PTR_WRITE "(");

            // Closing bracket
            Rewrite.InsertTextAfterToken(PtrRef->getEndLoc(), ")");

            string location = PtrRef->getBeginLoc().printToString(Rewrite.getSourceMgr());
            LogInstrumentation("Pointer ref assign", "", "", location);
        }

    private:
        Rewriter &Rewrite;
};

class GlobalVarHandler : public MatchFinder::MatchCallback {
    public:
        GlobalVarHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

        virtual void run(const MatchFinder::MatchResult &Result) {

            const DeclRefExpr * GlobalVarRef = Result.Nodes.getNodeAs<DeclRefExpr>("gvar_ref");

            const VarDecl * GlobalVar = Result.Nodes.getNodeAs<VarDecl>("gvar");
            if (excludeFromInstrumentation(GlobalVar)) {
                return;
            }

            string global_var_ref = GlobalVarRef->getNameInfo().getName().getAsString();
            string global_var_ref_instr =  GLOBAL_WRITE"(" + global_var_ref + ")";

            //Rewrite.InsertText(GlobalVarRef->getBeginLoc(), global_var_ref, true, true);
            Rewrite.ReplaceText(GlobalVarRef->getLocation(), global_var_ref_instr);

            string location = GlobalVarRef->getLocation().printToString(Rewrite.getSourceMgr());
            LogInstrumentation("Global variable", global_var_ref, global_var_ref_instr, location);
        }

    private:
        Rewriter &Rewrite;
};

class GlobalVarPrePostHandler : public MatchFinder::MatchCallback {
    public:
        GlobalVarPrePostHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

        virtual void run(const MatchFinder::MatchResult &Result) {

            const UnaryOperator *PrePost = Result.Nodes.getNodeAs<UnaryOperator>("unary_operator");

            const VarDecl * GlobalVar = Result.Nodes.getNodeAs<VarDecl>("gvar");
            if (GlobalVar != NULL && excludeFromInstrumentation(GlobalVar)) {
                return;
            }

            // Write and opening bracket
            if (PrePost->isPostfix()) {
                if (PrePost->isIncrementOp()) {
                    Rewrite.InsertTextAfter(PrePost->getBeginLoc(), GLOBAL_WRITE_PI "(");
                } else {
                    Rewrite.InsertTextAfter(PrePost->getBeginLoc(), GLOBAL_WRITE_PD "(");
                }
            } else {
                Rewrite.InsertTextAfter(PrePost->getBeginLoc(), GLOBAL_WRITE "(");
            }

            // Closing bracket
            Rewrite.InsertTextAfterToken(PrePost->getEndLoc(), ")");

            string location = PrePost->getExprLoc().printToString(Rewrite.getSourceMgr());
            LogInstrumentation("Global variable pre/post", "-", "-", location);
        }

    private:
        Rewriter &Rewrite;
};

class StructHandler : public MatchFinder::MatchCallback {
    public:
        StructHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

        virtual void run(const MatchFinder::MatchResult &Result) {

            const DeclRefExpr *VarRef = Result.Nodes.getNodeAs<DeclRefExpr>("struct_ref");
            const MemberExpr *StructMember = Result.Nodes.getNodeAs<MemberExpr>("struct_member");

            // Optional
            const ArraySubscriptExpr *ArrSubs = Result.Nodes.getNodeAs<ArraySubscriptExpr>("struct_arr_subs");

            const VarDecl *Var = Result.Nodes.getNodeAs<VarDecl>("struct");
            if (excludeFromInstrumentation(Var)) {
                return;
            }


            string location = VarRef->getLocation().printToString(Rewrite.getSourceMgr());

            if (ArrSubs == NULL) {
                /* Not an array member */
                string struct_member = StructMember->getMemberNameInfo().getName().getAsString();

                string var_ref = VarRef->getNameInfo().getName().getAsString();
                string var_ref_instr =  GLOBAL_WRITE"(" + var_ref;
                Rewrite.ReplaceText(VarRef->getLocation(), var_ref_instr);
                string struct_member_instr = struct_member + ")";
                Rewrite.ReplaceText(StructMember->getMemberLoc(), struct_member_instr);

                LogInstrumentation("Struct", var_ref, var_ref_instr, location);
            } else {
                /* Array member */
                // Opening brace
                Rewrite.InsertTextAfter(ArrSubs->getBeginLoc(), GLOBAL_WRITE "(");
                // Closing brace
                Rewrite.InsertTextAfterToken(ArrSubs->getEndLoc(), ")");

                LogInstrumentation("Struct array member", "-", "-", location);
            }

        }

    private:
        Rewriter &Rewrite;
};

class ArrHandler : public MatchFinder::MatchCallback {
    public:
        ArrHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

        virtual void run(const MatchFinder::MatchResult &Result) {

            const ArraySubscriptExpr *GlobalArrSubs = Result.Nodes.getNodeAs<ArraySubscriptExpr>("arr_subs");
            string location = GlobalArrSubs->getBeginLoc().printToString(Rewrite.getSourceMgr());

            if (isInstrumented(GlobalArrSubs)) {
                LogInstrumentation("Array [already instrumented]", "-", "-", location);
                return;
            } else {
                addInstrumented(GlobalArrSubs);
            }

            // Write and opening bracket
            Rewrite.InsertTextAfter(GlobalArrSubs->getBeginLoc(), GLOBAL_WRITE "(");

            // Closing bracket
            Rewrite.InsertTextAfterToken(GlobalArrSubs->getEndLoc(), ")");

            //string global_var_ref = GlobalArrRef->getNameInfo().getName().getAsString();
            //string global_var_ref_instr =  GLOBAL_WRITE"(" + global_var_ref + ")";
            //Rewrite.ReplaceText(GlobalArrRef->getLocation(), global_var_ref_instr);

            LogInstrumentation("Array", "-", "-", location);
        }

    private:
        Rewriter &Rewrite;

        list<const ArraySubscriptExpr *> instrumented_list;

        bool isInstrumented(const ArraySubscriptExpr *arr) {
            list<const ArraySubscriptExpr *>::iterator it;
            for (it=instrumented_list.begin(); it!=instrumented_list.end(); it++) {
                if (arr == *it) {
                    return true;
                }
            }
            return false;
        }

        void addInstrumented(const ArraySubscriptExpr *arr) {
            if (arr != NULL && !isInstrumented(arr)) {
                instrumented_list.push_back(arr);
            }
        }
};

class StructDereferenceAssignHandler : public MatchFinder::MatchCallback {
    public:
        StructDereferenceAssignHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

        virtual void run(const MatchFinder::MatchResult &Result) {
            const DeclRefExpr *SVarRef = Result.Nodes.getNodeAs<DeclRefExpr>("struct_ref");
            const MemberExpr *StructMember = Result.Nodes.getNodeAs<MemberExpr>("struct_member");

            string struct_member = StructMember->getMemberNameInfo().getName().getAsString();

            string var_ref = SVarRef->getNameInfo().getName().getAsString();
            string var_ref_instr =  GLOBAL_WRITE"(" + var_ref;
            Rewrite.ReplaceText(SVarRef->getLocation(), var_ref_instr);

            string struct_member_instr = struct_member + ")";
            Rewrite.ReplaceText(StructMember->getMemberLoc(), struct_member_instr);

            string location = SVarRef->getLocation().printToString(Rewrite.getSourceMgr());
            LogInstrumentation("Struct ptr", var_ref, var_ref_instr, location);
        }

    private:
        Rewriter &Rewrite;
};


// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser. It registers a couple of matchers and runs them on
// the AST.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : \
      HandlerForGlobal(R), \
      HandlerForGlobalPrePost(R), \
      HandlerForStruct(R), \
      HandlerForArr(R), \
      HandlerForPtrAssign(R)
    {

    // Global variable write
    Matcher.addMatcher(
            binaryOperator(
                //hasOperatorName("="),
                has(
                    declRefExpr(
                        to(
                            varDecl(
                                hasGlobalStorage()
                            ).bind("gvar")
                        )
                    ).bind("gvar_ref")
                )
            )
        ,
        &HandlerForGlobal);


    // Global postfix increment variable write
    Matcher.addMatcher(
            unaryOperator(
                anyOf(
                    hasOperatorName("++"),
                    hasOperatorName("--")
                ),
                has(
                    declRefExpr(
                        to(
                            varDecl(
                                hasGlobalStorage()
                            ).bind("gvar")
                        )
                    ).bind("gvar_ref")
                )
            ).bind("unary_operator")
        ,
        &HandlerForGlobalPrePost);

    // Global struct write
    Matcher.addMatcher(
            binaryOperator(
                //hasOperatorName("="),
                has(
                    memberExpr(
                    has(
                        declRefExpr(
                            to(
                                varDecl(
                                    hasGlobalStorage()
                                ).bind("struct")
                            )
                        ).bind("struct_ref")
                    )).bind("struct_member")
                )
            )
        ,
        &HandlerForStruct);

    // Global Struct array member assign
    Matcher.addMatcher(
            binaryOperator(
                hasOperatorName("="),
                has(arraySubscriptExpr(has(implicitCastExpr(
                    has(
                        memberExpr(
                        has(
                            declRefExpr(
                                to(
                                    varDecl(
                                        hasGlobalStorage()
                                    ).bind("struct")
                                )
                            ).bind("struct_ref")
                        )).bind("struct_member")
                    )
                ))).bind("struct_arr_subs"))
            )
        ,
        &HandlerForStruct);

    // Global struct member postfix increment variable write
    Matcher.addMatcher(
            unaryOperator(
                anyOf(
                    hasOperatorName("++"),
                    hasOperatorName("--")
                ),
                has(
                    memberExpr(
                    has(
                        declRefExpr(
                            to(
                                varDecl(
                                    hasGlobalStorage()
                                ).bind("struct")
                            )
                        ).bind("struct_ref")
                    )).bind("struct_member")
                )
            ).bind("unary_operator")
        ,
        &HandlerForGlobalPrePost);


    // Struct member reference assign
    Matcher.addMatcher(
            binaryOperator(
                //hasOperatorName("="),
                has(
                    memberExpr(
                    has(
                        implicitCastExpr(has(
                        declRefExpr(
                            to(
                                varDecl().bind("struct")
                            )
                        ).bind("struct_ref")))
                    )).bind("struct_member")
                )
            )
        ,
        &HandlerForStruct);

    // Struct member reference postfix increment variable write
    Matcher.addMatcher(
            unaryOperator(
                anyOf(
                    hasOperatorName("++"),
                    hasOperatorName("--")
                ),
                has(
                    memberExpr(
                    has(
                        implicitCastExpr(has(
                        declRefExpr(
                            to(
                                varDecl().bind("struct")
                            )
                        ).bind("struct_ref")))
                    )).bind("struct_member")
                )
            ).bind("unary_operator")
        ,
        &HandlerForGlobalPrePost);

    // Struct array member reference assign
    Matcher.addMatcher(
            binaryOperator(
                //hasOperatorName("="),
                has(arraySubscriptExpr(has(implicitCastExpr(
                    has(
                        memberExpr(
                        has(
                            implicitCastExpr(has(
                            declRefExpr(
                                to(
                                    varDecl().bind("struct")
                                )
                            ).bind("struct_ref")))
                        )).bind("struct_member")
                    )
                ))).bind("struct_arr_subs"))
            )
        ,
        &HandlerForStruct);

    // Global direct Array write unless struct member
    Matcher.addMatcher(
            binaryOperator(
                hasOperatorName("="),
                has(
                    arraySubscriptExpr(
                        has(
                            implicitCastExpr(
                                unless(has(memberExpr())),
                                hasDescendant(
                                    declRefExpr(to(
                                        varDecl(
                                            hasGlobalStorage()
                                        ).bind("gvar")
                                    ))
                                )
                            )
                        )
                    ).bind("arr_subs")
                )
            )
        ,
        &HandlerForArr);

    // Array write pointer unless struct member
    Matcher.addMatcher(
            binaryOperator(
                hasOperatorName("="),
                has(
                    arraySubscriptExpr(
                        has(
                            implicitCastExpr(
                                hasCastKind(CK_LValueToRValue),
                                unless(has(memberExpr()))
                            )
                        )
                    ).bind("arr_subs")
                )
            )
        ,
        &HandlerForArr);


    // Pointer reference assign
    Matcher.addMatcher(
        binaryOperator(
            hasOperatorName("="),
            has(
                unaryOperator(hasOperatorName("*"),
                    has(implicitCastExpr(hasCastKind(CK_LValueToRValue)))
                    ).bind("ptrRefAssign")
            )
        ),
        &HandlerForPtrAssign);

  }

  void HandleTranslationUnit(ASTContext &Context) override {
    // Run the matchers when we have the whole TU parsed.
    Matcher.matchAST(Context);
  }
  // Override the method that gets called for each parsed top-level
  // declaration.
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
      for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
          // Traverse the declaration using our AST visitor.
          (*b)->dump();
      }
      return true;
  }

private:
  GlobalVarHandler HandlerForGlobal;
  GlobalVarPrePostHandler HandlerForGlobalPrePost;
  StructHandler HandlerForStruct;
  ArrHandler HandlerForArr;
  PointerDereferenceAssignHandler HandlerForPtrAssign;
  //StructDereferenceAssignHandler HandlerForStructPtrAssign;

  MatchFinder Matcher;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
    public:
        MyFrontendAction() {}
        void EndSourceFileAction() override {
            TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
                .write(llvm::outs());
        }

        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                StringRef file) override {
            TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
            return llvm::make_unique<MyASTConsumer>(TheRewriter);
        }

    private:
        Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, MatcherSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
