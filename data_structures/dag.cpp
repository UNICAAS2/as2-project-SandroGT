#include "dag.h"

#include "trapezoid.h"

namespace gasprj {

DAG::DAG() :
    nodes()
{
}

const DAGNode& DAG::getRoot() const {
    return nodes[0];
}

const DAGNode& DAG::getNode(size_t id) const {
    return nodes[id];
}

const std::vector<DAGNode>& DAG::getNodes() const {
    return nodes;
}

void DAG::addNewNode(DAGNode& newNode) {
    nodes.push_back(newNode);
}

void DAG::overwriteNode(DAGNode& newNode, size_t id) {
    assert(id < nodes.size());
    nodes[id] = newNode;
}

size_t DAG::getNumberNodes() {
    return nodes.size();
}


size_t DAG::findLeaf(size_t trapezoidId) {
    return leavesMap[trapezoidId];
}

void DAG::mapLeaf(size_t trapezoidId, size_t nodeId) {
    leavesMap.insert(std::pair<size_t, size_t>(trapezoidId, nodeId));
}

void DAG::remapLeaf(size_t trapezoidId, size_t nodeId) {
    leavesMap.erase(trapezoidId);
    leavesMap.insert(std::pair<size_t, size_t>(trapezoidId, nodeId));
}

void DAG::clear() {
    nodes.clear();
    leavesMap.clear();

    DAGNode firstNode = DAGNode(NodeType::Leaf, 0, NO_ID, NO_ID);
    addNewNode(firstNode);
    mapLeaf(0, 0);
}

} // End namespace gasprj
