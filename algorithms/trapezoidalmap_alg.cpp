#include "trapezoidalmap_alg.h"

#include <cg3/geometry/utils2.h>

namespace gasprj {

void addSegmentToTrapezoidalMap() {

}

size_t queryTrapezoidalMap(const cg3::Point2d& point, const TrapezoidalMapDataset& trapMapData,
                           const gasprj::DAG& dag)
{
    DAGNode dagNode = dag.getRoot();

    // Scroll the DAG until a leaf is reached
    while(dagNode.getNodeType() != gasprj::NodeType::TrapNode) {
        // Check if the actual node is a x-node (vertex) or a y-node (segment)
        switch(dagNode.getNodeType())
        {
        case gasprj::NodeType::XNode:
            // Point-Vertex comparison
            if () {

            }
            break;
        case gasprj::NodeType::YNode:
            // Point-Segment comparison
            if () {

            }
            break;
        default:
            // Unreachable
            assert(false);
        } // End switch
    }

    // At this point the node must be a leaf
    assert(dagNode.getNodeType() == gasprj::NodeType::TrapNode)

}

} // End namespace gasprj
