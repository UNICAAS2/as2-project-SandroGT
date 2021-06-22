#include "drawable_trapezoidalmap.h"

namespace gasprj {

/* Internal methods declaration */

/**
 * @brief Compute the vertices of a drawable trapezoid
 * @param[in,out] dTrap The drawable trapezoid
 */
void DrawableTrapezoidalMap::setDrawableTrapezoidVertices(DrawableTrapezoid &dTrap)
{
    // The actual segments and points defining the trapezoid
    cg3::Segment2d segmentT, segmentB;
    cg3::Point2d pointL, pointR;

    if(dTrap.getIdPointL() == Trapezoid::NO_ID || dTrap.getIdPointR() == Trapezoid::NO_ID)
        assert(dTrap.getIdSegmentT() == Trapezoid::NO_ID && dTrap.getIdSegmentB() == Trapezoid::NO_ID);

    /* Retrieve the points and segments from the dataset */

    // Retrieve the left point
    if (dTrap.getIdPointL() != Trapezoid::NO_ID)
        pointL = this->getRefTrapezoidalMapDataset()->getPoint(dTrap.getIdPointL());
    else
        pointL = cg3::Point2d(this->getBoundingBox().min().x(), this->getBoundingBox().max().y());

    // Retrieve the right point
    if (dTrap.getIdPointR() != Trapezoid::NO_ID)
        pointR = this->getRefTrapezoidalMapDataset()->getPoint(dTrap.getIdPointR());
    else
        pointR = cg3::Point2d(this->getBoundingBox().max().x(), this->getBoundingBox().max().y());

    // Retrieve the top segment
    if (dTrap.getIdSegmentT() != Trapezoid::NO_ID) {
        segmentT = this->getRefTrapezoidalMapDataset()->getSegment(dTrap.getIdSegmentT());
        if (segmentT.p1().x() > segmentT.p2().x()) segmentT = cg3::Segment2d(segmentT.p2(), segmentT.p1());
    }
    else
        segmentT = cg3::Segment2d(cg3::Point2d(this->getBoundingBox().min().x(), this->getBoundingBox().max().y()),
                                  cg3::Point2d(this->getBoundingBox().max().x(), this->getBoundingBox().max().y()));

    // Retrieve the bottom segment
    if (dTrap.getIdSegmentB() != Trapezoid::NO_ID) {
        segmentB = this->getRefTrapezoidalMapDataset()->getSegment(dTrap.getIdSegmentB());
        if (segmentB.p1().x() > segmentB.p2().x()) segmentB = cg3::Segment2d(segmentB.p2(), segmentB.p1());
    }
    else
        segmentB = cg3::Segment2d(cg3::Point2d(this->getBoundingBox().min().x(), this->getBoundingBox().min().y()),
                                  cg3::Point2d(this->getBoundingBox().max().x(), this->getBoundingBox().min().y()));

    /* Compute the intersections between the segments and the vertical lines passing through the points */

    // Segment slopes
    double mT = (segmentT.p2().y()-segmentT.p1().y()) / (segmentT.p2().x()-segmentT.p1().x());
    double mB = (segmentB.p2().y()-segmentB.p1().y()) / (segmentB.p2().x()-segmentB.p1().x());

    dTrap.setVertexTL(cg3::Point2d(pointL.x(), segmentT.p1().y() + mT * (pointL.x() - segmentT.p1().x()))); // Top-left
    dTrap.setVertexTR(cg3::Point2d(pointR.x(), segmentT.p2().y() - mT * (segmentT.p2().x() - pointR.x()))); // Top-right
    dTrap.setVertexBR(cg3::Point2d(pointR.x(), segmentB.p2().y() - mB * (segmentB.p2().x() - pointR.x()))); // Bottom-right
    dTrap.setVertexBL(cg3::Point2d(pointL.x(), segmentB.p1().y() + mB * (pointL.x() - segmentB.p1().x()))); // Bottom-left

    /* Compute the bounding box for the drawable object */
    dTrap.setBoundingBox(
        cg3::Point2d(std::min(dTrap.getVertexTL().x(), dTrap.getVertexBL().x()),
                     std::min(dTrap.getVertexBL().y(), dTrap.getVertexBR().y())),   // Bottom-left corner
        cg3::Point2d(std::max(dTrap.getVertexTR().x(), dTrap.getVertexBR().x()),
                     std::max(dTrap.getVertexTL().y(), dTrap.getVertexTR().y()))    // Top-right corner
    );
}

} // End namespace gasprj
