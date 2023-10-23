#pragma once

#include "pass.h"

namespace compiler
{

class Analysis;
class Optimization;
class Rpo;
class DomTree;
class LoopAnalyzer;
class Graph;

#define UNREACHABLE() __builtin_unreachable()

class PassManager final
{
  public:
    explicit PassManager(Graph *g) : graph(g)
    {}
    ~PassManager();

    // template <typename PassName>
    // TODO: fix PassManager
    // [some template magic was broken and there is crutch]
    void runPassRpo();
    void runPassDomTree();
    void runPassLoopAnalyzer();

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