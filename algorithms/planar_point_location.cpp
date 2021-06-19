#include "planar_point_location.h"

#include<algorithm>

#include <cg3/geometry/utils2.h>

namespace gasprj {

/* Builders */

void initTrapezoidalMap(TrapezoidalMap &trapMap, DAG &dag)
{
    assert(trapMap.size() == 0 && dag.size() == 0);

    Trapezoid boundingBoxTrap = Trapezoid();
    boundingBoxTrap.setIdDagLeaf(0);
    DAG::Node boundingBoxNode = DAG::Node(DAG::Node::Type::Leaf, 0, DAG::Node::NO_ID, DAG::Node::NO_ID);

    trapMap.addTrapezoid(boundingBoxTrap);
    dag.addNode(boundingBoxNode);
}

void addSegmentToTrapezoidalMap(const cg3::Segment2d &segment, TrapezoidalMap &trapMap, DAG &dag)
{           
    // Order the segment and its endpoints
    cg3::Segment2d orderedSegment;
    if (segment.p1().x() > segment.p2().x()) {
        orderedSegment = cg3::Segment2d(segment.p2(), segment.p1());
    }
    else {
        orderedSegment = segment;
    }
    assert(orderedSegment.p1().x() < orderedSegment.p2().x());

    // Find the trapezoids crossed by the new segment
    std::vector<size_t> crossedTraps = std::vector<size_t>();
    crossedTrapezoids(orderedSegment, trapMap, dag, crossedTraps);
    assert(crossedTraps.size() > 0);

    /*
     * Update the trapezoidal map and the DAG, updating all the crossed trapezoids and their corresponding DAG leaves.
     * Behave differently wheter one or more than one trapezoid has been crossed by the new segment
     */

    // New segment lying entirely in one trapezoid
    if (crossedTraps.size() == 1)
        updateOneCrossedTrapezoid(orderedSegment, crossedTraps[0], trapMap, dag);

    // New segment crossing two or more trapezoids
    else
        updateMoreCrossedTrapezoids(orderedSegment, crossedTraps, trapMap, dag);
}

/* Query */

size_t queryTrapezoidalMap(const cg3::Point2d &point, const TrapezoidalMap trapMap, const DAG &dag)
{
    // Trapezoidal map dataset
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    const DAG::Node *dagNode = &dag.getRoot();
    // Scroll the DAG until a leaf is reached
    while(dagNode->getType() != DAG::Node::Type::Leaf) {
        // Check if the actual node is a X-node (endpoint) or a Y-node (segment)
        switch(dagNode->getType()) {
            // Point-Endpoint comparison
            case DAG::Node::Type::XNode: {
                const cg3::Point2d &endpoint = trapMapData.getPoint(dagNode->getIdInfo());
                // Query point to the left of the segment endpoint
                if (point.x() < endpoint.x()) {
                    dagNode = &dag.getNode(dagNode->getIdNodeL());
                }
                // Query point either to the right or in the same vertical extension of the endpoint:
                // in both cases we treat it as being at the right (for real or conceptually)
                else {
                    dagNode = &dag.getNode(dagNode->getIdNodeR());
                }
                break;
            }
            // Point-Segment comparison
            case DAG::Node::Type::YNode: {
                const cg3::Segment2d &segment = trapMapData.getSegment(dagNode->getIdInfo());
                cg3::Segment2d orderedSegment;
                if (segment.p1().x() > segment.p2().x()) orderedSegment = cg3::Segment2d(segment.p2(), segment.p1());
                else orderedSegment = segment;

                // Query point above the segment
                if (cg3::isPointAtLeft(orderedSegment, point)) {
                    assert(orderedSegment.p1() != point);
                    dagNode = &dag.getNode(dagNode->getIdNodeL());
                }
                #ifdef NDEBUG
                // Query point below the segment
                else {
                    assert(orderedSegment.p1() != point);
                    dagNode = &dag.getNode(dagNode->getIdNodeR());
                }
                #else
                // Query point below the segment
                else if (cg3::isPointAtRight(orderedSegment, point)) {
                    assert(orderedSegment.p1() != point);
                    dagNode = &dag.getNode(dagNode->getIdNodeR());
                }
                // Query point lying on the segment
                else {
                    assert(false);
                }
                #endif
                break;
            }
            default: {
                // Unreachable: should not reach this with a leaf
                assert(false);
            }
        }
    }

    // At this point the node must be a leaf
    assert(dagNode->getType() == DAG::Node::Type::Leaf);

    // Return the index of the trapezoid
    return dagNode->getIdInfo();
}



/* Helper functions */

void updateOneCrossedTrapezoid(const cg3::Segment2d &segment, size_t idCrossedTrap, TrapezoidalMap &trapMap, DAG &dag)
{
    // Trapezoidal map dataset (could be const, but the 'find' method is not declared const unfortunately)
    TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    // Define the variables for all the IDs, trapezoids and nodes that will be computed
    size_t idAvail;                                                           // Support variable for next available ID
    size_t idTrapT, idTrapB, idTrapL, idTrapR;                                // New trapezoid IDs
    size_t idNodeXL, idNodeXR, idNodeY, idLeafT, idLeafB, idLeafL, idLeafR;   // New DAG leaf and node IDs
    Trapezoid trap; DAG::Node node;                                           // New trapezoid and node variables

    // Find the IDs of the segment and its endpoints
    bool found;
    size_t idSegment = trapMapData.findSegment(segment, found); assert(found);
    size_t idEndpointL = trapMapData.findPoint(segment.p1(), found); assert(found);
    size_t idEndpointR = trapMapData.findPoint(segment.p2(), found); assert(found);

    // Get a copy of the crossed trapezoid
    const Trapezoid crossedTrap = trapMap.getTrapezoid(idCrossedTrap);

    // Check the geomeytry conditions
    bool gNotOverlapL = !doesOverlapL(segment, idCrossedTrap, trapMap);
    bool gNotOverlapR = !doesOverlapR(segment, idCrossedTrap, trapMap);
    bool gEndpointTL = hasEndpointTL(idCrossedTrap, trapMap);
    bool gEndpointBL = hasEndpointBL(idCrossedTrap, trapMap);
    bool gEndpointTR = hasEndpointTR(idCrossedTrap, trapMap);
    bool gEndpointBR = hasEndpointBR(idCrossedTrap, trapMap);

    // Define the IDs of the new trapezoids
    idTrapT = idCrossedTrap;                         // Mandatory top trapezoid
    idTrapB = trapMap.size(), idAvail = idTrapB+1;   // Mandatory bottom trapezoid
    if (gNotOverlapL) idTrapL = idAvail++;           // Optional left trapezoid
    if (gNotOverlapR) idTrapR = idAvail;             // Optional right trapezoid

    // Define the IDs of the new DAG nodes
    idAvail = crossedTrap.getIdDagLeaf();            // First ID (trapezoid leaf ID)
    if (gNotOverlapL)                                // Optional sub-graph for left trapezoid
        idNodeXL = idAvail, idLeafL = dag.size(), idAvail = idLeafL+1;
    if (gNotOverlapR)                                // Optional sub-graph for right trapezoid
        idNodeXR = idAvail, idLeafR = gNotOverlapL ? idNodeXR+1 : dag.size(), idAvail = idLeafR+1;
    idNodeY = idAvail,                               // Mandatory sub-graph for top and bottom trapezoids
    idLeafT = gNotOverlapL || gNotOverlapR ? idNodeY+1 : dag.size(), idLeafB = idLeafT+1;

    // Update the adjacencies of the neighboor trapezoids
    if (crossedTrap.getIdAdjacencyTL() != Trapezoid::NO_ID)   // Top-left adjacency
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyTL()).setIdAdjacencyTR(gNotOverlapL ? idTrapL : idTrapT);
    if (crossedTrap.getIdAdjacencyTR() != Trapezoid::NO_ID)   // Top-right adjacency
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyTR()).setIdAdjacencyTL(gNotOverlapR ? idTrapR : idTrapT);
    if (crossedTrap.getIdAdjacencyBL() != Trapezoid::NO_ID)   // Bottom-left adjacency
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyBL()).setIdAdjacencyBR(gNotOverlapL ? idTrapL : idTrapB);
    if (crossedTrap.getIdAdjacencyBR() != Trapezoid::NO_ID)   // Bottom-right adjacency
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyBR()).setIdAdjacencyBL(gNotOverlapR ? idTrapR : idTrapB);

    /* Update the trapezoidal map */

    // Mandatory top trapezoid
    trap = Trapezoid(
        crossedTrap.getIdSegmentT(), idSegment, idEndpointL, idEndpointR,                           // Identifiers
        gNotOverlapL ? idTrapL : gEndpointTL ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyTL(),   // Top-left adjacency
        gNotOverlapR ? idTrapR : gEndpointTR ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyTR(),   // Top-right adjacency
        Trapezoid::NO_ID, Trapezoid::NO_ID,                                                         // Bottom adjacencies
        idLeafT);                                                                                   // DAG leaf
    trapMap.overwriteTrapezoid(trap, idTrapT);

    // Mandatory bottom trapezoid
    trap = Trapezoid(
        idSegment, crossedTrap.getIdSegmentB(), idEndpointL, idEndpointR,                           // Identifiers
        Trapezoid::NO_ID, Trapezoid::NO_ID,                                                         // Top adjacencies
        gNotOverlapL ? idTrapL : gEndpointBL ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyBL(),   // Bottom-left adjacency
        gNotOverlapR ? idTrapR : gEndpointBR ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyBR(),   // Bottom-right adjacency
        idLeafB);                                                                                   // DAG leaf
    trapMap.addTrapezoid(trap);

    // Optional left trapezoid
    if (gNotOverlapL) {
        trap = Trapezoid(
            crossedTrap.getIdSegmentT(), crossedTrap.getIdSegmentB(), crossedTrap.getIdPointL(), idEndpointL,   // Identifiers
            crossedTrap.getIdAdjacencyTL(), idTrapT, crossedTrap.getIdAdjacencyBL(), idTrapB, idLeafL);         // Adjacencies and DAG leaf
        trapMap.addTrapezoid(trap);
    }

    // Optional right trapezoid
    if (gNotOverlapR) {
        trap = Trapezoid(
            crossedTrap.getIdSegmentT(), crossedTrap.getIdSegmentB(), idEndpointR, crossedTrap.getIdPointR(),   // Identifiers
            idTrapT, crossedTrap.getIdAdjacencyTR(), idTrapB, crossedTrap.getIdAdjacencyBR(), idLeafR);         // Adjacencies and DAG leaf
        trapMap.addTrapezoid(trap);
    }

    /* Update the DAG */

    // Optional sub-graph for left trapezoid
    if (gNotOverlapL) {
        node = DAG::Node(DAG::Node::Type::XNode, idEndpointL, idLeafL, gNotOverlapR ? idNodeXR : idNodeY);
        dag.overwriteNode(node, idNodeXL);
        node = DAG::Node(DAG::Node::Type::Leaf, idTrapL, DAG::Node::NO_ID, DAG::Node::NO_ID);
        dag.addNode(node);
    }

    // Optional sub-graph for right trapezoid
    if (gNotOverlapR) {
        node = DAG::Node(DAG::Node::Type::XNode, idEndpointR, idNodeY, idLeafR);
        gNotOverlapL ? dag.addNode(node) : dag.overwriteNode(node, idNodeXR);
        node = DAG::Node(DAG::Node::Type::Leaf, idTrapR, DAG::Node::NO_ID, DAG::Node::NO_ID);
        dag.addNode(node);
    }

    // Mandatory sub-graph for top and bottom trapezoids
    node = DAG::Node(DAG::Node::Type::YNode, idSegment, idLeafT, idLeafB);
    gNotOverlapL || gNotOverlapR ? dag.addNode(node) : dag.overwriteNode(node, idNodeY);
    node = DAG::Node(DAG::Node::Type::Leaf, idTrapT, DAG::Node::NO_ID, DAG::Node::NO_ID);
    dag.addNode(node);
    node = DAG::Node(DAG::Node::Type::Leaf, idTrapB, DAG::Node::NO_ID, DAG::Node::NO_ID);
    dag.addNode(node);
}

void updateMoreCrossedTrapezoids(const cg3::Segment2d &segment,  const std::vector<size_t> &crossedTraps,
                                 TrapezoidalMap &trapMap, DAG &dag)
{
    TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    // Finds the IDs of the new segment and of its endpoints
    bool found;
    size_t idNewSegment = trapMapData.findSegment(newSegment, found);
    size_t idNewSegmentPL = trapMapData.findPoint(newSegment.p1(), found);
    size_t idNewSegmentPR = trapMapData.findPoint(newSegment.p2(), found);

    // Check the geomeytry conditions
    bool overlapL = !doesOverlapL(newSegment, idCrossedTrap, trapMap);
    bool overlapR = !doesOverlapR(newSegment, idCrossedTrap, trapMap);
    bool isLVertexTEnd = hasEndpointTL(idCrossedTrap, trapMap);
    bool isLVertexBEnd = hasEndpointBL(idCrossedTrap, trapMap);
    bool isRVertexTEnd = hasEndpointTR(idCrossedTrap, trapMap);
    bool isRVertexBEnd = hasEndpointBR(idCrossedTrap, trapMap);

    // Define the IDs for the new trapezoids that could to be created
    size_t idNewTrapT = NO_ID, idNewTrapB = NO_ID, idNewTrapL = NO_ID, idNewTrapR = NO_ID;

    // Define the IDs for the new nodes of the DAG that could to be created
    size_t idNewNodeXL = NO_ID, idNewNodeXR = NO_ID, idNewNodeY = NO_ID,
           idNewLeafT = NO_ID, idNewLeafB = NO_ID, idNewLeafL = NO_ID, idNewLeafR = NO_ID;

    // Support variable for IDs
    size_t idNext = NO_ID;

    // Trapezoid of interest
    size_t idTrap, idPreviousTrap;
    gasprj::Trapezoid trap, previousTrap;

    // DAG support variables
    size_t idLeaf;
    gasprj::DAGNode newNode;

    // Position of the new segment with respect to the right point of the previous crossed trapezoid
    bool segmentPreviouslyBelow;

    // Define the IDs of the backup of the top and bottom trapezoids
    size_t idPreviousTrapT = NO_ID, idPreviousTrapB = NO_ID, idPreviousLeafT = NO_ID, idPreviousLeafB = NO_ID;


    //------ Leftmost trapezoid update

    //--- Trapezoidal Map update

    // Trapezoid of interest
    idTrap = idFirstTrapezoid;
    trap = firstTrapezoid;

    // Assign the IDs to the new trapezoids

    // If the left point of the new segment do not overlap with the left point of the trapezoid,
    // then we have a left trapezoid
    if (!overlapL) idNewTrapL = trapMap.getNumberTrapezoids();
    // There always is a top and bottom
    idNewTrapT = idTrap, idNewTrapB = overlapL ? trapMap.getNumberTrapezoids() : idNewTrapL+1;

    // Update the adjacencies of the old neighbors of the old trapezoid
    if (trap.getIdTrapezoidTL() != NO_ID)
        trapMap.getTrapezoid(trap.getIdTrapezoidTL()).setIdTrapezoidTR(overlapL ? idNewTrapT : idNewTrapL);
    if (trap.getIdTrapezoidBL() != NO_ID)
        trapMap.getTrapezoid(trap.getIdTrapezoidBL()).setIdTrapezoidBR(overlapL ? idNewTrapB : idNewTrapL);

    // Create the new trapezoids
    gasprj::Trapezoid newTrap, newTrapT, newTrapB, previousTrapT, previousTrapB;

    // Top and bottom trapezoids (created but not added to the trapezoidal map yet)
    newTrapT = trap;
    newTrapT.setIdSegmentB(idNewSegment);
    newTrapT.setIdPointL(idNewSegmentPL);
    newTrapT.setIdTrapezoidBL(NO_ID);
    if (!overlapL) newTrapT.setIdTrapezoidTL(idNewTrapL);
    else if (isLVertexTEnd) newTrapT.setIdTrapezoidTL(NO_ID);

    newTrapB = trap;
    newTrapB.setIdSegmentT(idNewSegment);
    newTrapB.setIdPointL(idNewSegmentPL);
    newTrapB.setIdTrapezoidTL(NO_ID);
    if (!overlapL) newTrapB.setIdTrapezoidBL(idNewTrapL);
    else if (isLVertexBEnd) newTrapB.setIdTrapezoidBL(NO_ID);

    // Left trapezoid (added to the trapezoidal map)
    if (!overlapL) {
        newTrap = trap;
        newTrap.setIdPointR(idNewSegmentPL);
        newTrap.setIdTrapezoidTR(idNewTrapT);
        newTrap.setIdTrapezoidBR(idNewTrapB);
        newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
        trapMap.addNewTrapezoid(newTrap);
    }

    //--- End of Trapezoidal Map update

    //--- DAG Update

    // DAG leaf of interest
    idLeaf = dag.findLeaf(idTrap);

    // Assign the IDs to the new nodes of the DAG

    idNext = idLeaf;
    // If the left point of the new segment do not overlap with the left point of the trapezoid,
    // then we have an X node for the left point of the new segment and a leaf for the new left trapezoid
    if (!overlapL) idNewNodeXL = idLeaf, idNewLeafL = dag.getNumberNodes(), idNext = idNewLeafL+1;
    // There always is a Y node with the leaves for the next top and bottom trapezoids
    idNewNodeY = idNext, idNewLeafT = overlapL ? dag.getNumberNodes() : idNewNodeY+1, idNewLeafB = idNewLeafT+1;

    // Create the new nodes and add them to the DAG

    // Structure related to the left trapezoid
    if (!overlapL) {
        newNode = DAGNode(gasprj::NodeType::XNode, idNewSegmentPL, idNewLeafL, idNewNodeY);
        dag.overwriteNode(newNode, idNewNodeXL);
        newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapL, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        dag.mapLeaf(idNewTrapL, idNewLeafL);
    }

    // Structure related to the top and bottom trapezoids
    newNode = DAGNode(gasprj::NodeType::YNode, idNewSegment, idNewLeafT, idNewLeafB);
    overlapL ? dag.overwriteNode(newNode, idNewNodeY) : dag.addNewNode(newNode);
    newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapT, NO_ID, NO_ID);
    dag.addNewNode(newNode);
    dag.remapLeaf(idNewTrapT, idNewLeafT);
    newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapB, NO_ID, NO_ID);
    dag.addNewNode(newNode);
    dag.mapLeaf(idNewTrapB, idNewLeafB);

    //--- End of DAG update

    //------ End of leftmost trapezoid update


    //------ Center trapezoids update

    idPreviousTrap = idTrap, idPreviousTrapT = idNewTrapT, idPreviousTrapB = idNewTrapB;
    previousTrap = trap, previousTrapT = newTrapT, previousTrapB = newTrapB;
    idPreviousLeafT = idNewLeafT, idPreviousLeafB = idNewLeafB;
    assert(trap.getIdPointR() != NO_ID);
    segmentPreviouslyBelow = cg3::isPointAtLeft(orderedNewSegment, trapMapData.getPoint(trap.getIdPointR()));;
    for (size_t i = 1; i < traversedTraps.size()-1; i++) {

        //--- Trapezoidal Map update

        // Trapezoid of interest
        idTrap = traversedTraps[i];
        trap = trapMap.getTrapezoid(idTrap);

        // Create the new trapezoids and add the previous to the trapezoidal map

        if (segmentPreviouslyBelow) {
            // ID of the new top trapezoid
            idNewTrapT = idTrap;
            // The previous top trapezoid has come to an end, add it to the trapezoidal map
            previousTrapT.setIdTrapezoidTR(previousTrap.getIdTrapezoidTR());
            previousTrapT.setIdTrapezoidBR(idNewTrapT);
            previousTrapT.setIdPointR(trap.getIdPointL());
            previousTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
            if (idPreviousTrapT < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapT, idPreviousTrapT);
            else trapMap.addNewTrapezoid(previousTrapT);
            // Update the adjacencies of the old neighbors of the old trapezoid
            if (previousTrap.getIdTrapezoidTR() != NO_ID)
                trapMap.getTrapezoid(previousTrap.getIdTrapezoidTR()).setIdTrapezoidTL(idPreviousTrapT);
            // Create the new top trapezoid
            newTrapT = trap;
            newTrapT.setIdSegmentB(idNewSegment);
            newTrapT.setIdTrapezoidBL(idPreviousTrapT);
            idPreviousTrapT = idNewTrapT;
            previousTrapT = newTrapT;
        }
        else {
            // ID of the new bottom trapezoid
            idNewTrapB = idTrap;
            // The previous bottom trapezoid has come to an end, add it to the trapezoidal map
            previousTrapB.setIdTrapezoidTR(idNewTrapB);
            previousTrapB.setIdTrapezoidBR(previousTrap.getIdTrapezoidBR());
            previousTrapB.setIdPointR(trap.getIdPointL());
            previousTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
            if (idPreviousTrapB < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapB, idPreviousTrapB);
            else trapMap.addNewTrapezoid(previousTrapB);
            // Update the adjacencies of the old neighbors of the old trapezoid
            if (previousTrap.getIdTrapezoidBR() != NO_ID)
                trapMap.getTrapezoid(previousTrap.getIdTrapezoidBR()).setIdTrapezoidBL(idPreviousTrapB);
            // Create the new bottom trapezoid
            newTrapB = trap;
            newTrapB.setIdSegmentT(idNewSegment);
            newTrapB.setIdTrapezoidTL(idPreviousTrapB);
            idPreviousTrapB = idNewTrapB;
            previousTrapB = newTrapB;
        }

        //--- End of Trapezoidal Map update

        //--- DAG Update

        // DAG leaf of interest
        idLeaf = dag.findLeaf(idTrap);

        // Assign the IDs to the new nodes of the DAG

        idNewNodeY = idLeaf;
        if (segmentPreviouslyBelow) idNewLeafT = dag.getNumberNodes(), idNewLeafB = idPreviousLeafB;
        else idNewLeafT = idPreviousLeafT, idNewLeafB = dag.getNumberNodes();

        // Add the nodes to the DAG

        newNode = DAGNode(gasprj::NodeType::YNode, idNewSegment, idNewLeafT, idNewLeafB);
        dag.overwriteNode(newNode, idNewNodeY);
        if(segmentPreviouslyBelow) {
            // There is just a new top trapezoid, so just a new left leaf
            newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapT, NO_ID, NO_ID);
            dag.addNewNode(newNode);
            dag.remapLeaf(idNewTrapT, idNewLeafT);
            idPreviousLeafT = idNewLeafT;
        }
        else {
            // There is just a new bottom trapezoid, so just a new right leaf
            newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapB, NO_ID, NO_ID);
            dag.addNewNode(newNode);
            dag.remapLeaf(idNewTrapB, idNewLeafB);
            idPreviousLeafB = idNewLeafB;
        }

        //--- End of DAG update

        previousTrap = trap, idPreviousTrap = idTrap;
        segmentPreviouslyBelow = cg3::isPointAtLeft(orderedNewSegment, trapMapData.getPoint(trap.getIdPointR()));
    }

    //------ End of center trapezoids update

    //------ Rightmost trapezoid

    // Trapezoid of interest
    idTrap = idLastTrapezoid;
    trap = lastTrapezoid;

    //--- Trapezoidal Map update

    // Assign the ID to the new trapezoid
    if (!overlapR) idNewTrapR = std::max(idPreviousTrapT+1, std::max(idPreviousTrapB+1, trapMap.getNumberTrapezoids()));

    // Create all the remaining trapezoids and add them to the trapezoidal map

    if (segmentPreviouslyBelow) {
        // ID of the new top trapezoid
        idNewTrapT = idTrap;
        // The previous top trapezoid has come to an end, add it to the trapezoidal map
        previousTrapT.setIdTrapezoidTR(previousTrap.getIdTrapezoidTR());
        previousTrapT.setIdTrapezoidBR(idNewTrapT);
        previousTrapT.setIdPointR(trap.getIdPointL());
        previousTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
        if (idPreviousTrapT < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapT, idPreviousTrapT);
        else trapMap.addNewTrapezoid(previousTrapT);
        // Update the adjacencies of the old neighbors of the old trapezoid
        if (previousTrap.getIdTrapezoidTR() != NO_ID)
            trapMap.getTrapezoid(previousTrap.getIdTrapezoidTR()).setIdTrapezoidTL(idPreviousTrapT);
        // The previous bottom trapezoid has come to an end too, add it to the trapezoidal map
        previousTrapB.setIdTrapezoidTR(NO_ID);
        previousTrapB.setIdPointR(idNewSegmentPR);
        if (!overlapR) previousTrapB.setIdTrapezoidBR(idNewTrapR);
        else if (isRVertexBEnd) previousTrapB.setIdTrapezoidBR(NO_ID);
        else previousTrapB.setIdTrapezoidBR(trap.getIdTrapezoidBR());
        previousTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
        if (idPreviousTrapB < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapB, idPreviousTrapB);
        else trapMap.addNewTrapezoid(previousTrapB);
        // Update the adjacencies of the old neighbors of the old trapezoid
        if (previousTrap.getIdTrapezoidBR() != NO_ID)
            trapMap.getTrapezoid(previousTrap.getIdTrapezoidBR()).setIdTrapezoidBL(idPreviousTrapB);
        // Create the new top trapezoid
        newTrapT = trap;
        newTrapT.setIdSegmentB(idNewSegment);
        newTrapT.setIdPointR(idNewSegmentPR);
        newTrapT.setIdTrapezoidBL(idPreviousTrapT);
        newTrapT.setIdTrapezoidBR(NO_ID);
        if (!overlapR) newTrapT.setIdTrapezoidTR(idNewTrapR);
        else if (isRVertexTEnd) newTrapT.setIdTrapezoidTR(NO_ID);
        else newTrapT.setIdTrapezoidTR(trap.getIdTrapezoidTR());
        newTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
        trapMap.overwriteTrapezoid(newTrapT, idNewTrapT);
        idPreviousTrapT = idNewTrapT;
        previousTrapT = newTrapT;
    }
    else {
        // ID of the new bottom trapezoid
        idNewTrapB = idTrap;
        // The previous bottom trapezoid has come to an end, add it to the trapezoidal map
        previousTrapB.setIdTrapezoidTR(idNewTrapB);
        previousTrapB.setIdTrapezoidBR(previousTrap.getIdTrapezoidBR());
        previousTrapB.setIdPointR(trap.getIdPointL());
        previousTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
        if (idPreviousTrapB < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapB, idPreviousTrapB);
        else trapMap.addNewTrapezoid(previousTrapB);
        // Update the adjacencies of the old neighbors of the old trapezoid
        if (previousTrap.getIdTrapezoidBR() != NO_ID)
            trapMap.getTrapezoid(previousTrap.getIdTrapezoidBR()).setIdTrapezoidBL(idPreviousTrapB);
        // The previous top trapezoid has come to an end too, add it to the trapezoidal map
        previousTrapT.setIdTrapezoidBR(NO_ID);
        previousTrapT.setIdPointR(idNewSegmentPR);
        if (!overlapR) previousTrapT.setIdTrapezoidTR(idNewTrapR);
        else if (isRVertexTEnd) previousTrapT.setIdTrapezoidTR(NO_ID);
        else previousTrapT.setIdTrapezoidTR(trap.getIdTrapezoidTR());
        previousTrapT.updateVertices(trapMapData, trapMap.getBoundingBox());
        if (idPreviousTrapT < trapMap.getNumberTrapezoids()) trapMap.overwriteTrapezoid(previousTrapT, idPreviousTrapT);
        else trapMap.addNewTrapezoid(previousTrapT);
        // Update the adjacencies of the old neighbors of the old trapezoid
        if (previousTrap.getIdTrapezoidTR() != NO_ID)
            trapMap.getTrapezoid(previousTrap.getIdTrapezoidTR()).setIdTrapezoidTL(idPreviousTrapT);
        // Create the new bottom trapezoid
        newTrapB = trap;
        newTrapB.setIdSegmentT(idNewSegment);
        newTrapB.setIdPointR(idNewSegmentPR);
        newTrapB.setIdTrapezoidTL(idPreviousTrapB);
        newTrapB.setIdTrapezoidTR(NO_ID);
        if (!overlapR) newTrapB.setIdTrapezoidBR(idNewTrapR);
        else if (isRVertexBEnd) newTrapB.setIdTrapezoidBR(NO_ID);
        else newTrapB.setIdTrapezoidBR(trap.getIdTrapezoidBR());
        newTrapB.updateVertices(trapMapData, trapMap.getBoundingBox());
        trapMap.overwriteTrapezoid(newTrapB, idNewTrapB);
        idPreviousTrapB = idNewTrapB;
        previousTrapB = newTrapB;
    }
    // Right trapezoid
    if (!overlapR) {
        newTrap = trap;
        newTrap.setIdPointL(idNewSegmentPR);
        newTrap.setIdTrapezoidTL(idPreviousTrapT);
        newTrap.setIdTrapezoidBL(idPreviousTrapB);
        newTrap.updateVertices(trapMapData, trapMap.getBoundingBox());
        trapMap.addNewTrapezoid(newTrap);
    }

    // Update the adjacencies of the old neighbors of the old trapezoid
    if (trap.getIdTrapezoidTR() != NO_ID)
        trapMap.getTrapezoid(trap.getIdTrapezoidTR()).setIdTrapezoidTL(overlapR ? idNewTrapT : idNewTrapR);
    if (trap.getIdTrapezoidBR() != NO_ID)
        trapMap.getTrapezoid(trap.getIdTrapezoidBR()).setIdTrapezoidBL(overlapR ? idNewTrapB : idNewTrapR);

    //--- End of Trapezoidal Map update

    //--- DAG Update

    // DAG leaf of interest
    idLeaf = dag.findLeaf(idTrap);

    // Assign the IDs to the new nodes of the DAG

    idNext = idLeaf;
    // If the right point of the new segment do not overlap with the right point of the trapezoid,
    // then we have an X node for the right point of the new segment and a leaf for the new right trapezoid
    if (!overlapR) idNewNodeXR = idLeaf, idNewLeafR = dag.getNumberNodes(), idNext = idNewLeafR+1;
    // There always is a Y node with the leaves for the actual top and bottom trapezoids
    idNewNodeY = idNext, idNext = overlapR ? dag.getNumberNodes() : idNewNodeY+1;
    if (segmentPreviouslyBelow) idNewLeafT = idNext, idNewLeafB = idPreviousLeafB;
    else idNewLeafT = idPreviousLeafT, idNewLeafB = idNext;

    // Create the new nodes and add them to the DAG

    // Structure related to the right trapezoid
    if (!overlapR) {
        newNode = DAGNode(gasprj::NodeType::XNode, idNewSegmentPR, idNewNodeY, idNewLeafR);
        dag.overwriteNode(newNode, idNewNodeXR);
        newNode = DAGNode(gasprj::NodeType::Leaf, idNewTrapR, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        dag.mapLeaf(idNewTrapR, idNewLeafR);
    }
    // Structure related to the top and bottom trapezoids
    newNode = DAGNode(gasprj::NodeType::YNode, idNewSegment, idNewLeafT, idNewLeafB);
    overlapR ? dag.overwriteNode(newNode, idNewNodeY) : dag.addNewNode(newNode);

    if (segmentPreviouslyBelow) {
        newNode = DAGNode(gasprj::NodeType::Leaf, idPreviousTrapT, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        dag.remapLeaf(idPreviousTrapT, idNewLeafT);
    }
    else {
        newNode = DAGNode(gasprj::NodeType::Leaf, idPreviousTrapB, NO_ID, NO_ID);
        dag.addNewNode(newNode);
        dag.remapLeaf(idPreviousTrapB, idNewLeafB);
    }

    //--- End of DAG Update

    //------ End of rightmost trapezoid update
}

void crossedTrapezoids(const cg3::Segment2d &segment, const TrapezoidalMap &trapMap, const DAG &dag,
                       std::vector<size_t> &crossedTraps)
{
    assert(segment.p1().x() < segment.p2().x());
    assert(crossedTraps.size() == 0);

    // Trapezoidal map dataset
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    // Search for the trapezoid in which the left endpoint of the segment lies
    size_t idTrap = queryToBuildTrapezoidalMap(segment, trapMap, dag);
    // Get the right point of the actual trapezoid (if there is no right point, take the bounding box corner)
    size_t idPointR = trapMap.getTrapezoid(idTrap).getIdPointR();
    cg3::Point2d trapezoidPointR =
            (idPointR == Trapezoid::NO_ID) ? trapMap.getBoundingBox().max() : trapMapData.getPoint(idPointR);

    // Store the trapezoid
    crossedTraps.push_back(idTrap);

    // Search for all the crossed trapezoids and save their IDs in the vector
    while(segment.p2().x() > trapezoidPointR.x()) {
        // If the right point of the trapezoid lies above the segment, move to the bottom-right adjacency
        if (cg3::isPointAtLeft(segment, trapezoidPointR)) {
            idTrap = trapMap.getTrapezoid(idTrap).getIdAdjacencyBR();
        }
        #ifdef NDEBUG
        // If the right point of the trapezoid lies below the segment, move to the top-right adjacency
        else {
            idTrap = trapMap.getTrapezoid(idTrap).getIdTrapezoidTR();
        }
        #else
        // If the right point of the trapezoid lies below the segment, move to the top-right adjacency
        else if (cg3::isPointAtRight(segment, trapezoidPointR)) {
            idTrap = trapMap.getTrapezoid(idTrap).getIdAdjacencyTR();
        }
        // Point lies on a segment
        else {
            assert(false);
        }
        #endif

        // Get the right point of trapezoid reached at this iteration (if there is no right point, take the bounding box corner)
        idPointR = trapMap.getTrapezoid(idTrap).getIdPointR();
        trapezoidPointR = (idPointR == Trapezoid::NO_ID) ? trapMap.getBoundingBox().max() : trapMapData.getPoint(idPointR);

        // Store the trapezoid
        crossedTraps.push_back(idTrap);
    }
}

size_t queryToBuildTrapezoidalMap(const cg3::Segment2d &segment, const TrapezoidalMap trapMap, const DAG &dag)
{
    assert(segment.p1().x() < segment.p2().x());

    // Trapezoidal map dataset
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    const DAG::Node *dagNode = &dag.getRoot();
    // Scroll the DAG until a leaf is reached
    while(dagNode->getType() != DAG::Node::Type::Leaf) {
        // Check if the actual node is a x-node (endpoint) or a y-node (segment)
        switch(dagNode->getType()) {
            // Point-Endpoint comparison
            case DAG::Node::Type::XNode: {
                const cg3::Point2d &endpoint = trapMapData.getPoint(dagNode->getIdInfo());
                // Query point to the left of the segment endpoint
                if (segment.p1().x() < endpoint.x()) {
                    dagNode = &dag.getNode(dagNode->getIdNodeL());
                }
                // Query point either to the right or in the same vertical extension of the endpoint:
                // in both cases we treat it as being at the right (for real or conceptually)
                else {
                    dagNode = &dag.getNode(dagNode->getIdNodeR());
                }
                break;
            }
            // Point-Segment comparison
            case DAG::Node::Type::YNode: {
                const cg3::Segment2d &nodeSegment = trapMapData.getSegment(dagNode->getIdInfo());
                cg3::Segment2d nodeOrderedSegment;
                if (nodeSegment.p1().x() > nodeSegment.p2().x()) nodeOrderedSegment = cg3::Segment2d(nodeSegment.p2(), nodeSegment.p1());
                else nodeOrderedSegment = nodeSegment;

                // Query point above the segment
                if (cg3::isPointAtLeft(nodeOrderedSegment, segment.p1())) {
                    assert(nodeOrderedSegment.p1() != segment.p1());
                    dagNode = &dag.getNode(dagNode->getIdNodeL());
                }
                // Query point below the segment
                else if (cg3::isPointAtRight(nodeOrderedSegment, segment.p1())) {
                    assert(nodeOrderedSegment.p1() != segment.p1());
                    dagNode = &dag.getNode(dagNode->getIdNodeR());
                }
                // Query point lying on the segment: this happens when the query point is also the
                // left endpoint of the segment. Compare the slopes of the two segments to check
                // where the query continues
                else {
                    assert(nodeOrderedSegment.p1() == segment.p1());
                    // New segment slope is larger, continue above
                    if (cg3::isPointAtLeft(nodeOrderedSegment, segment.p2())) {
                        dagNode = &dag.getNode(dagNode->getIdNodeL());
                    }
                    #ifdef NDEBUG
                    // New segment slope is smaller, continue below
                    else {
                        dagNode = &dag.getNode(dagNode->getIdNodeR());
                    }
                    #else
                    // New segment slope is smaller, continue below
                    else if (cg3::isPointAtRight(nodeOrderedSegment, segment.p2())) {
                        dagNode = &dag.getNode(dagNode->getIdNodeR());
                    }
                    else {
                        // Overlapping segments
                        assert(false);
                    }
                    #endif
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
    assert(dagNode->getType() == DAG::Node::Type::Leaf);

    // Return the index of the trapezoid
    return dagNode->getIdInfo();
}

bool doesOverlapL(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    assert(segment.p1().x() < segment.p2().x());
    if (trapMap.getTrapezoid(idTrapezoid).getIdPointL() == Trapezoid::NO_ID) return false;
    else return segment.p1() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointL());
}

bool doesOverlapR(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    assert(segment.p1().x() < segment.p2().x());
    if (trapMap.getTrapezoid(idTrapezoid).getIdPointR() == Trapezoid::NO_ID)return false;
    else return segment.p2() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointR());
}

bool hasEndpointTL(size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    if (trapMap.getTrapezoid(idTrapezoid).getIdPointL() == Trapezoid::NO_ID ||
            trapMap.getTrapezoid(idTrapezoid).getIdSegmentT() == Trapezoid::NO_ID)
        return false;

    const cg3::Segment2d &segmentT = trapMapData.getSegment(trapMap.getTrapezoid(idTrapezoid).getIdSegmentT());
    if (segmentT.p1().x() < segmentT.p2().x())
        return segmentT.p1() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointL());
    else
        return segmentT.p2() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointL());
}

bool hasEndpointBL(size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    if (trapMap.getTrapezoid(idTrapezoid).getIdPointL() == Trapezoid::NO_ID ||
            trapMap.getTrapezoid(idTrapezoid).getIdSegmentB() == Trapezoid::NO_ID)
        return false;

    const cg3::Segment2d &segmentB = trapMapData.getSegment(trapMap.getTrapezoid(idTrapezoid).getIdSegmentB());
    if (segmentB.p1().x() < segmentB.p2().x())
        return segmentB.p1() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointL());
    else
        return segmentB.p2() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointL());
}

bool hasEndpointTR(size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    if (trapMap.getTrapezoid(idTrapezoid).getIdPointR() == Trapezoid::NO_ID ||
            trapMap.getTrapezoid(idTrapezoid).getIdSegmentT() == Trapezoid::NO_ID)
        return false;

    const cg3::Segment2d &segmentT = trapMapData.getSegment(trapMap.getTrapezoid(idTrapezoid).getIdSegmentT());
    if (segmentT.p1().x() < segmentT.p2().x())
        return segmentT.p2() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointR());
    else
        return segmentT.p1() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointR());
}

bool hasEndpointBR(size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    if (trapMap.getTrapezoid(idTrapezoid).getIdPointR() == Trapezoid::NO_ID ||
            trapMap.getTrapezoid(idTrapezoid).getIdSegmentB() == Trapezoid::NO_ID)
        return false;

    const cg3::Segment2d &segmentB = trapMapData.getSegment(trapMap.getTrapezoid(idTrapezoid).getIdSegmentB());
    if (segmentB.p1().x() < segmentB.p2().x())
        return segmentB.p2() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointR());
    else
        return segmentB.p1() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointR());
}

} // End namespace gasprj
