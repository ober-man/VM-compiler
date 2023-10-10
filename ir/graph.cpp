#include "graph.h"

namespace compiler
{

void replaceBB(std::shared_ptr<BasicBlock> bb,
               std::shared_ptr<BasicBlock> new_bb)
{
    auto it = std::find(BBs.begin(), BBs.end(), bb);
    assert(it != BBs.end() && "replace not existing bb");
    BBs[it->getId()] = new_bb;
}

void replaceBB(size_t num, std::shared_ptr<BasicBlock> new_bb)
{
    auto it = std::find_if(BBs.begin(), BBs.end(),
                           [num](auto bb) { return bb->getId() == num; });
    assert(it != BBs.end() && "replace not existing bb");
    BBs[num] = new_bb;
}

} // namespace compiler