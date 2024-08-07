#pragma once

#include "ir/marker.h"
#include "pass.h"
#include <concepts>
#include <iostream>
#include <vector>

namespace compiler
{

class Analysis;
class Optimization;
class Rpo;
class DomTree;
class LoopAnalysis;
class ConstFolding;
class Dce;
class Inline;
class Peepholes;
class ChecksElimination;
class LinearOrder;
class LivenessAnalysis;
class RegisterAllocation;
class Graph;

template <typename T>
concept LegalAnalysis =
    std::is_same_v<T, Rpo> || std::is_same_v<T, DomTree> || std::is_same_v<T, LoopAnalysis> ||
    std::is_same_v<T, LinearOrder> || std::is_same_v<T, LivenessAnalysis>;

template <typename T>
concept LegalOptimization =
    std::is_same_v<T, ConstFolding> || std::is_same_v<T, Dce> || std::is_same_v<T, Inline> ||
    std::is_same_v<T, Peepholes> || std::is_same_v<T, ChecksElimination> ||
    std::is_same_v<T, RegisterAllocation>;

template <typename T>
concept LegalPass = LegalAnalysis<T> || LegalOptimization<T>;

class PassManager final
{
  public:
    explicit PassManager(Graph* g) : graph(g)
    {}
    ~PassManager();

    template <LegalPass PassName, typename... Args>
    bool runPass(Args&&... args)
    {
        auto* pass = new PassName{graph, std::forward<Args>(args)...};
        [[maybe_unused]] std::string pass_name;
        if constexpr (LegalAnalysis<PassName>)
        {
            analyses.push_back(static_cast<Analysis*>(pass));
            pass_name = pass->getAnalysisName();
        }
        else if constexpr (LegalOptimization<PassName>)
        {
            opts.push_back(static_cast<Optimization*>(pass));
            pass_name = pass->getOptName();
        }
        else
        {
            UNREACHABLE();
        }

        if (!pass->runPassImpl())
        {
            std::cerr << "Pass " << pass_name << " failed" << std::endl;
            return false;
        }
        return true;
    }

    Graph* getGraph() const noexcept
    {
        return graph;
    }

    void dumpAnalyses(std::ostream& out = std::cout);
    void dumpOpts(std::ostream& out = std::cout);

  private:
    Graph* graph = nullptr;
    std::vector<Analysis*> analyses;
    std::vector<Optimization*> opts;
};

} // namespace compiler
