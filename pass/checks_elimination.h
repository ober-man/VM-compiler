#pragma once

#include "ir/graph.h"
#include "pass.h"
#include "visitor.h"

namespace compiler
{

class ChecksElimination final : public Optimization, public Visitor
{
  public:
    explicit ChecksElimination(Graph* g) : Optimization(g), Visitor()
    {}

    ~ChecksElimination() override = default;

    bool runPassImpl() override;

    std::string getOptName() const noexcept override
    {
        return "ChecksElimination";
    }

  private:
    void visitZeroCheck([[maybe_unused]] Visitor* v, Inst* inst) override;
    void visitBoundsCheck([[maybe_unused]] Visitor* v, Inst* inst) override;
};

} // namespace compiler