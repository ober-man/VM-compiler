#pragma once

#include "ir/graph.h"
#include "pass.h"
#include "visitor.h"

namespace compiler
{

class Peepholes final : public Optimization, public Visitor
{
  public:
    explicit Peepholes(Graph* g) : Optimization(g), Visitor()
    {}

    ~Peepholes() override = default;

    bool runPassImpl() override;

    std::string getOptName() const noexcept override
    {
        return "Peepholes";
    }

  private:
    void visitMul([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitAShr([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitOr([[maybe_unused]] Visitor* v, Inst* inst) override;
};

} // namespace compiler