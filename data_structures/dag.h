#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>

namespace gasprj {

class DAG
{
public:
    /* Classes */
    class Node;

    /* Constructors */
    DAG();

    /* Public methods */
    Node& getRoot() const;
    Node& getNode(size_t id) const;

    const std::vector<Node>& getNodes() const;
    size_t getNumberNodes();

    void addNode(Node& node);
    void overwriteNode(Node& node, size_t id);

    void clear();

private:
    std::vector<Node> nodes;
};

} // End namespace gasprj

#include "dag_node.h"
#include "dag.cpp"

#endif // DAG_H
