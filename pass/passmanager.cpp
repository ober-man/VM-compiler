#include "passmanager.h"
#include "domtree.h"
#include "linear_order.h"
#include "liveness.h"
#include "loop_analysis.h"
#include "pass.h"
#include "rpo.h"

namespace compiler
{

PassManager::~PassManager()
{
    for (auto* analysis : analyses)
        delete analysis;

    for (auto* opt : opts)
        delete opt;
}

void PassManager::dumpAnalyses(std::ostream& out)
{
    std::for_each(analyses.begin(), analyses.end(),
                  [&out](auto* analysis) { out << analysis->getAnalysisName() << " "; });
}

void PassManager::dumpOpts(std::ostream& out)
{
    std::for_each(opts.begin(), opts.end(), [&out](auto* opt) { out << opt->getOptName() << " "; });
}

} // namespace compiler