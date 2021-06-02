#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>

namespace gasprj {

enum class NodeType {XNode, YNode, Leaf};

class DAGNode
{
public:
    DAGNode();
    DAGNode(NodeType typeInfo, size_t idInfo, size_t idNodeL, size_t idNodeR);

    NodeType getNodeType();
    void setNodeType(NodeType nodeType);

    size_t getIdInfo() const;
    void setIdInfo(size_t id);

    size_t getIdNodeL() const;
    void setIdNodeL(size_t id);
    size_t getIdNodeR() const;
    void setIdNodeR(size_t id);

private:
    NodeType typeInfo;
    size_t idInfo, idNodeL, idNodeR;
};

class DAG
{
public:
    DAG();

    const DAGNode& getRoot() const;
    const DAGNode& getNode(size_t id) const;
    const std::vector<DAGNode>& getNodes() const;
    void addNewNode(DAGNode& newNode);
    void overwriteNode(DAGNode& newNode, size_t id);
    size_t getNumberNodes();

    size_t findLeaf(size_t trapezoidId);
    void mapLeaf(size_t trapezoidId, size_t nodeId);
    void remapLeaf(size_t trapezoidId, size_t nodeId);

    void clear();

private:
    std::vector<DAGNode> nodes;

    std::unordered_map<size_t, size_t> leavesMap;
};

} // End namespace gasprj

#endif // DAG_H
