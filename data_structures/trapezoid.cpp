#include "trapezoid.h"

namespace gasprj {

Trapezoid::Trapezoid() :
    idSegmentT(NO_ID), idSegmentB(NO_ID), idPointL(NO_ID), idPointR(NO_ID),
    idTrapezoidTL(NO_ID), idTrapezoidTR(NO_ID), idTrapezoidBR(NO_ID), idTrapezoidBL(NO_ID)
{

}

size_t Trapezoid::getIdSegmentT() {
    return idSegmentT;
}

void Trapezoid::setIdSegmentT(size_t id) {
    idSegmentT = id;
}

size_t Trapezoid::getIdSegmentB() {
    return idSegmentB;
}

void Trapezoid::setIdSegmentB(size_t id) {
    idSegmentB = id;
}

size_t Trapezoid::getIdPointL() {
    return idPointL;
}

void Trapezoid::setIdPointL(size_t id) {
    idPointL = id;
}

size_t Trapezoid::getIdPointR() {
    return idPointR;
}

void Trapezoid::setIdPointR(size_t id) {
    idPointR = id;
}

size_t Trapezoid::getIdTrapezoidTL() {
    return idTrapezoidTL;
}

void Trapezoid::setIdTrapezoidTL(size_t id) {
    idTrapezoidTL = id;
}

size_t Trapezoid::getIdTrapezoidTR() {
    return idTrapezoidTR;
}

void Trapezoid::setIdTrapezoidTR(size_t id) {
    idTrapezoidTR = id;
}

size_t Trapezoid::getIdTrapezoidBR() {
    return idTrapezoidBR;
}

void Trapezoid::setIdTrapezoidBR(size_t id) {
    idTrapezoidBR = id;
}

size_t Trapezoid::getIdTrapezoidBL() {
    return idTrapezoidBL;
}

void Trapezoid::setIdTrapezoidBL(size_t id) {
    idTrapezoidBL = id;
}

void Trapezoid::updateVertices(const cg3::BoundingBox2 boundingBox) {

    assert(idPointL == NO_ID && idPointR == NO_ID && idSegmentT == NO_ID && idSegmentB == NO_ID);

    vertexTL = cg3::Point2d(boundingBox.min().x(), boundingBox.max().y());
    vertexTR = cg3::Point2d(boundingBox.max().x(), boundingBox.max().y());
    vertexBR = cg3::Point2d(boundingBox.max().x(), boundingBox.min().y());
    vertexBL = cg3::Point2d(boundingBox.min().x(), boundingBox.min().y());
}

void Trapezoid::updateVertices(const TrapezoidalMapDataset& trapMapData, const cg3::BoundingBox2 boundingBox) {
    cg3::Segment2d segmentT, segmentB;
    cg3::Point2d pointL, pointR;

    if(idPointL == NO_ID || idPointR == NO_ID) assert(idSegmentT == NO_ID && idSegmentB == NO_ID);

    if (idPointL != NO_ID) pointL = trapMapData.getPoint(idPointL);
    else pointL = cg3::Point2d(boundingBox.min().x(), boundingBox.max().y());

    if (idPointR != NO_ID) pointR = trapMapData.getPoint(idPointR);
    else pointR = cg3::Point2d(boundingBox.max().x(), boundingBox.max().y());

    if (idSegmentT != NO_ID) segmentT = trapMapData.getSegment(idSegmentT);
    else segmentT = cg3::Segment2d(cg3::Point2d(boundingBox.min().x(), boundingBox.max().y()),
                                   cg3::Point2d(boundingBox.max().x(), boundingBox.max().y()));
    if (segmentT.p1().x() > segmentT.p2().x()) segmentT = cg3::Segment2d(segmentT.p2(), segmentT.p1());

    if (idSegmentB != NO_ID) segmentB = trapMapData.getSegment(idSegmentB);
    else segmentB = cg3::Segment2d(cg3::Point2d(boundingBox.min().x(), boundingBox.min().y()),
                                   cg3::Point2d(boundingBox.max().x(), boundingBox.min().y()));
    if (segmentB.p1().x() > segmentB.p2().x()) segmentB = cg3::Segment2d(segmentB.p2(), segmentB.p1());

    double mT = (segmentT.p2().y()-segmentT.p1().y()) / (segmentT.p2().x()-segmentT.p1().x());
    double mB = (segmentB.p2().y()-segmentB.p1().y()) / (segmentB.p2().x()-segmentB.p1().x());

    vertexTL = cg3::Point2d(pointL.x(), segmentT.p1().y() + mT * (pointL.x() - segmentT.p1().x()));
    vertexTR = cg3::Point2d(pointR.x(), segmentT.p2().y() - mT * (segmentT.p2().x() - pointR.x()));
    vertexBR = cg3::Point2d(pointR.x(), segmentB.p2().y() - mB * (segmentB.p2().x() - pointR.x()));
    vertexBL = cg3::Point2d(pointL.x(), segmentB.p1().y() + mB * (pointL.x() - segmentB.p1().x()));
}

const cg3::Point2d& Trapezoid::getVertexTL() const {
    return vertexTL;
}

const cg3::Point2d& Trapezoid::getVertexTR() const {
    return vertexTR;
}

const cg3::Point2d& Trapezoid::getVertexBR() const {
    return vertexBR;
}

const cg3::Point2d& Trapezoid::getVertexBL() const {
    return vertexBL;
}

}
