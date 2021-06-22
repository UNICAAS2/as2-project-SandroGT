#include "dag_node.h"

namespace gasprj {

/**
 * @brief Default constructor of a node
 */
inline DAG::Node::Node()
{
}

/**
 * @brief Constructor of a node
 * @param[in] type Type of the node
 * @param[in] idInfo Reference to the information held by the node (depends on the node type)
 * @param[in] idNodeL Reference to the left child
 * @param[in] idNodeR Reference to the right child
 */
inline DAG::Node::Node(Type type, size_t idInfo, size_t idNodeL, size_t idNodeR) :
    type(type), idInfo(idInfo), idNodeL(idNodeL), idNodeR(idNodeR)
{
}

/**
 * @brief Get the type of the node
 * @return Type of the node
 */
inline DAG::Node::Type DAG::Node::getType() const
{
    return type;
}

/**
 * @brief Set the type of the node
 * @param type New type of the node
 */
inline void DAG::Node::setType(Type type)
{
    this->type = type;
}

/**
 * @brief Get the reference to the information held by the node
 * @return An ID (or index) for the element referenced by the node
 */
inline size_t DAG::Node::getIdInfo() const
{
    return idInfo;
}

/**
 * @brief Set the reference to the information held by the node
 * @param The ID (or index) for the new element referenced by the node
 */
inline void DAG::Node::setIdInfo(size_t id)
{
    idInfo = id;
}

/**
 * @brief Get the index of the left child
 * @return Index of the left child
 */
inline size_t DAG::Node::getIdNodeL() const
{
    return idNodeL;
}

/**
 * @brief Set the index of the left child
 * @param New index of the left child
 */
inline void DAG::Node::setIdNodeL(size_t id)
{
    idNodeL = id;
}

/**
 * @brief Get the index of the right child
 * @return Index of the right child
 */
inline size_t DAG::Node::getIdNodeR() const
{
    return idNodeR;
}

/**
 * @brief Set the index of the right child
 * @param New index of the right child
 */
inline void DAG::Node::setIdNodeR(size_t id)
{
    idNodeR = id;
}

} // End namespace gasprj
