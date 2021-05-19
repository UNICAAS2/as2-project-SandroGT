#include "trapezoidalmap_alg.h"

#include <cg3/geometry/utils2.h>

namespace gasprj {

void addSegmentToTrapezoidalMap() {

}

size_t queryTrapezoidalMap(const cg3::Point2d& point, const TrapezoidalMapDataset& trapMapData,
                           const gasprj::DAG& dag)
{
    // !!!: getRoot returns a const reference, but I'll have to change dagNode. Everything ok?!?
    DAGNode dagNode = dag.getRoot();

    // Scroll the DAG until a leaf is reached
    while(dagNode.getNodeType() != gasprj::NodeType::TrapNode) {
        // Check if the actual node is a x-node (vertex) or a y-node (segment)
        switch(dagNode.getNodeType()) {
            case gasprj::NodeType::XNode: {
                // Point-Vertex comparison
                const cg3::Point2d vertex = trapMapData.getPoint(dagNode.getIdxInfo());
                if (point.x() < vertex.x()) {
                    dagNode = dag.getNodes()[dagNode.getIdxNodeLeft()];
                }
                else if (point.x() > vertex.x()) {
                    dagNode = dag.getNodes()[dagNode.getIdxNodeRight()];
                }
                else {
                    // Query point is not allowed to have the same x-coordinate of a vertex
                    assert(false);
                }
                break;
            }
            case gasprj::NodeType::YNode: {
                // Point-Segment comparison
                cg3::Segment2d segment = trapMapData.getSegment(dagNode.getIdxInfo());
                // !!!: not sure about taking the segment, maybe the indexed segment is better ?!?
                if (cg3::isPointAtLeft(segment, point)) {
                    dagNode = dag.getNodes()[dagNode.getIdxNodeLeft()];
                }
                else if (cg3::isPointAtRight(segment, point)) {
                    dagNode = dag.getNodes()[dagNode.getIdxNodeRight()];
                }
                else {
                    // Query point is not allowed to lay on a segment
                    assert(false);
                }
                break;
            }
            default: {
                // Unreachable
                assert(false);
            }
        }
    }

    // At this point the node must be a leaf
    assert(dagNode.getNodeType() == gasprj::NodeType::TrapNode);

    // Return the index of the trapezoid (it will need to be changed for sure)
    return dagNode.getIdxInfo();
}

} // End namespace gasprj
