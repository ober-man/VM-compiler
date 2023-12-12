#pragma once

#include "ir/marker.h"
#include "pass.h"
#include <iostream>
#include <vector>

namespace compiler
{

class Analysis;
class Optimization;
class Rpo;
class DomTree;
class LoopAnalysis;
class LinearOrder;
class LivenessAnalysis;
class Graph;

#define NO_MARKER 1000
#define UNREACHABLE() __builtin_unreachable()

class PassManager final
{
  public:
    explicit PassManager(Graph *g) : graph(g)
    {}
    ~PassManager();

    // template <typename PassName>
    // TODO: fix PassManager
    // [some template magic was broken and there is a crutch]
    bool runPassRpo(marker_t marker = NO_MARKER);
    bool runPassDomTree();
    bool runPassLoopAnalysis();
    bool runPassLinearOrder();
    bool runPassLivenessAnalysis();

    Graph *getGraph() const noexcept
    {
        return graph;
    }

    void dumpAnalyses(std::ostream &out = std::cout);
    void dumpOpts(std::ostream &out = std::cout);

  private:
    Graph *graph = nullptr;
    std::vector<Analysis *> analyses;
    std::vector<Optimization *> opts;
};

} // namespace compiler
