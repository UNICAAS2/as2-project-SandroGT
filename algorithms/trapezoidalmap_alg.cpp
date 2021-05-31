#include "trapezoidalmap_alg.h"

#include<algorithm>

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
    size_t idPointR = trapMap.getTrapezoid(idTrap).getIdPointR();
    cg3::Point2d trapezoidPointR = (idPointR == NO_ID) ? trapMap.getBoundingBox().max() : trapMapData.getPoint(idPointR);

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
        idPointR = trapMap.getTrapezoid(idTrap).getIdPointR();
        trapezoidPointR = (idPointR == NO_ID) ? trapMap.getBoundingBox().max() : trapMapData.getPoint(idPointR);
        traversedTraps.push_back(idTrap);
    }
}

void addSegmentToTrapezoidalMap(const cg3::Segment2d& newSegment, TrapezoidalMapDataset& trapMapData,
                                gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag)
{
    // Finds the IDs of the new segment and of its endpoints
    bool found;
    size_t idNewSegment = trapMapData.findSegment(newSegment, found);
    size_t idNewSegmentP1 = trapMapData.findPoint(newSegment.p1(), found);
    size_t idNewSegmentP2 = trapMapData.findPoint(newSegment.p2(), found);

    // Find the trapezoids traversed by the new segment
    std::vector<size_t> traversedTraps = std::vector<size_t>();
    crossedTrapezoids(newSegment, trapMapData, trapMap, dag, traversedTraps);
    assert(traversedTraps.size() > 0);
    
    // Check the geometry conditions
    size_t idFirstTrapezoid = traversedTraps[0];
    size_t idLastTrapezoid = traversedTraps[traversedTraps.size()-1];
    gasprj::Trapezoid firstTrapezoid = trapMap.getTrapezoid(idFirstTrapezoid);
    gasprj::Trapezoid lastTrapezoid = trapMap.getTrapezoid(idLastTrapezoid);
    bool overlapL, overlapR, isLVertexTEnd, isLVertexBEnd, isRVertexTEnd, isRVertexBEnd;

    if (firstTrapezoid.getIdPointL() == NO_ID) overlapL = false;
    else overlapL = newSegment.p1() == trapMapData.getPoint(firstTrapezoid.getIdPointL());

    if (firstTrapezoid.getIdPointR() == NO_ID) overlapR = false;
    else overlapR = newSegment.p2() == trapMapData.getPoint(lastTrapezoid.getIdPointR());

    if (firstTrapezoid.getIdPointL() == NO_ID || firstTrapezoid.getIdSegmentT() == NO_ID) isLVertexTEnd = false;
    else isLVertexTEnd = trapMapData.getSegment(firstTrapezoid.getIdSegmentT()).p1() == trapMapData.getPoint(firstTrapezoid.getIdPointL());

    if (firstTrapezoid.getIdPointL() == NO_ID || firstTrapezoid.getIdSegmentB() == NO_ID) isLVertexBEnd = false;
    else isLVertexBEnd = trapMapData.getSegment(firstTrapezoid.getIdSegmentB()).p1() == trapMapData.getPoint(firstTrapezoid.getIdPointL());

    if (lastTrapezoid.getIdPointR() == NO_ID || lastTrapezoid.getIdSegmentT() == NO_ID) isRVertexTEnd = false;
    else isRVertexTEnd = trapMapData.getSegment(lastTrapezoid.getIdSegmentT()).p2() == trapMapData.getPoint(lastTrapezoid.getIdPointR());

    if (lastTrapezoid.getIdPointR() == NO_ID || lastTrapezoid.getIdSegmentB() == NO_ID) isRVertexBEnd = false;
    else isRVertexBEnd = trapMapData.getSegment(lastTrapezoid.getIdSegmentB()).p2() == trapMapData.getPoint(lastTrapezoid.getIdPointR());

    // Define the IDs for the new trapezoids that could to be created
    size_t idNewTrapT = NO_ID, idNewTrapB = NO_ID, idNewTrapL = NO_ID, idNewTrapR = NO_ID;

    // Define the IDs for the new nodes of the DAG that could to be created
    size_t idNewNodeXL = NO_ID, idNewNodeXR = NO_ID, idNewNodeY = NO_ID,
           idNewLeafT = NO_ID, idNewLeafB = NO_ID, idNewLeafL = NO_ID, idNewLeafR = NO_ID;

    // Support variable for IDs
    size_t idNext = NO_ID;

    // Trapezoid of interest
    size_t idTrapezoid;
    gasprj::Trapezoid trapezoid;

    // DAG leaf of interest
    size_t idLeaf;

    // Update the DAG and the trapezoidal map.
    // Behave differently wheter one or more than one trapezoid has been intersected by the new segment
    if (traversedTraps.size() == 1) {

        //--- Trapezoidal Map update

        // Trapezoid of interest
        idTrapezoid = idFirstTrapezoid;
        trapezoid = firstTrapezoid;

        // Assign the IDs to the new trapezoids

        // There always is a top trapezoid
        idNewTrapT = idTrapezoid;
        // There always is a bottom trapezoid
        idNewTrapB = trapMap.getNumberTrapezoids();
        // If the left point of the new segment do not overlap with the left point of the trapezoid,
        // then we have a left trapezoid
        if (!overlapL) idNewTrapL = idNewTrapB+1;
        // If the right point of the new segment do not overlap with the right point of the trapezoid,
        // then we have a right trapezoid
        if (!overlapR) idNewTrapR = idNewTrapL+1;


        // Create the new trapezoids and add them to the trapezoidal map
        gasprj::Trapezoid newTrap;

        // Top trapezoid
        newTrap = trapezoid;
        newTrap.setIdSegmentB(idNewSegment);
        newTrap.setIdPointL(idNewSegmentP1);
        newTrap.setIdPointR(idNewSegmentP2);
        newTrap.setIdTrapezoidBL(NO_ID);
        newTrap.setIdTrapezoidBR(NO_ID);
        if (!overlapL) newTrap.setIdTrapezoidTL(idNewTrapL);
        else if (isLVertexTEnd) newTrap.setIdTrapezoidTL(NO_ID);
        if (!overlapR) newTrap.setIdTrapezoidTR(idNewTrapR);
        else if (isRVertexTEnd) newTrap.setIdTrapezoidTR(NO_ID);
        newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
        trapMap.overwriteTrapezoid(newTrap, idNewTrapT);

        // Bottom trapezoid
        newTrap = trapezoid;
        newTrap.setIdSegmentT(idNewSegment);
        newTrap.setIdPointL(idNewSegmentP1);
        newTrap.setIdPointR(idNewSegmentP2);
        newTrap.setIdTrapezoidTL(NO_ID);
        newTrap.setIdTrapezoidTR(NO_ID);
        if (!overlapL) newTrap.setIdTrapezoidBL(idNewTrapL);
        else if (isLVertexBEnd) newTrap.setIdTrapezoidBL(NO_ID);
        if (!overlapR) newTrap.setIdTrapezoidBR(idNewTrapR);
        else if (isRVertexBEnd) newTrap.setIdTrapezoidBR(NO_ID);
        newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
        trapMap.addNewTrapezoid(newTrap);

        // Left trapezoid
        if (!overlapL) {
            newTrap = trapezoid;
            newTrap.setIdPointR(idNewSegmentP1);
            newTrap.setIdTrapezoidTR(idNewTrapT);
            newTrap.setIdTrapezoidBR(idNewTrapB);
            newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
            trapMap.addNewTrapezoid(newTrap);
        }

        // Right trapezoid
        if (!overlapR) {
            newTrap = trapezoid;
            newTrap.setIdPointL(idNewSegmentP2);
            newTrap.setIdTrapezoidTL(idNewTrapT);
            newTrap.setIdTrapezoidBL(idNewTrapB);
            newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
            trapMap.addNewTrapezoid(newTrap);
        }

        //--- End Trapezoidal Map update


        //--- DAG Update

        // DAG leaf of interest
        idLeaf = dag.findLeaf(idTrapezoid);

        // Assign the IDs to the new nodes of the DAG

        idNext = idLeaf;
        // If the left point of the new segment do not overlap with the left point of the trapezoid,
        // then we have an X node for the left point of the new segment and a leaf for the new left trapezoid
        if (!overlapL) idNewNodeXL = idLeaf, idNewLeafL = dag.getNumberNodes(), idNext = idNewLeafL+1;
        // If the right point of the new segment do not overlap with the right point of the trapezoid,
        // then we have an X node for the right point of the new segment and a leaf for the new right trapezoid
        if (!overlapR) {
            if (!overlapL) idNewNodeXR = idNewLeafL+1, idNewLeafR = idNewNodeXR+1;
            else idNewNodeXR = idLeaf, idNewLeafR = dag.getNumberNodes();
            idNext = idNewLeafR+1;
        }
        // There always is a Y node with the leaves for the new top and bottom trapezoids
        idNewNodeY = idNext, idNewLeafT = (overlapL && overlapR) ? dag.getNumberNodes() : idNewNodeY+1, idNewLeafB = idNewLeafT+1;


        // Create the new nodes and add them to the DAG
        gasprj::DAGNode newNode;

        // Structure related to the left trapezoid
        if (!overlapL) {
            newNode = DAGNode(gasprj::NodeType::XNode, idNewSegmentP1, idNewLeafL, overlapR ? idNewNodeY : idNewNodeXR);
            dag.overwriteNode(newNode, idNewNodeXL);
            newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapL, NO_ID, NO_ID);
            dag.addNewNode(newNode);
            dag.mapLeaf(idNewTrapL, idNewLeafL);
        }

        // Structure related to the right trapezoid
        if (!overlapR) {
            newNode = DAGNode(gasprj::NodeType::XNode, idNewSegmentP2, idNewNodeY, idNewLeafR);
            overlapL ? dag.overwriteNode(newNode, idNewNodeXR) : dag.addNewNode(newNode);
            newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapR, NO_ID, NO_ID);
            dag.addNewNode(newNode);
            dag.mapLeaf(idNewTrapR, idNewLeafR);
        }

        // Structure related to the top and bottom trapezoids
        newNode = DAGNode(gasprj::NodeType::YNode, idNewSegment, idNewLeafT, idNewLeafB);
        overlapL && overlapR ? dag.overwriteNode(newNode, idNewNodeY) : dag.addNewNode(newNode);
        newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapT, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        dag.remapLeaf(idNewTrapT, idNewLeafT);
        newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapB, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        dag.mapLeaf(idNewTrapB, idNewLeafB);

        //--- End DAG Update
    }
    else {

        // Position of the new segment with respect to the right point of the previous crossed trapezoid
        bool segmentPreviouslyBelow;

        // Define the IDs of the backup of the top and bottom trapezoids
        size_t idPreviousTrapT = NO_ID, idPreviousTrapB = NO_ID, idPreviousLeafT = NO_ID, idPreviousLeafB = NO_ID;


        //------ Leftmost trapezoid update

        //--- Trapezoidal Map update

        // Trapezoid of interest
        idTrapezoid = idFirstTrapezoid;
        trapezoid = firstTrapezoid;

        // Assign the IDs to the new trapezoids

        // If the left point of the new segment do not overlap with the left point of the trapezoid,
        // then we have a left trapezoid
        if (!overlapL) idNewTrapL = trapMap.getNumberTrapezoids();
        // There always is a top and bottom
        idNewTrapT = idTrapezoid, idNewTrapB = overlapL ? trapMap.getNumberTrapezoids() : idNewTrapL+1;

        // Create the new trapezoids
        gasprj::Trapezoid newTrap, newTrapT, newTrapB, previousTrapT, previousTrapB;

        // Top and bottom trapezoids (created but not added to the trapezoidal map yet)
        newTrapT = trapezoid;
        newTrapT.setIdSegmentB(idNewSegment);
        newTrapT.setIdPointL(idNewSegmentP1);
        newTrapT.setIdTrapezoidBL(NO_ID);
        if (!overlapL) newTrapT.setIdTrapezoidTL(idNewTrapL);
        else if (isLVertexTEnd) newTrapT.setIdTrapezoidTL(NO_ID);

        newTrapB = trapezoid;
        newTrapB.setIdSegmentT(idNewSegment);
        newTrapB.setIdPointL(idNewSegmentP1);
        newTrapB.setIdTrapezoidTL(NO_ID);
        if (!overlapL) newTrapB.setIdTrapezoidBL(idNewTrapL);
        else if (isLVertexBEnd) newTrapB.setIdTrapezoidBL(NO_ID);

        // Left trapezoid (added to the trapezoidal map)
        if (!overlapL) {
            newTrap = trapezoid;
            newTrap.setIdPointR(idNewSegmentP1);
            newTrap.setIdTrapezoidTR(idNewTrapT);
            newTrap.setIdTrapezoidBR(idNewTrapB);
            newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
            trapMap.addNewTrapezoid(newTrap);
        }

        //--- End of Trapezoidal Map update

        //--- DAG Update

        // DAG leaf of interest
        idLeaf = dag.findLeaf(idTrapezoid);

        // Assign the IDs to the new nodes of the DAG

        idNext = idLeaf;
        // If the left point of the new segment do not overlap with the left point of the trapezoid,
        // then we have an X node for the left point of the new segment and a leaf for the new left trapezoid
        if (!overlapL) idNewNodeXL = idLeaf, idNewLeafL = dag.getNumberNodes(), idNext = idNewLeafL+1;
        // There always is a Y node with the leaves for the next top and bottom trapezoids
        idNewNodeY = idNext, idNewLeafT = overlapL ? dag.getNumberNodes() : idNewNodeY+1, idNewLeafB = idNewLeafT+1;

        // Create the new nodes and add them to the DAG
        gasprj::DAGNode newNode, newLeafT, newLeafB;

        // Structure related to the left trapezoid
        if (!overlapL) {
            newNode = DAGNode(gasprj::NodeType::XNode, idNewSegmentP1, idNewLeafL, idNewNodeY);
            dag.overwriteNode(newNode, idNewNodeXL);
            newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapL, NO_ID, NO_ID);
            dag.addNewNode(newNode);
            dag.mapLeaf(idNewTrapL, idNewLeafL);
        }

        // Structure related to the top and bottom trapezoids
        newNode = DAGNode(gasprj::NodeType::YNode, idNewSegment, idNewLeafT, idNewLeafB);
        overlapL ? dag.overwriteNode(newNode, idNewNodeY) : dag.addNewNode(newNode);
        newLeafT = DAGNode(gasprj::NodeType::Leaf, idNewTrapT, NO_ID, NO_ID);
        dag.addNewNode(newLeafT);
        dag.remapLeaf(idNewTrapT, idNewLeafT);
        newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapB, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        dag.mapLeaf(idNewTrapB, idNewLeafB);

        //--- End of DAG update

        //------ End of leftmost trapezoid update


        //------ Center trapezoids update

        idPreviousTrapT = idNewTrapT, idPreviousTrapB = idNewTrapB;
        previousTrapT = newTrapT, previousTrapB = newTrapB;
        idPreviousLeafT = idNewLeafT, idPreviousLeafB = idNewLeafB;
        assert(trapezoid.getIdPointR() != NO_ID);
        segmentPreviouslyBelow = cg3::isPointAtLeft(newSegment, trapMapData.getPoint(trapezoid.getIdPointR()));;
        for (size_t i = 1; i < traversedTraps.size()-1; i++) {

            //--- Trapezoidal Map update

            // Trapezoid of interest
            idTrapezoid = traversedTraps[i];
            trapezoid = trapMap.getTrapezoid(idTrapezoid);

            // Create the new trapezoids and add the previous to the trapezoidal map

            if (segmentPreviouslyBelow) {
                // ID of the new top trapezoid
                idNewTrapT = idTrapezoid;
                // The previous top trapezoid has come to an end, add it to the trapezoidal map
                previousTrapT.setIdTrapezoidTR(NO_ID);
                previousTrapT.setIdTrapezoidBR(idNewTrapT);
                previousTrapT.setIdPointR(trapezoid.getIdPointL());
                previousTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
                if (idPreviousTrapT < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapT, idPreviousTrapT);
                else trapMap.addNewTrapezoid(previousTrapT);
                // Create the new top trapezoid
                newTrapT = trapezoid;
                newTrapT.setIdSegmentB(idNewSegment);
                newTrapT.setIdTrapezoidBL(idPreviousTrapT);                
                idPreviousTrapT = idNewTrapT;
                previousTrapT = newTrapT;
            }
            else {
                // ID of the new bottom trapezoid
                idNewTrapB = idTrapezoid;
                // The previous bottom trapezoid has come to an end, add it to the trapezoidal map
                previousTrapB.setIdTrapezoidTR(idNewTrapB);
                previousTrapB.setIdTrapezoidBR(NO_ID);
                previousTrapB.setIdPointR(trapezoid.getIdPointL());
                previousTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
                if (idPreviousTrapB < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapB, idPreviousTrapB);
                else trapMap.addNewTrapezoid(previousTrapB);
                // Create the new bottom trapezoid
                newTrapB = trapezoid;
                newTrapB.setIdSegmentT(idNewSegment);
                newTrapB.setIdTrapezoidTL(idPreviousTrapB);
                idPreviousTrapB = idNewTrapB;
                previousTrapB = newTrapB;
            }

            //--- End of Trapezoidal Map update

            //--- DAG Update

            // DAG leaf of interest
            idLeaf = dag.findLeaf(idTrapezoid);

            // Assign the IDs to the new nodes of the DAG

            idNewNodeY = idLeaf;
            if (segmentPreviouslyBelow) idNewLeafT = dag.getNumberNodes(), idNewLeafB = idPreviousLeafB;
            else idNewLeafT = idPreviousLeafT, idNewLeafB = dag.getNumberNodes();

            // Add the nodes to the DAG

            newNode = DAGNode(gasprj::NodeType::YNode, idNewSegment, idNewLeafT, idNewLeafB);
            dag.overwriteNode(newNode, idNewNodeY);
            if(segmentPreviouslyBelow) {
                // There is just a new top trapezoid, so just a new left leaf
                newNode = DAGNode(gasprj::NodeType::Leaf, idNewLeafT, NO_ID, NO_ID);
                dag.addNewNode(newNode);
                dag.remapLeaf(idNewTrapT, idNewLeafT);
            }
            else {
                // There is just a new bottom trapezoid, so just a new right leaf
                newNode = DAGNode(gasprj::NodeType::Leaf, idNewLeafB, NO_ID, NO_ID);
                dag.addNewNode(newNode);
                dag.remapLeaf(idNewTrapB, idNewLeafB);
            }

            //--- End of DAG update

            segmentPreviouslyBelow = cg3::isPointAtLeft(newSegment, trapMapData.getPoint(trapezoid.getIdPointR()));
        }

        //------ End of center trapezoids update

        //------ Rightmost trapezoid

        // Trapezoid of interest
        idTrapezoid = idLastTrapezoid;
        trapezoid = lastTrapezoid;

        //--- Trapezoidal Map update

        // Assign the ID to the new trapezoid
        if (!overlapR) idNewTrapR = std::max(idPreviousTrapT+1, std::max(idPreviousTrapB+1, trapMap.getNumberTrapezoids()));

        // Create all the remaining trapezoids and add them to the trapezoidal map

        if (segmentPreviouslyBelow) {
            // ID of the new top trapezoid
            idNewTrapT = idTrapezoid;
            // The previous top trapezoid has come to an end, add it to the trapezoidal map
            previousTrapT.setIdTrapezoidTR(NO_ID);
            previousTrapT.setIdTrapezoidBR(idNewTrapT);
            previousTrapT.setIdPointR(trapezoid.getIdPointL());
            previousTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
            if (idPreviousTrapT < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapT, idPreviousTrapT);
            else trapMap.addNewTrapezoid(previousTrapT);
            // The previous bottom trapezoid has come to an end too, add it to the trapezoidal map
            previousTrapB.setIdTrapezoidTR(NO_ID);
            previousTrapB.setIdPointR(idNewSegmentP2);
            if (!overlapR) previousTrapB.setIdTrapezoidBR(idNewTrapR);
            else if (isRVertexBEnd) previousTrapB.setIdTrapezoidBR(NO_ID);
            else previousTrapB.setIdTrapezoidBR(trapezoid.getIdTrapezoidBR());
            previousTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
            if (idPreviousTrapB < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapB, idPreviousTrapB);
            else trapMap.addNewTrapezoid(previousTrapB);
            // Create the new top trapezoid
            newTrapT = trapezoid;
            newTrapT.setIdSegmentB(idNewSegment);
            newTrapT.setIdPointR(idNewSegmentP2);
            newTrapT.setIdTrapezoidBL(idPreviousTrapT);
            newTrapT.setIdTrapezoidBR(NO_ID);
            if (!overlapR) newTrapT.setIdTrapezoidTR(idNewTrapR);
            else if (isRVertexTEnd) newTrapT.setIdTrapezoidTR(NO_ID);
            else newTrapT.setIdTrapezoidTR(trapezoid.getIdTrapezoidTR());
            newTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
            trapMap.overwriteTrapezoid(newTrapT, idNewTrapT);
            idPreviousTrapT = idNewTrapT;
            previousTrapT = newTrapT;
        }
        else {
            // ID of the new bottom trapezoid
            idNewTrapB = idTrapezoid;
            // The previous bottom trapezoid has come to an end, add it to the trapezoidal map
            previousTrapB.setIdTrapezoidTR(idNewTrapB);
            previousTrapB.setIdTrapezoidBR(NO_ID);
            previousTrapB.setIdPointR(trapezoid.getIdPointL());
            previousTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
            if (idPreviousTrapB < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapB, idPreviousTrapB);
            else trapMap.addNewTrapezoid(previousTrapB);
            // The previous top trapezoid has come to an end too, add it to the trapezoidal map
            previousTrapT.setIdTrapezoidBR(NO_ID);
            previousTrapT.setIdPointR(idNewSegmentP2);
            if (!overlapR) previousTrapT.setIdTrapezoidTR(idNewTrapR);
            else if (isRVertexTEnd) previousTrapT.setIdTrapezoidTR(NO_ID);
            else previousTrapT.setIdTrapezoidTR(trapezoid.getIdTrapezoidTR());
            previousTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
            if (idPreviousTrapT < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapT, idPreviousTrapT);
            else trapMap.addNewTrapezoid(previousTrapT);
            // Create the new bottom trapezoid
            newTrapB = trapezoid;
            newTrapB.setIdSegmentT(idNewSegment);
            newTrapB.setIdPointR(idNewSegmentP2);
            newTrapB.setIdTrapezoidTL(idPreviousTrapB);
            newTrapB.setIdTrapezoidTR(NO_ID);
            if (!overlapR) newTrapB.setIdTrapezoidBR(idNewTrapR);
            else if (isRVertexBEnd) newTrapB.setIdTrapezoidBR(NO_ID);
            else newTrapB.setIdTrapezoidBR(trapezoid.getIdTrapezoidBR());
            newTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
            trapMap.overwriteTrapezoid(newTrapB, idNewTrapB);
            idPreviousTrapB = idNewTrapB;
            previousTrapB = newTrapB;
        }
        // Right trapezoid
        if (!overlapR) {
            newTrap = trapezoid;
            newTrap.setIdPointL(idNewSegmentP2);
            newTrap.setIdTrapezoidTL(idPreviousTrapT);
            newTrap.setIdTrapezoidBL(idPreviousTrapB);
            newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
            trapMap.addNewTrapezoid(newTrap);
        }

        //--- End of Trapezoidal Map update

        //--- DAG Update

        // DAG leaf of interest
        idLeaf = dag.findLeaf(idTrapezoid);

        // Assign the IDs to the new nodes of the DAG

        idNext = idLeaf;
        // If the right point of the new segment do not overlap with the right point of the trapezoid,
        // then we have an X node for the right point of the new segment and a leaf for the new right trapezoid
        if (!overlapR) idNewNodeXR = idLeaf, idNewLeafR = dag.getNumberNodes(), idNext = idNewLeafR+1;
        // There always is a Y node with the leaves for the actual top and bottom trapezoids
        idNewNodeY = idNext, idNewLeafT = overlapL ? dag.getNumberNodes() : idNewNodeY+1, idNewLeafB = idNewLeafT+1;

        // Create the new nodes and add them to the DAG

        // Structure related to the right trapezoid
        if (!overlapR) {
            newNode = DAGNode(gasprj::NodeType::XNode, idNewSegmentP2, idNewNodeY, idNewLeafR);
            dag.overwriteNode(newNode, idNewNodeXR);
            newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapR, NO_ID, NO_ID);
            dag.addNewNode(newNode);
            dag.mapLeaf(idNewTrapR, idNewLeafR);
        }
        // Structure related to the top and bottom trapezoids
        newNode = DAGNode(gasprj::NodeType::YNode, idNewSegment, idNewLeafT, idNewLeafB);
        overlapR ? dag.overwriteNode(newNode, idNewNodeY) : dag.addNewNode(newNode);

        newNode = DAGNode(gasprj::NodeType::Leaf, idPreviousTrapT, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        if (idPreviousTrapT < trapMap.getNumberTrapezoids()) dag.remapLeaf(idPreviousTrapT, idNewLeafT);
        else dag.mapLeaf(idPreviousTrapT, idNewLeafT);

        newNode = DAGNode(gasprj::NodeType::Leaf, idPreviousTrapB, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        if (idPreviousTrapB < trapMap.getNumberTrapezoids()) dag.remapLeaf(idPreviousTrapB, idNewLeafB);
        else dag.mapLeaf(idPreviousTrapB, idNewLeafB);

        //--- End of DAG Update

        //------ End of rightmost trapezoid update
    }
}

} // End namespace gasprj
