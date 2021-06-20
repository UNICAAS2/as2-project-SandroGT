#include "planar_point_location.h"

#include<algorithm>

#include <cg3/geometry/utils2.h>

namespace gasprj {

namespace gasprjint {

/* Internal functions declaration */

void updateOneCrossedTrapezoid(const cg3::Segment2d &segment, size_t idCrossedTrap, TrapezoidalMap &trapMap, DAG &dag);
void updateMoreCrossedTrapezoids(const cg3::Segment2d &segment, const std::vector<size_t> &crossedTraps,
                                 TrapezoidalMap &trapMap, DAG &dag);
void crossedTrapezoids(const cg3::Segment2d &segment, const TrapezoidalMap &trapMap, const DAG &dag,
                       std::vector<size_t> &crossedTraps);
size_t queryToBuildTrapezoidalMap(const cg3::Segment2d &segment, const TrapezoidalMap trapMap, const DAG &dag);
bool doesOverlapL(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool doesOverlapR(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointTL(size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointBL(size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointTR(size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointBR(size_t idTrapezoid, const TrapezoidalMap &trapMap);

} // End namespace gasprjint



/* Builders */

/**
 * @brief Initialize the data structures for the planar point location
 * @param[out] trapMap A brandly new trapezoidal map data structure
 * @param[out] dag A brandly new DAG search data structure
 *
 * Initialize the data structures with the default trapezoid: the bounding box. Add the trapezoid to the trapezoidal map
 * and the corresponding leaf to the DAG. This method has to be called right after the call of the constructors or after
 * the call of a clear() method.
 */
void initTrapezoidalMap(TrapezoidalMap &trapMap, DAG &dag)
{
    assert(trapMap.size() == 0 && dag.size() == 0);

    Trapezoid boundingBoxTrap = Trapezoid();
    boundingBoxTrap.setIdDagLeaf(0);
    DAG::Node boundingBoxNode = DAG::Node(DAG::Node::Type::Leaf, 0, DAG::Node::NO_ID, DAG::Node::NO_ID);

    trapMap.addTrapezoid(boundingBoxTrap);
    dag.addNode(boundingBoxNode);
}

/**
 * @brief Add a segment to the trapezoidal map and DAG data structures
 * @param[in] segment The new segment
 * @param[in,out] trapMap The trapezoidal map data structure
 * @param[in,out] dag The DAG query data structure
 *
 * Perform the incremental step of the trapezoidal map and DAG bulding algorithm, updating these data structures with
 * the new trapezoids defined by the new segment.
 */
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
    gasprjint::crossedTrapezoids(orderedSegment, trapMap, dag, crossedTraps);
    assert(crossedTraps.size() > 0);

    /*
     * Update the trapezoidal map and the DAG, updating all the crossed trapezoids and their corresponding DAG leaves.
     * Behave differently wheter one or more than one trapezoid has been crossed by the new segment
     */

    // New segment lying entirely in one trapezoid
    if (crossedTraps.size() == 1)
        gasprjint::updateOneCrossedTrapezoid(orderedSegment, crossedTraps[0], trapMap, dag);

    // New segment crossing two or more trapezoids
    else
        gasprjint::updateMoreCrossedTrapezoids(orderedSegment, crossedTraps, trapMap, dag);
}

/* Query */

/**
 * @brief Find the trapezoid containing the query point using the DAG: performs the query point location
 * @param[in] point The query point
 * @param[in] trapMap The trapezoidal map data structure
 * @param[in] dag The DAG query data structure
 * @return The ID of the trapezoid containing the query point
 */
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



namespace gasprjint {

/* Internal functions implementation */

/**
 * @brief Update the trapezoidal map and DAG data strucures when the new segment crosses one trapezoid
 * @param[in] segment The new segment
 * @param[in] idCrossedTrap The ID of the trapezoid crossed by the new segment
 * @param[in,out] trapMap The trapezoidal map data structure
 * @param[in,out] dag The DAG query data structure
 *
 * Perform the incremental step of the trapezoidal map and DAG bulding algorithm, updating the structure in the case the
 * segment lyes entirely in one and only trapezoid.
 */
void updateOneCrossedTrapezoid(const cg3::Segment2d &segment, size_t idCrossedTrap, TrapezoidalMap &trapMap, DAG &dag)
{
    // Trapezoidal map dataset (could be const, but the 'find' method is not declared const unfortunately)
    TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    // Define the variables for all the IDs, trapezoids and nodes that will be computed
    size_t idAvail;                                                    // Support variable for next available ID
    size_t idTrapT = Trapezoid::NO_ID, idTrapB = Trapezoid::NO_ID,     // New trapezoid IDs
           idTrapL = Trapezoid::NO_ID, idTrapR = Trapezoid::NO_ID;
    size_t idNodeXL = DAG::Node::NO_ID, idNodeXR = DAG::Node::NO_ID,   // New DAG leaf and node IDs
           idNodeY = DAG::Node::NO_ID, idLeafT = DAG::Node::NO_ID,
           idLeafB = DAG::Node::NO_ID, idLeafL = DAG::Node::NO_ID,
           idLeafR = DAG::Node::NO_ID;
    Trapezoid trap; DAG::Node node;                                    // New trapezoid and node variables

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

/**
 * @brief Update the trapezoidal map and DAG data strucures when the new segment crosses one trapezoid
 * @param[in] segment The new segment
 * @param[in] crossedTraps The vector of IDs of the trapezoids crossed by the segment
 * @param[in,out] trapMap The trapezoidal map data structure
 * @param[in,out] dag The DAG query data structure
 *
 * Perform the incremental step of the trapezoidal map and DAG bulding algorithm, updating the structure in the case the
 * segment crosses more than one trapezoid.
 */
void updateMoreCrossedTrapezoids(const cg3::Segment2d &segment,  const std::vector<size_t> &crossedTraps,
                                 TrapezoidalMap &trapMap, DAG &dag)
{
    // Trapezoidal map dataset (could be const, but the 'find' method is not declared const unfortunately)
    TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    // Define the variables for all the IDs, trapezoids and nodes that will be computed
    size_t idAvail;                                                    // Support variable for next available ID
    size_t idTrapT = Trapezoid::NO_ID, idTrapB = Trapezoid::NO_ID,     // New trapezoid IDs
           idTrapL = Trapezoid::NO_ID, idTrapR = Trapezoid::NO_ID;

    size_t idNodeXL = DAG::Node::NO_ID, idNodeXR = DAG::Node::NO_ID,   // New DAG leaf and node IDs
           idNodeY = DAG::Node::NO_ID, idLeafT = DAG::Node::NO_ID,
           idLeafB = DAG::Node::NO_ID, idLeafL = DAG::Node::NO_ID,
           idLeafR = DAG::Node::NO_ID;
    size_t idPrevTrapT = Trapezoid::NO_ID, idPrevTrapB = Trapezoid::NO_ID,   // Backup IDs of trapezoids and leaves
           idPrevLeafT = DAG::Node::NO_ID, idPrevLeafB = DAG::Node::NO_ID;
    Trapezoid trapT, trapB, prevTrap, *prevTrapT, *prevTrapB;                // Backup of trapezoids
    Trapezoid trap; DAG::Node node;                                          // New trapezoid and node variables

    // Actual crossed trapezoid
    size_t idCrossedTrap; Trapezoid crossedTrap;

    // Find the IDs of the segment and its endpoints
    bool found;
    size_t idSegment = trapMapData.findSegment(segment, found); assert(found);
    size_t idEndpointL = trapMapData.findPoint(segment.p1(), found); assert(found);
    size_t idEndpointR = trapMapData.findPoint(segment.p2(), found); assert(found);

    // Check the geomeytry conditions
    bool segmentCrossBelow;
    bool gNotOverlapL = !doesOverlapL(segment, crossedTraps[0], trapMap);
    bool gNotOverlapR = !doesOverlapR(segment, crossedTraps[crossedTraps.size()-1], trapMap);
    bool gEndpointTL = hasEndpointTL(crossedTraps[0], trapMap);
    bool gEndpointBL = hasEndpointBL(crossedTraps[0], trapMap);
    bool gEndpointTR = hasEndpointTR(crossedTraps[crossedTraps.size()-1], trapMap);
    bool gEndpointBR = hasEndpointBR(crossedTraps[crossedTraps.size()-1], trapMap);



    /*------ Leftmost trapezoid start ------*/

    // Get the crossed trapezoid
    idCrossedTrap = crossedTraps[0];
    crossedTrap = trapMap.getTrapezoid(idCrossedTrap);

    // Define the IDs of the new trapezoids
    idTrapT = idCrossedTrap;                               // Mandatory top trapezoid
    if (gNotOverlapL) idTrapL = trapMap.size();            // Optional left trapezoid
    idTrapB = gNotOverlapL ? idTrapL+1 : trapMap.size();   // Mandatory bottom trapezoid

    // Define the IDs of the new DAG nodes
    idAvail = crossedTrap.getIdDagLeaf();                  // First ID (trapezoid leaf ID)
    if (gNotOverlapL)                                      // Optional sub-graph for left trapezoid
        idNodeXL = idAvail, idLeafL = dag.size(), idAvail = idLeafL+1;
    idNodeY = idAvail,                                     // Mandatory sub-graph for top and bottom trapezoids
    idLeafT = gNotOverlapL ? idNodeY+1 : dag.size(), idLeafB = idLeafT+1;

    // Update the adjacencies of the neighboor trapezoids
    if (crossedTrap.getIdAdjacencyTL() != Trapezoid::NO_ID)   // Top-left adjacency
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyTL()).setIdAdjacencyTR(gNotOverlapL ? idTrapL : idTrapT);
    if (crossedTrap.getIdAdjacencyBL() != Trapezoid::NO_ID)   // Bottom-left adjacency
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyBL()).setIdAdjacencyBR(gNotOverlapL ? idTrapL : idTrapB);

    /* Update the trapezoidal map */

    // Mandatory top trapezoid
    trapT = Trapezoid(
        crossedTrap.getIdSegmentT(), idSegment, idEndpointL, Trapezoid::NO_ID,                      // Identifiers (right point unknown)
        gNotOverlapL ? idTrapL : gEndpointTL ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyTL(),   // Top-left adjacency
        Trapezoid::NO_ID,                                                                           // Top-right adjacency (unknown)
        Trapezoid::NO_ID, Trapezoid::NO_ID,                                                         // Bottom adjacencies
        idLeafT);                                                                                   // DAG leaf

    // Mandatory bottom trapezoid
    trapB = Trapezoid(
        idSegment, crossedTrap.getIdSegmentB(), idEndpointL, Trapezoid::NO_ID,                      // Identifiers (right point unknown)
        Trapezoid::NO_ID, Trapezoid::NO_ID,                                                         // Top adjacencies
        gNotOverlapL ? idTrapL : gEndpointBL ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyBL(),   // Bottom-left adjacency
        Trapezoid::NO_ID,                                                                           // Bottom-right adjacency (unknown)
        idLeafB);                                                                                   // DAG leaf

    // Optional left trapezoid
    if (gNotOverlapL) {
        trap = Trapezoid(
            crossedTrap.getIdSegmentT(), crossedTrap.getIdSegmentB(), crossedTrap.getIdPointL(), idEndpointL,   // Identifiers
            crossedTrap.getIdAdjacencyTL(), idTrapT, crossedTrap.getIdAdjacencyBL(), idTrapB, idLeafL);         // Adjacencies and DAG leaf
        trapMap.addTrapezoid(trap);
    }

    /* Update the DAG */

    // Optional sub-graph for left trapezoid
    if (gNotOverlapL) {
        node = DAG::Node(DAG::Node::Type::XNode, idEndpointL, idLeafL, idNodeY);
        dag.overwriteNode(node, idNodeXL);
        node = DAG::Node(DAG::Node::Type::Leaf, idTrapL, DAG::Node::NO_ID, DAG::Node::NO_ID);
        dag.addNode(node);
    }

    // Mandatory sub-graph for top and bottom trapezoids
    node = DAG::Node(DAG::Node::Type::YNode, idSegment, idLeafT, idLeafB);
    gNotOverlapL ? dag.addNode(node) : dag.overwriteNode(node, idNodeY);
    node = DAG::Node(DAG::Node::Type::Leaf, idTrapT, DAG::Node::NO_ID, DAG::Node::NO_ID);
    dag.addNode(node);
    node = DAG::Node(DAG::Node::Type::Leaf, idTrapB, DAG::Node::NO_ID, DAG::Node::NO_ID);
    dag.addNode(node);



    /*------ Center trapezoids crossing ------*/

    // Backup data for the crossing iteration
    prevTrap = crossedTrap;                                                                 // Previous crossed trapezoid
    idPrevTrapT = idTrapT, prevTrapT = &trapT; idPrevTrapB = idTrapB, prevTrapB = &trapB;   // Previous new top and bottom trapezoids
    idPrevLeafT = idLeafT, idPrevLeafB = idLeafB;                                           // Previous new top and bottom DAG leaves

    // Check the position of the segment with respect of the right point of the previous trapezoid
    assert(prevTrap.getIdPointR() != Trapezoid::NO_ID);
    segmentCrossBelow = cg3::isPointAtLeft(segment, trapMapData.getPoint(prevTrap.getIdPointR()));

    // Cross all the center trapezoids
    for (size_t i = 1; i < crossedTraps.size()-1; ++i) {

        // Get the crossed trapezoid
        idCrossedTrap = crossedTraps[i];
        crossedTrap = trapMap.getTrapezoid(idCrossedTrap);

        // The actual trapezoid is a bottom-right adjacency of the previous trapezoid
        if (segmentCrossBelow) {

            // Define the ID of the new top trapezoid
            idTrapT = idCrossedTrap;

            // Define the IDs of the new DAG nodes
            idNodeY = crossedTrap.getIdDagLeaf(), idLeafT = dag.size(), idLeafB = idPrevLeafB;

            // Update the top-right adjacency of the neighboor trapezoid
            if (prevTrap.getIdAdjacencyTR() != Trapezoid::NO_ID)
                trapMap.getTrapezoid(prevTrap.getIdAdjacencyTR()).setIdAdjacencyTL(idPrevTrapT);

            /* Update the trapezoidal map */

            // The previous top trapezoid has come to an end, complete its right informations and add it to the trapezoidal map
            prevTrapT->setIdPointR(crossedTrap.getIdPointL());          // Identifier: right point
            prevTrapT->setIdAdjacencyTR(prevTrap.getIdAdjacencyTR());   // Top-right adjacency
            prevTrapT->setIdAdjacencyBR(idTrapT);                       // Bottom-right adjacency
            if (idPrevTrapT < trapMap.size()) trapMap.overwriteTrapezoid(*prevTrapT, idPrevTrapT);
            else trapMap.addTrapezoid(*prevTrapT);

            // Create the new top trapezoid as a copy of the actual crossed trapezoid, then adapt it
            trapT = crossedTrap;
            trapT.setIdSegmentB(idSegment), trapT.setIdAdjacencyBL(idPrevTrapT), trapT.setIdDagLeaf(idLeafT);

            // Update the previous top trapezoid references
            idPrevTrapT = idTrapT, prevTrapT = &trapT;

            /* Update the DAG */

            // Mandatory sub-graph for top and bottom trapezoids
            node = DAG::Node(DAG::Node::Type::YNode, idSegment, idLeafT, idLeafB);
            dag.overwriteNode(node, idNodeY);
            node = DAG::Node(DAG::Node::Type::Leaf, idTrapT, DAG::Node::NO_ID, DAG::Node::NO_ID);
            dag.addNode(node); idPrevLeafT = idLeafT;
        }

        // The actual trapezoid is a top-right adjacency of the previous trapezoid
        else {

            // Define the ID of the new bottom trapezoid
            idTrapB = idCrossedTrap;

            // Define the IDs of the new DAG nodes
            idNodeY = crossedTrap.getIdDagLeaf(), idLeafT = idPrevLeafT, idLeafB = dag.size();

            // Update the bottom-right adjacency of the neighboor trapezoid
            if (prevTrap.getIdAdjacencyBR() != Trapezoid::NO_ID)
                trapMap.getTrapezoid(prevTrap.getIdAdjacencyBR()).setIdAdjacencyBL(idPrevTrapB);

            /* Update the trapezoidal map */

            // The previous bottom trapezoid has come to an end, complete its right informations and add it to the trapezoidal map
            prevTrapB->setIdPointR(crossedTrap.getIdPointL());          // Identifier: right point
            prevTrapB->setIdAdjacencyTR(idTrapB);                       // Top-right adjacency
            prevTrapB->setIdAdjacencyBR(prevTrap.getIdAdjacencyBR());   // Bottom-right adjacency
            if (idPrevTrapB < trapMap.size()) trapMap.overwriteTrapezoid(*prevTrapB, idPrevTrapB);
            else trapMap.addTrapezoid(*prevTrapB);

            // Create the new bottom trapezoid as a copy of the actual crossed trapezoid, then adapt it
            trapB = crossedTrap;
            trapB.setIdSegmentT(idSegment), trapB.setIdAdjacencyTL(idPrevTrapB), trapB.setIdDagLeaf(idLeafB);

            // Update the previous bottom trapezoid references
            idPrevTrapB = idTrapB, prevTrapB = &trapB;

            /* Update the DAG */

            // Mandatory sub-graph for top and bottom trapezoids
            node = DAG::Node(DAG::Node::Type::YNode, idSegment, idLeafT, idLeafB);
            dag.overwriteNode(node, idNodeY);
            node = DAG::Node(DAG::Node::Type::Leaf, idTrapB, DAG::Node::NO_ID, DAG::Node::NO_ID);
            dag.addNode(node); idPrevLeafB = idLeafB;
        }

        // Update the backup of the previous trapezoid
        prevTrap = crossedTrap;
        // Check the position of the segment with respect to the right point of the previous trapezoid
        assert(prevTrap.getIdPointR() != Trapezoid::NO_ID);
        segmentCrossBelow = cg3::isPointAtLeft(segment, trapMapData.getPoint(prevTrap.getIdPointR()));
    }



    /*------ Rightmost trapezoid end ------*/

    // Get the crossed trapezoid
    idCrossedTrap = crossedTraps[crossedTraps.size()-1];
    crossedTrap = trapMap.getTrapezoid(idCrossedTrap);

    // Define the IDs of the new trapezoids
    if (gNotOverlapR) idTrapR = std::max(idPrevTrapT+1, std::max(idPrevTrapB+1, trapMap.size()));
    if (segmentCrossBelow) idTrapT = idCrossedTrap; else idTrapB = idCrossedTrap;

    // Define the IDs of the new DAG nodes
    idAvail = crossedTrap.getIdDagLeaf();                                 // First ID (trapezoid leaf ID)
    if (gNotOverlapR)                                                     // Optional sub-graph for right trapezoid
        idNodeXR = idAvail, idLeafR = dag.size(), idAvail = idLeafR+1;
    idNodeY = idAvail, idAvail = gNotOverlapR ? idNodeY+1 : dag.size();   // Mandatory sub-graph for top and bottom trapezoids
    if (segmentCrossBelow) idLeafT = idAvail, idLeafB = idPrevLeafB;
    else idLeafT = idPrevLeafT, idLeafB = idAvail;

    // Update the adjacencies of the neighboor trapezoids
    if (prevTrap.getIdAdjacencyTR() != Trapezoid::NO_ID)
        trapMap.getTrapezoid(prevTrap.getIdAdjacencyTR()).setIdAdjacencyTL(idPrevTrapT);
    if (prevTrap.getIdAdjacencyBR() != Trapezoid::NO_ID)
        trapMap.getTrapezoid(prevTrap.getIdAdjacencyBR()).setIdAdjacencyBL(idPrevTrapB);
    if (crossedTrap.getIdAdjacencyTR() != Trapezoid::NO_ID)
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyTR()).setIdAdjacencyTL(gNotOverlapR ? idTrapR : idTrapT);
    if (crossedTrap.getIdAdjacencyBR() != Trapezoid::NO_ID)
        trapMap.getTrapezoid(crossedTrap.getIdAdjacencyBR()).setIdAdjacencyBL(gNotOverlapR ? idTrapR : idTrapB);

    /* Update the trapezoidal map */

    // The actual trapezoid is a bottom-right adjacency of the previous trapezoid
    if (segmentCrossBelow) {

        // The previous top trapezoid has come to an end, complete its right informations and add it to the trapezoidal map
        prevTrapT->setIdPointR(crossedTrap.getIdPointL());          // Identifier: right point
        prevTrapT->setIdAdjacencyTR(prevTrap.getIdAdjacencyTR());   // Top-right adjacency
        prevTrapT->setIdAdjacencyBR(idTrapT);                       // Bottom-right adjacency
        if (idPrevTrapT < trapMap.size()) trapMap.overwriteTrapezoid(*prevTrapT, idPrevTrapT);
        else trapMap.addTrapezoid(*prevTrapT);

        // The previous bottom trapezoid has come to an end too, complete its right informations and add it to the trapezoidal map
        prevTrapB->setIdPointR(idEndpointR);             // Identifier: right point
        prevTrapB->setIdAdjacencyTR(Trapezoid::NO_ID);   // Top-right adjacency
        prevTrapB->setIdAdjacencyBR(                     // Bottom-right adjacency
                    gNotOverlapR ? idTrapR : gEndpointBR ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyBR());
        if (idPrevTrapB < trapMap.size()) trapMap.overwriteTrapezoid(*prevTrapB, idPrevTrapB);
        else trapMap.addTrapezoid(*prevTrapB);

        // Create the new top trapezoid
        trapT = Trapezoid(
            crossedTrap.getIdSegmentT(), idSegment, crossedTrap.getIdPointL(), idEndpointR,             // Identifiers
            crossedTrap.getIdAdjacencyTL(),                                                             // Top-left adjacency
            gNotOverlapR ? idTrapR : gEndpointTR ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyTR(),   // Top-right adjacency
            idPrevTrapT, Trapezoid::NO_ID,                                                              // Bottom adjacencies
            idLeafT);
        trapMap.overwriteTrapezoid(trapT, idTrapT);
        idPrevTrapT = idTrapT, prevTrapT = &trapT;
    }

    // The actual trapezoid is a top-right adjacency of the previous trapezoid
    else {

        // The previous bottom trapezoid has come to an end, complete its right informations and add it to the trapezoidal map
        prevTrapB->setIdPointR(crossedTrap.getIdPointL());          // Identifier: right point
        prevTrapB->setIdAdjacencyTR(idTrapB);                       // Top-right adjacency
        prevTrapB->setIdAdjacencyBR(prevTrap.getIdAdjacencyBR());   // Bottom-right adjacency
        if (idPrevTrapB < trapMap.size()) trapMap.overwriteTrapezoid(*prevTrapB, idPrevTrapB);
        else trapMap.addTrapezoid(*prevTrapB);

        // The previous top trapezoid has come to an end too, complete its right informations and add it to the trapezoidal map
        prevTrapT->setIdPointR(idEndpointR);             // Identifier: right point
        prevTrapT->setIdAdjacencyBR(Trapezoid::NO_ID);   // Bottom-right adjacency
        prevTrapT->setIdAdjacencyTR(                     // Top-right adjacency
                    gNotOverlapR ? idTrapR : gEndpointTR ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyTR());
        if (idPrevTrapT < trapMap.size()) trapMap.overwriteTrapezoid(*prevTrapT, idPrevTrapT);
        else trapMap.addTrapezoid(*prevTrapT);

        // Create the new bottom trapezoid
        trapB = Trapezoid(
            idSegment, crossedTrap.getIdSegmentB(), crossedTrap.getIdPointL(), idEndpointR,             // Identifiers
            idPrevTrapB, Trapezoid::NO_ID,                                                              // Top adjacencies
            crossedTrap.getIdAdjacencyBL(),                                                             // Bottom-left adjacency
            gNotOverlapR ? idTrapR : gEndpointBR ? Trapezoid::NO_ID : crossedTrap.getIdAdjacencyBR(),   // Bottom-right adjacency
            idLeafB);                                                                                   // DAG leaf
        trapMap.overwriteTrapezoid(trapB, idTrapB);
        idPrevTrapB = idTrapB, prevTrapB = &trapB;
    }

    // Optional right trapezoid (create it as a copy of the actual crossed trapezoid, then adapt it)
    if (gNotOverlapR) {
        trap = crossedTrap;
        trap.setIdPointL(idEndpointR), trap.setIdAdjacencyTL(idPrevTrapT), trap.setIdAdjacencyBL(idPrevTrapB), trap.setIdDagLeaf(idLeafR);
        trapMap.addTrapezoid(trap);
    }

    /* Update the DAG */

    // Optional sub-graph for right trapezoid
    if (gNotOverlapR) {
        node = DAG::Node(DAG::Node::Type::XNode, idEndpointR, idNodeY, idLeafR);
        dag.overwriteNode(node, idNodeXR);
        node = DAG::Node(DAG::Node::Type::Leaf, idTrapR, DAG::Node::NO_ID, DAG::Node::NO_ID);
        dag.addNode(node);
    }

    // Mandatory sub-graph for top and bottom trapezoids
    node = DAG::Node(DAG::Node::Type::YNode, idSegment, idLeafT, idLeafB);
    gNotOverlapR ? dag.addNode(node) : dag.overwriteNode(node, idNodeY);
    node = DAG::Node(DAG::Node::Type::Leaf, segmentCrossBelow ? idPrevTrapT : idPrevTrapB, DAG::Node::NO_ID, DAG::Node::NO_ID);
    dag.addNode(node);
}

/**
 * @brief Find the trapezoids crossed by the segment
 * @param[in] segment The query segment
 * @param[in] trapMap The trapezoidal map data structure
 * @param[in] dag The DAG query data structure
 * @param[out] crossedTraps An empty vector to contain the list of IDs of the trapezoids crossed by the segment
 */
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
            idTrap = trapMap.getTrapezoid(idTrap).getIdAdjacencyTR();
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

/**
 * @brief Find the trapezoid containing the left endpoint of the segment
 * @param[in] segment The query segment
 * @param[in] dag The DAG query data structure
 * @return The ID of the trapezoid containing the left left endpoint of the segment
 *
 * This version of the query function is called by the building functions to find the correct leftmost trapezoid
 * traversed by the new segment. Comparisons with the new segment (and not just its left endpoint) could be made.
 */
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

/**
 * @brief Check if the left point of the trapezoid overlaps with the left endpoint of the segment
 * @param[in] segment The new segment
 * @param[in] idTrapezoid The ID of the first trapezoid
 * @param[in] trapMap The trapezoidal map data structure
 * @return True, if the left point of the trapezoid overlaps with the left endpoint of the segment, false otherwise
 */
bool doesOverlapL(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    assert(segment.p1().x() < segment.p2().x());
    if (trapMap.getTrapezoid(idTrapezoid).getIdPointL() == Trapezoid::NO_ID) return false;
    else return segment.p1() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointL());
}

/**
 * @brief Check if the right point of the trapezoid overlaps with the right endpoint of the segment
 * @param[in] segment The new segment
 * @param[in] idTrapezoid The ID of the last trapezoid
 * @param[in] trapMap The trapezoidal map data structure
 * @return True, if the right point of the trapezoid overlaps with the right endpoint of the segment, false otherwise
 */
bool doesOverlapR(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap)
{
    const TrapezoidalMapDataset &trapMapData = *trapMap.getRefTrapezoidalMapDataset();

    assert(segment.p1().x() < segment.p2().x());
    if (trapMap.getTrapezoid(idTrapezoid).getIdPointR() == Trapezoid::NO_ID)return false;
    else return segment.p2() == trapMapData.getPoint(trapMap.getTrapezoid(idTrapezoid).getIdPointR());
}

/**
 * @brief Check if the left point of the first trapezoid coincides with the left endpoint of its top segment
 * @param[in] idTrapezoid The ID of the first trapezoid
 * @param[in] trapMap The trapezoidal map data structure
 * @return True, if the left point of the first trapezoid coincides with the left endpoint of its top segment, false otherwise
 */
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

/**
 * @brief Check if the left point of the first trapezoid coincides with the left endpoint of its bottom segment
 * @param[in] idTrapezoid The ID of the first trapezoid
 * @param[in] trapMap The trapezoidal map data structure
 * @return True, if the left point of the first trapezoid coincides with the left endpoint of its bottom segment, false otherwise
 */
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

/**
 * @brief Check if the right point of the last trapezoid coincides with the right endpoint of its top segment
 * @param[in] idTrapezoid The ID of the last trapezoid
 * @param[in] trapMap The trapezoidal map data structure
 * @return True, if the right point of the last trapezoid coincides with the right endpoint of its top segment, false otherwise
 */
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

/**
 * @brief Check if the right point of the last trapezoid coincides with the right endpoint of its bottom segment
 * @param[in] idTrapezoid The ID of the last trapezoid
 * @param[in] trapMap The trapezoidal map data structure
 * @return True, if the right point of the last trapezoid coincides with the right endpoint of its bottom segment, false otherwise
 */
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

} // End namespace gasprjint

} // End namespace gasprj
