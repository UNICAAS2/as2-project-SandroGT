#include "trapezoidalmap_alg.h"

#include <cg3/geometry/utils2.h>

namespace gasprj {

void crossedTrapezoids(const cg3::Segment2d& segment, const TrapezoidalMapDataset& trapMapData,
                       gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag,
                       std::vector<size_t>& traversedTraps);



size_t queryTrapezoidalMap(const cg3::Point2d& point, const TrapezoidalMapDataset& trapMapData,
                           const gasprj::DAG& dag)
{
    // TODO: find a way to apply the const correctness and use the pass-by-reference to avoid the
    // copy of nodes
    DAGNode dagNode = dag.getRoot();
    // Scroll the DAG until a leaf is reached
    while(dagNode.getNodeType() != gasprj::NodeType::Leaf) {
        // Check if the actual node is a x-node (vertex) or a y-node (segment)
        switch(dagNode.getNodeType()) {
            // Point-Vertex comparison
            case gasprj::NodeType::XNode: {
                const cg3::Point2d& vertex = trapMapData.getPoint(dagNode.getIdInfo());
                // Query point to the left of the segment vertex
                if (point.x() < vertex.x()) {
                    dagNode = dag.getNode(dagNode.getIdNodeL());
                }
                // Query point either to the right or in the same vertical extension of the vertex:
                // in both cases we treat it as being at the right (for real or conceptually)
                else {
                    dagNode = dag.getNode(dagNode.getIdNodeR());
                }
                break;
            }
            // Point-Segment comparison
            case gasprj::NodeType::YNode: {
                const cg3::Segment2d& segment = trapMapData.getSegment(dagNode.getIdInfo());
                // Query point above the segment
                if (cg3::isPointAtLeft(segment, point)) {
                    assert(segment.p1() != point);
                    dagNode = dag.getNode(dagNode.getIdNodeL());
                }
                // Query point below the segment
                else if (cg3::isPointAtRight(segment, point)) {
                    assert(segment.p1() != point);
                    dagNode = dag.getNode(dagNode.getIdNodeR());
                }
                // Query point lying on the segment
                else {
                    assert(false);
                }
                break;
            }
            default: {
                // Unreachable: should not reach this with a leaf
                assert(false);
            }
        }
    }

    // At this point the node must be a leaf
    assert(dagNode.getNodeType() == gasprj::NodeType::Leaf);

    // Return the index of the trapezoid
    return dagNode.getIdInfo();
}

size_t queryTrapezoidalMap(const cg3::Segment2d& newSegment, const TrapezoidalMapDataset& trapMapData,
                           const gasprj::DAG& dag) {
    // TODO: find a way to apply the const correctness and use the pass-by-reference to avoid the
    // copy of nodes
    DAGNode dagNode = dag.getRoot();
    // Scroll the DAG until a leaf is reached
    while(dagNode.getNodeType() != gasprj::NodeType::Leaf) {
        // Check if the actual node is a x-node (vertex) or a y-node (segment)
        switch(dagNode.getNodeType()) {
            // Point-Vertex comparison
            case gasprj::NodeType::XNode: {
                const cg3::Point2d& vertex = trapMapData.getPoint(dagNode.getIdInfo());
                // Query point to the left of the segment vertex
                if (newSegment.p1().x() < vertex.x()) {
                    dagNode = dag.getNode(dagNode.getIdNodeL());
                }
                // Query point either to the right or in the same vertical extension of the vertex:
                // in both cases we treat it as being at the right (for real or conceptually)
                else {
                    dagNode = dag.getNode(dagNode.getIdNodeR());
                }
                break;
            }
            // Point-Segment comparison
            case gasprj::NodeType::YNode: {
                const cg3::Segment2d& segment = trapMapData.getSegment(dagNode.getIdInfo());
                // Query point above the segment
                if (cg3::isPointAtLeft(segment, newSegment.p1())) {
                    assert(segment.p1() != newSegment.p1());
                    dagNode = dag.getNode(dagNode.getIdNodeL());
                }
                // Query point below the segment
                else if (cg3::isPointAtRight(segment, newSegment.p1())) {
                    assert(segment.p1() != newSegment.p1());
                    dagNode = dag.getNode(dagNode.getIdNodeR());
                }
                // Query point lying on the segment: this happens when the query point is also the
                // left endpoint of the segment. Compare the slopes of the two segments to check
                // where the query continues
                else {
                    assert(segment.p1() == newSegment.p1());
                    // New segment slope is larger, continue above
                    if (cg3::isPointAtLeft(segment, newSegment.p2())) {
                        dagNode = dag.getNode(dagNode.getIdNodeL());
                    }
                    // New segment slope is smaller, continue below
                    else if (cg3::isPointAtRight(segment, newSegment.p2())) {
                        dagNode = dag.getNode(dagNode.getIdNodeR());
                    }
                    else {
                        // Overlapping segments
                        assert(false);
                    }
                }
                break;
            }
            default: {
                // Unreachable: should not reach this with a leaf
                assert(false);
            }
        }
    }

    // At this point the node must be a leaf
    assert(dagNode.getNodeType() == gasprj::NodeType::Leaf);

    // Return the index of the trapezoid
    return dagNode.getIdInfo();
}

void crossedTrapezoids(const cg3::Segment2d& segment, const TrapezoidalMapDataset& trapMapData,
                       gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag,
                       std::vector<size_t>& traversedTraps) {
    // Need an empty vector
    assert(traversedTraps.size() == 0);

    // Search for the trapezoid in which the left vertex of the segment lies
    size_t idTrap = queryTrapezoidalMap(segment, trapMapData, dag);
    cg3::Point2d trapezoidPointR = trapMapData.getPoint(trapMap.getTrapezoid(idTrap).getIdPointR());

    // Search for all the crossed trapezoids and save their IDs in the vector
    traversedTraps.push_back(idTrap);
    while(segment.p2().x() > trapezoidPointR.x()) {
        // If the right point of the trapezoid lies above the segment, move to the bottom-right adjacency
        if (cg3::isPointAtLeft(segment, trapezoidPointR)) {
            idTrap = trapMap.getTrapezoid(idTrap).getIdTrapezoidBR();
        }
        // If the right point of the trapezoid lies below the segment, move to the top-right adjacency
        else if (cg3::isPointAtRight(segment, trapezoidPointR)) {
            idTrap = trapMap.getTrapezoid(idTrap).getIdTrapezoidTR();
        }
        // Point lies on a segment
        else {
            assert(false);
        }
        trapezoidPointR = trapMapData.getPoint(trapMap.getTrapezoid(idTrap).getIdPointR());
        traversedTraps.push_back(idTrap);
    }
}



void addSegmentToTrapezoidalMap(const cg3::Segment2d& newSegment, TrapezoidalMapDataset& trapMapData,
                                gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag)
{
    // Find the trapezoids traversed by the new segment
    std::vector<size_t> traversedTraps = std::vector<size_t>();
    crossedTrapezoids(newSegment, trapMapData, trapMap, dag, traversedTraps);
    assert(traversedTraps.size() > 0);
    
    // Update the DAG and the trapezoidal map.
    // Behave differently wheter one or more than one trapezoid has been intersected by the new segment
    if (traversedTraps.size() == 1) {

        // Trapezoid of interest
        size_t idTrapezoid = traversedTraps[0];
        gasprj::Trapezoid trapezoid = trapMap.getTrapezoid(idTrapezoid);

        //--- Trapezoidal Map update

        // Check the geometry conditions
        bool overlapL = newSegment.p1() == trapMapData.getPoint(trapezoid.getIdPointL());
        bool overlapR = newSegment.p2() == trapMapData.getPoint(trapezoid.getIdPointR());
        bool isLVertexTEnd = trapMapData.getSegment(trapezoid.getIdSegmentT()).p1() == trapMapData.getPoint(trapezoid.getIdPointL());
        bool isLVertexBEnd = trapMapData.getSegment(trapezoid.getIdSegmentB()).p1() == trapMapData.getPoint(trapezoid.getIdPointL());
        bool isRVertexTEnd = trapMapData.getSegment(trapezoid.getIdSegmentT()).p2() == trapMapData.getPoint(trapezoid.getIdPointR());
        bool isRVertexBEnd = trapMapData.getSegment(trapezoid.getIdSegmentB()).p2() == trapMapData.getPoint(trapezoid.getIdPointR());

        // New trapezoid IDs
        size_t idLastUsed = idTrapezoid;
        size_t idNewTrapL = NO_ID;
        size_t idNewTrapT = NO_ID;
        size_t idNewTrapB = NO_ID;
        size_t idNewTrapR = NO_ID;

        // If the left point of the new segment do not overlap with the left point of the trapezoid,
        // then we have a left trapezoid
        if (!overlapL) {
            // Assign ID to left trapezoid
        }
        // If the right point of the new segment do not overlap with the right point of the trapezoid,
        // then we have a right trapezoid
        if (!overlapR) {
            // Assign ID to right trapezoid
        }
        // We always have the top and bottom trapezoids
        // Assign ID to top and bottom trapezoid

    }
    else {
        
    }
}

} // End namespace gasprj
