#ifndef NODE_H
#define NODE_H

#include "dag.h"

namespace gasprj {

/**
 * @brief The internal node or leaf of a DAG
 *
 * The internal nodes in a DAG are of two kinds, both with exactly one left and one right child:
 *  - X-node, containing a reference to a point (a vertex of a segment);
 *  - Y-node, containing a reference to a segment;
 * The leaves are the terminal nodes, containing a reference to a trapezoid.
 */
class DAG::Node
{
public:
    /**
     * @brief The type of the node.
     */
    enum class Type {XNode, YNode, Leaf};

    /**
     * @brief Definition of no reference to other nodes
     */
    static constexpr size_t NO_ID = std::numeric_limits<size_t>::max();

    /* Constructors */
    Node();
    Node(Type type, size_t idInfo, size_t idNodeL, size_t idNodeR);

    /* Getters and setters */
    Type getType() const;
    void setType(Type type);

    size_t getIdInfo() const;
    void setIdInfo(size_t id);

    size_t getIdNodeL() const;
    void setIdNodeL(size_t id);
    size_t getIdNodeR() const;
    void setIdNodeR(size_t id);

private:
    Type type;
    size_t idInfo, idNodeL, idNodeR;
};

} // End namespace gasprj

#include "dag_node.cpp"

#endif // NODE_H
