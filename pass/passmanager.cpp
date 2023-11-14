#include "passmanager.h"
#include "domtree.h"
#include "pass.h"
#include "loop_analyzer.h"
#include "rpo.h"

namespace compiler
{

PassManager::~PassManager()
{
    for (auto *analysis : analyses)
        delete analysis;

    for (auto *opt : opts)
        delete opt;
}

// template <typename PassName, typename... Args>
bool PassManager::runPassRpo(marker_t marker)
{
    auto *pass = new Rpo{graph, marker};
    /*if (pass->isAnalysis())
        analyses.push_back(static_cast<Analysis*>(pass));
    else if (pass->isOptimization())
        opts.push_back(static_cast<Optimization*>(pass));
    else
        UNREACHABLE();*/

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass rpo failed" << std::endl;
        return false;
    }
    return true;
}

bool PassManager::runPassDomTree()
{
    auto *pass = new DomTree{graph};

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass domtree failed" << std::endl;
        return false;
    }
    return true;
}

bool PassManager::runPassLoopAnalyzer()
{
    auto *pass = new LoopAnalyzer{graph};

    if (!pass->runPassImpl())
    {
        std::cerr << "Pass LoopAnalyzer failed" << std::endl;
        return false;
    }
    return true;
}

void PassManager::dumpAnalyses(std::ostream &out)
{
    std::for_each(analyses.begin(), analyses.end(),
                  [&out](auto *analysis) { out << analysis->getAnalysisName() << " "; });
}

void PassManager::dumpOpts(std::ostream &out)
{
    std::for_each(opts.begin(), opts.end(), [&out](auto *opt) { out << opt->getOptName() << " "; });
}

} // namespace compiler