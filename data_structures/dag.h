#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>

namespace gasprj {

enum class NodeType {XNode, YNode, TrapNode};

class DAGNode
{
public:
    // TODO: define constructor
    DAGNode();

    NodeType getNodeType();
    size_t getIdxInfo();
    size_t getIdxNodeLeft();
    size_t getIdxNodeRight();

private:
    NodeType typeInfo;
    size_t idxInfo;
    size_t idxNodeLeft, idxNodeRight;
};

class DAG
{
public:
    // TODO: define constructor
    DAG();

    const DAGNode& getRoot() const;
    const std::vector<DAGNode>& getNodes() const;

private:
    std::vector<DAGNode> nodes;
    std::unordered_map<size_t, size_t> leafMap;
};

} // End namespace gasprj

#endif // DAG_H
