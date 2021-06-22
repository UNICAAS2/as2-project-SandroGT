#ifndef DAG_H
#define DAG_H

#include <vector>

namespace gasprj {

/**
 * @brief The DAG query data structure
 *
 * This class defines a DAG data structure associated to a trapezoidal map to perform the point location queries. The
 * DAG (directed acyclic graph) is composed by a set of internal nodes (X and Y nodes) and leaves (trapezoids).
 */
class DAG
{
public:
    /* Classes */
    class Node;

    /* Constructors */
    DAG();

    /* Public methods */
    const Node &getRoot() const;
    const Node &getNode(size_t id) const;

    const std::vector<Node> &getNodes() const;
    size_t size();

    void addNode(Node &node);
    void overwriteNode(Node &node, size_t id);

    void clear();

private:
    /* Attributes */
    std::vector<Node> nodes;
};

} // End namespace gasprj

#include "dag_node.h"
#include "dag.tpp"

#endif // DAG_H
