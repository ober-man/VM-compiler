#pragma once

#include "basicblock.h"
#include "pass/passmanager.h"

namespace compiler
{

#define GRAPH_BB_NUM 50

class Graph
{
  public:
    Graph(std::string name = "") : func_name(name)
    {
        BBs.reserve(GRAPH_BB_NUM);
        rpo_BBs.reserve(GRAPH_BB_NUM);
        graph_size = 0;
    }

    ~Graph() = default;

    size_t size() const
    {
        return graph_size;
    }
    bool isEmpty()
    {
        return graph_size == 0;
    }

    std::string getName() const
    {
        return func_name;
    }
    void setName(std::string &name)
    {
        func_name = name;
    }

    std::vector<std::shared_ptr<BasicBlock>> &getBBs()
    {
        return BBs;
    }

    std::vector<std::shared_ptr<BasicBlock>> &getRPOBBs()
    {
        return rpo_BBs;
    }

    void setRPOBBs(std::vector<std::shared_ptr<BasicBlock>> &rpo)
    {
        rpo_BBs = rpo;
    }

    std::shared_ptr<BasicBlock> getFirstBB() const
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[0];
    }

    std::shared_ptr<BasicBlock> getLastBB() const
    {
        if (graph_size == 0)
            return nullptr;
        return BBs[graph_size - 1];
    }

    std::shared_ptr<BasicBlock> getBB(size_t id) const
    {
        if (id >= graph_size)
            return nullptr;
        else
            return BBs[id];
    }

    void insertBB(std::shared_ptr<BasicBlock> bb)
    {
        BBs.push_back(bb);
        ++graph_size;
    }

    void removeBB(std::shared_ptr<BasicBlock> bb)
    {
        auto it = BBs.erase(std::find(BBs.begin(), BBs.end(), bb));
        assert(it != BBs.end() && "remove not existing bb");
    }

    void removeBB(size_t num)
    {
        auto it =
            BBs.erase(std::find_if(BBs.begin(), BBs.end(), [num](auto bb) {
                return bb->getId() == num;
            }));
        assert(it != BBs.end() && "remove not existing bb");
    }

    void replaceBB(std::shared_ptr<BasicBlock> bb,
                   std::shared_ptr<BasicBlock> new_bb);
    void replaceBB(size_t num, std::shared_ptr<BasicBlock> new_bb);

    void dump(std::ostream &out = std::cout)
    {
        std::cout << "Graph for proc" << func_name << std::endl;
        std::for_each(BBs.begin(), BBs.end(),
                      [&out](auto bb) { bb->dump(out); });
    }

    void dumpDomTree(std::ostream &out = std::cout)
    {
        BBs[0]->dumpDomTree(out);
    }

    template <typename Pass>
    void RunPass()
    {
        PassManager pm;
        pm.RunPass<Pass>(std::make_shared<Graph>(*this));
    }

  private:
    std::string func_name = "";
    size_t graph_size = 0;

    std::vector<std::shared_ptr<BasicBlock>> BBs;
    std::vector<std::shared_ptr<BasicBlock>> rpo_BBs;
};

} // namespace compiler