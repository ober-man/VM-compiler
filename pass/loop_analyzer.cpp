#include "loop_analyzer.h"
#include "domtree.h"
#include "rpo.h"

namespace compiler
{

bool LoopAnalyzer::runPassImpl()
{
    assert(graph != nullptr && "nullptr graph in LoopAnalyzer pass");

    graph->runPassRpo();
    auto bbs = graph->getRpoBBs();
    assert(bbs.size() > 0 && "empty graph in LoopAnalyzer");

    // ...

    return true;
}

} // namespace compiler