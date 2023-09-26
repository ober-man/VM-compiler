#pragma once

#include "basicblock.h"

namespace compiler
{

class Graph
{
	public:
		Graph(std::string& name_ = "") : name(name_)
		{
			nodes.reserve(50);
			size = 0;
		}

		~Graph() = default;

		size_t size() const       { return size; }
		bool isEmpty()            { return size == 0; }

		std::string getName() const     { return func_name; }
		void setName(std::string& name) { func_name = name; }

		std::vector<std::shared_ptr<BasicBlock>>& getBBs() const 
		{ 
			return BBs; 
		}

		std::shared_ptr<BasicBlock> getFirstBB() const 
		{ 
			if (size == 0)
				return nullptr;
			return BBs[0]; 
		}

		std::shared_ptr<BasicBlock> getLastBB () const 
		{ 
			if (size == 0)
				return nullptr;
			return BBs[size-1];
		}

		std::shared_ptr<BasicBlock> getBB(size_t id) const
		{
			if(id >= size)
				return nullptr;
			else return BBs[id];
		}

		void insertBB(std::shared_ptr<BasicBlock> bb)
		{
			BBs.push_back(bb);
			++size;
		}

		void removeBB(std::shared_ptr<BasicBlock> bb) 
		{
			auto it = BBs.erase(std::find(BBs.begin(), BBs.end(), bb)); 
			assert(it != BBs.end() && "remove not existing bb");
		}

		void removeBB(size_t num) 
		{ 
			auto it = BBs.erase(std::find_if(BBs.begin(), BBs.end(), 
						           			 [](auto bb) { bb->getId() == num; }));
			assert(it != BBs.end() && "remove not existing bb");
		}

		void replaceBB(std::shared_ptr<BasicBlock> bb) 
		{
			auto it = std::find(BBs.begin(), BBs.end(), bb); 
			assert(it != BBs.end() && "replace not existing bb");
			BBs[it->getId()] = bb;
		}

		void replaceBB(size_t num) 
		{ 
			auto it = std::find_if(BBs.begin(), BBs.end(), 
						           [](auto bb) { bb->getId() == num; });
			assert(it != BBs.end() && "replace not existing bb");
			BBs[num] = bb;
		}

		void dump()
		{
			std::cout << "Graph for proc" << name << std::endl;
			std::for_each(BBs.begin(), BBs.end(), [](auto bb) { dump(bb); });
		}

	private:
		std::string func_name "";
		size_t size = 0;

		std::vector<std::shared_ptr<Node> BBs;

};

} // namespace compiler