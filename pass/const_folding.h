#pragma once

#include "ir/graph.h"
#include "pass.h"
#include "visitor.h"

namespace compiler
{

class ConstFolding final : public Optimization, public Visitor
{
  public:
    explicit ConstFolding(Graph* g) : Optimization(g), Visitor()
    {}

    ~ConstFolding() override = default;

    bool runPassImpl() override;

    std::string getOptName() const noexcept override
    {
        return "ConstFolding";
    }

  private:
    void visitAdd([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitSub([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitMul([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitDiv([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitMod([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitShl([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitShr([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitAShr([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitAnd([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitOr([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitXor([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitNot([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitNeg([[maybe_unused]] Visitor* v, Inst* inst) override;
};

} // namespace compiler