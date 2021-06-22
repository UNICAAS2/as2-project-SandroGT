#include "dag.h"

#include <cassert>

#include "trapezoid.h"

namespace gasprj {

/**
 * @brief Default constructor of a DAG
 */
inline DAG::DAG() :
    nodes()
{
}

/**
 * @brief Get the root of the DAG
 * @return The first node of the DAG
 */
inline const DAG::Node &DAG::getRoot() const
{
    return nodes[0];
}

/**
 * @brief Get a specif node of the DAG
 * @param[in] id Index of the node
 * @return The specified node of the DAG
 */
inline const DAG::Node &DAG::getNode(size_t id) const
{
    return nodes[id];
}

/**
 * @brief Get all the nodes of the DAG
 * @return A vector containing all the nodes of the DAG
 */
inline const std::vector<DAG::Node> &DAG::getNodes() const
{
    return nodes;
}

/**
 * @brief Add a new node in the DAG
 * @param node New node
 *
 * Perform an insertion in the back of the vector of nodes
 */
inline void DAG::addNode(DAG::Node &node)
{
    nodes.push_back(node);
}

/**
 * @brief Replace a node in the DAG
 * @param node New node
 * @param id Index of the node to be replaced
 */
inline void DAG::overwriteNode(DAG::Node &node, size_t id)
{
    assert(id < nodes.size());
    nodes[id] = node;
}

/**
 * @brief Get the number of nodes (internal and leaves) composing the DAG
 * @return The number of nodes in the DAG
 */
inline size_t DAG::size()
{
    return nodes.size();
}

/**
 * @brief Delete all the nodes in the DAG
 */
inline void DAG::clear()
{
    nodes.clear();
}

} // End namespace gasprj
