#include "trapezoidalmap_alg.h"

#include <cg3/geometry/utils2.h>

namespace gasprj {

void crossedTrapezoids(const cg3::Segment2d& segment, const TrapezoidalMapDataset& trapMapData,
                       gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag,
                       std::vector<size_t>& traversedTraps);



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

void addSegmentToTrapezoidalMap(const cg3::Segment2d& segment, TrapezoidalMapDataset& trapMapData,
                                gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag)
{
    // Find the segment and its vertices
    // !!!: probably doing it not in the best way
    bool found;
    size_t segmentId = trapMapData.findSegment(segment, found);
    assert(found);
    size_t pLeftId = trapMapData.findPoint(segment.p1(), found);
    assert(found);
    size_t pRightId = trapMapData.findPoint(segment.p2(), found);
    assert(found);

    size_t noId = std::numeric_limits<size_t>::max();

    // Find the trapezoids traversed by the new segment
    std::vector<size_t> traversedTraps = std::vector<size_t>();
    crossedTrapezoids(segment, trapMapData, trapMap, dag, traversedTraps);
    assert(traversedTraps.size() > 0);

    // Move along the crossed trapezoids updating the trapezoidal map and the DAG
    if (traversedTraps.size() == 1) {
        // Segment lies entirely in a trapezoid

        //--- Trapezoidal map update
        size_t idxOldTrap = traversedTraps[0];
        gasprj::Trapezoid oldTrap = trapMap.trapezoids[idxOldTrap];

        // New trapezoid IDs
        size_t idxNewTrapLeft = idxOldTrap,
               idxNewTrapMiddleTop = trapMap.trapezoids.size(),
               idxNewTrapMiddleBottom = idxNewTrapMiddleTop+1,
               idxNewTrapRight = idxNewTrapMiddleBottom+1;

        // The old trapezoid becomes the left trapezoid of the new subdivision
        // We have to change its right adjacencies
        trapMap.trapezoids[idxOldTrap].idxPointRight = pLeftId;
        trapMap.trapezoids[idxOldTrap].idxTrapezoidTopRight = idxNewTrapMiddleTop;
        trapMap.trapezoids[idxOldTrap].idxTrapezoidBottomRight = idxNewTrapMiddleBottom;

        // Create and add the new middle-top trapezoid
        gasprj::Trapezoid newTrap = oldTrap;
        newTrap.idxPointLeft = pLeftId;
        newTrap.idxPointRight = pRightId;
        newTrap.idxEdgeBottom = segmentId;
        newTrap.idxTrapezoidTopLeft = idxNewTrapLeft;
        newTrap.idxTrapezoidBottomLeft = noId;
        newTrap.idxTrapezoidTopRight = idxNewTrapRight;
        newTrap.idxTrapezoidBottomRight = noId;
        trapMap.trapezoids.push_back(newTrap);

        // Create and add the new middle-bottom trapezoid
        newTrap = oldTrap;
        newTrap.idxPointLeft = pLeftId;
        newTrap.idxPointRight = pRightId;
        newTrap.idxEdgeTop = segmentId;
        newTrap.idxTrapezoidTopLeft = idxNewTrapLeft;
        newTrap.idxTrapezoidBottomLeft = noId;
        newTrap.idxTrapezoidTopRight = idxNewTrapRight;
        newTrap.idxTrapezoidBottomRight = noId;
        trapMap.trapezoids.push_back(newTrap);

        // Create and add the new right trapezoid
        newTrap = oldTrap;
        newTrap.idxPointLeft = pRightId;
        newTrap.idxTrapezoidTopLeft = idxNewTrapMiddleTop;
        newTrap.idxTrapezoidBottomLeft = idxNewTrapMiddleBottom;
        trapMap.trapezoids.push_back(newTrap);

        //--- end trapezoidal map update

        //--- DAG update
        size_t idxOldDagLeaf = dag.leafMap[idxOldTrap];
        gasprj::DAGNode dagNode = DAGNode();

        // New node IDs
        size_t idxRootXNode = idxOldDagLeaf,
               idxOtherXNode = dag.nodes.size(),
               idxOtherYNode = idxOtherXNode+1,
               idxLeafLeftTrap = idxOtherYNode+1,
               idxLeafMiddleTopTrap = idxLeafLeftTrap+1,
               idxLeafMiddleBottomTrap = idxLeafMiddleTopTrap+1,
               idxLeafRightTrap = idxLeafMiddleBottomTrap+1;

        // Root (X node)
        dagNode.typeInfo = gasprj::NodeType::XNode;
        dagNode.idxInfo = pLeftId;
        dagNode.idxNodeLeft = idxLeafLeftTrap;
        dagNode.idxNodeRight = idxOtherXNode;
        dag.nodes[idxRootXNode] = dagNode;

        // Other X Node
        dagNode.typeInfo = gasprj::NodeType::XNode;
        dagNode.idxInfo = pRightId;
        dagNode.idxNodeLeft = idxOtherYNode;
        dagNode.idxNodeRight = idxLeafRightTrap;
        dag.nodes.push_back(dagNode);

        // Other Y Node
        dagNode.typeInfo = gasprj::NodeType::YNode;
        dagNode.idxInfo = segmentId;
        dagNode.idxNodeLeft = idxLeafMiddleTopTrap;
        dagNode.idxNodeRight = idxLeafMiddleBottomTrap;
        dag.nodes.push_back(dagNode);

        // Leaves
        dagNode.typeInfo = gasprj::NodeType::TrapNode;
        dagNode.idxInfo = idxNewTrapLeft;
        dagNode.idxNodeLeft = noId;
        dagNode.idxNodeRight = noId;
        dag.nodes.push_back(dagNode);

        dagNode.typeInfo = gasprj::NodeType::TrapNode;
        dagNode.idxInfo = idxNewTrapMiddleTop;
        dagNode.idxNodeLeft = noId;
        dagNode.idxNodeRight = noId;
        dag.nodes.push_back(dagNode);

        dagNode.typeInfo = gasprj::NodeType::TrapNode;
        dagNode.idxInfo = idxNewTrapMiddleBottom;
        dagNode.idxNodeLeft = noId;
        dagNode.idxNodeRight = noId;
        dag.nodes.push_back(dagNode);

        dagNode.typeInfo = gasprj::NodeType::TrapNode;
        dagNode.idxInfo = idxNewTrapRight;
        dagNode.idxNodeLeft = noId;
        dagNode.idxNodeRight = noId;
        dag.nodes.push_back(dagNode);

        // Update leaf map
        dag.leafMap.erase(idxOldTrap));
        dag.leafMap.insert(std::pair<size_t, size_t>(idxNewTrapLeft, idxLeafLeftTrap));
        dag.leafMap.insert(std::pair<size_t, size_t>(idxNewTrapMiddleTop, idxLeafMiddleTopTrap));
        dag.leafMap.insert(std::pair<size_t, size_t>(idxNewTrapMiddleBottom, idxLeafMiddleBottomTrap));
        dag.leafMap.insert(std::pair<size_t, size_t>(idxNewTrapRight, idxLeafRightTrap));
        //--- end DAG update
    }
    else if (traversedTraps.size() == 2) {
        // Segment crosses just two trapezoids

    }
    else {
        // Segment crosses many trapezoids

    }
}

void crossedTrapezoids(const cg3::Segment2d& segment, const TrapezoidalMapDataset& trapMapData,
                       gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag,
                       std::vector<size_t>& traversedTraps) {
    // Need an empty vector
    assert(traversedTraps.size() == 0);

    // Search for the trapezoid in which the left vertex of the segment lays
    size_t idxTrap = queryTrapezoidalMap(segment.p1(), trapMapData, dag);
    cg3::Point2d trapRightPoint = trapMapData.getPoint(trapMap.getTrapezoid(idxTrap).idxPointRight);

    traversedTraps.push_back(idxTrap);
    while(segment.p2().x() > trapRightPoint.x()) {
        if (cg3::isPointAtLeft(segment, trapRightPoint)) {
            idxTrap = trapMap.getTrapezoid(idxTrap).idxTrapezoidBottomRight;
        }
        else if (cg3::isPointAtRight(segment, trapRightPoint)) {
            idxTrap = trapMap.getTrapezoid(idxTrap).idxTrapezoidTopRight;
        }
        else {
            // Point lies on a segment
            assert(false);
        }
        trapRightPoint = trapMapData.getPoint(trapMap.getTrapezoid(idxTrap).idxPointRight);
        traversedTraps.push_back(idxTrap);
    }
}

} // End namespace gasprj
