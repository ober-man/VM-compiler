#pragma once

#include "ir/graph.h"
#include "pass.h"
#include <bitset>

namespace compiler
{

// Dead Code Elimination
class Dce final : public Optimization
{
  public:
    explicit Dce(Graph* g) : Optimization(g)
    {}

    ~Dce() override = default;

    bool runPassImpl() override;

    std::string getOptName() const noexcept override
    {
        return "Dce";
    }

  private:
    bool isDceAppliable(Inst* inst);
};

} // namespace compiler