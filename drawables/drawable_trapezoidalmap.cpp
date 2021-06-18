#include "drawable_trapezoidalmap.h"

#include <GL/gl.h>

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

namespace gasprj {

/**
 * @brief The constructor of the drawable trapezoidal map
 * @param[in] refTrapezoidalMapDataset The reference to the trapezoidal map dataset
 * @param[in] boundingBoxCornerBL The bottom-left corner of the bounding box
 * @param[in] boundingBoxCornerTR The top-right corner of the bounding box
 */
DrawableTrapezoidalMap::DrawableTrapezoidalMap(TrapezoidalMapDataset *const trapezoidalMapDataset,
                                               const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR) :
    TrapezoidalMap(trapezoidalMapDataset, boundingBoxCornerBL, boundingBoxCornerTR),
    idHighlightedTrapezoid(Trapezoid::NO_ID)
{
}

/**
 * @brief Draw the trapezoidal map
 */
void DrawableTrapezoidalMap::draw() const
{
    // Define the width of the vertical lines
    glLineWidth(WIDTH_VERTICAL_LINE);

    // Cycle over all the drawable trapezoids
    size_t i = 0;
    for (const DrawableTrapezoid &dTrap : trapezoids) {

        // Define the color of the vertical segments
        glColor4d(COLOR_VERTICAL_LINE.redF(), COLOR_VERTICAL_LINE.greenF(),
                  COLOR_VERTICAL_LINE.blueF(), COLOR_VERTICAL_LINE.alphaF());

        // Draw the vertical segments
        glBegin(GL_LINES);
        glVertex2d(dTrap.getVertexTL().x(), dTrap.getVertexTL().y());
        glVertex2d(dTrap.getVertexBL().x(), dTrap.getVertexBL().y());
        glVertex2d(dTrap.getVertexTR().x(), dTrap.getVertexTR().y());
        glVertex2d(dTrap.getVertexBR().x(), dTrap.getVertexBR().y());
        glEnd();

        // Define the trapezoid color
        if (i == idHighlightedTrapezoid)
            glColor4d(COLOR_TRAPEZOID_SELECTED.redF(), COLOR_TRAPEZOID_SELECTED.greenF(),
                      COLOR_TRAPEZOID_SELECTED.blueF(), COLOR_TRAPEZOID_SELECTED.alphaF());
        else
            glColor4d(dTrap.getColorR(), dTrap.getColorG(), dTrap.getColorB(), TRAPEZOID_TRANSPARENCY);

        // Draw the trapezoid
        glBegin(GL_POLYGON);
        glVertex2d(dTrap.getVertexTL().x(), dTrap.getVertexTL().y());
        glVertex2d(dTrap.getVertexTR().x(), dTrap.getVertexTR().y());
        glVertex2d(dTrap.getVertexBR().x(), dTrap.getVertexBR().y());
        glVertex2d(dTrap.getVertexBL().x(), dTrap.getVertexBL().y());
        glEnd();

        ++i;
    }
}

/**
 * @brief Get the center of the scene
 * @return The center of the scene as a 3D point
 */
cg3::Point3d DrawableTrapezoidalMap::sceneCenter() const
{
    return cg3::Point3d(this->boundingBox.center().x(), this->boundingBox.center().y(), 0);
}

/**
 * @brief Get the radius of the scene
 * @return The radius of the scene
 */
double DrawableTrapezoidalMap::sceneRadius() const
{
    return this->boundingBox.diag();
}

/**
 * @brief Get the ID of the highlighted trapezoid
 * @return The ID of the highlighted trapezoid
 */
size_t DrawableTrapezoidalMap::getIdHighlightedTrapezoid() const
{
    return idHighlightedTrapezoid;
}

/**
 * @brief Set the ID of the highlighted trapezoid
 * @param[in] id The new ID of the highlighted trapezoid
 */
void DrawableTrapezoidalMap::setIdHighlightedTrapezoid(size_t id)
{
    idHighlightedTrapezoid = id;
}

/**
 * @brief Get a reference to the specified drawable trapezoid from the drawable trapezoidal map
 * @param[in] id The ID of the drawable trapezoid
 * @return A reference to the specified drawable trapezoid
 */
Trapezoid &DrawableTrapezoidalMap::getTrapezoid(size_t id)
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

/**
 * @brief Get a non-modifiable reference to the specified drawable trapezoid from the drawable trapezoidal map
 * @param[in] id The ID of the drawable trapezoid
 * @return A costant reference to the specified drawable trapezoid
 */
const Trapezoid &DrawableTrapezoidalMap::getTrapezoid(size_t id) const
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

/**
 * @brief Get the number of drawable trapezoids in the drawable trapezoidal map
 * @return The number of drawable trapezoids in the drawable trapezoidal map
 */
size_t DrawableTrapezoidalMap::size() const
{
    return trapezoids.size();
}

/**
 * @brief Add a new drawable trapezoid to the drawable trapezoidal map
 * @param[in] trapezoid The new trapezoid, converted to its drawable version
 */
void DrawableTrapezoidalMap::addTrapezoid(const Trapezoid &trapezoid)
{
    DrawableTrapezoid drawableTrapezoid = DrawableTrapezoid(trapezoid);
    setDrawableTrapezoidVertices(*this, drawableTrapezoid);
    setDrawableTrapezoidColor(drawableTrapezoid);

    trapezoids.push_back(drawableTrapezoid);
}

/**
 * @brief Overwrite the specified drawable trapezoid in the drawable trapezoidal map
 * @param[in] trapezoid The new trapezoid, converted to its drawable version
 * @param[in] id The ID of the drawable trapezoid to overwrite
 */
void DrawableTrapezoidalMap::overwriteTrapezoid(const Trapezoid &trapezoid, size_t id)
{
    assert(id < trapezoids.size());
    DrawableTrapezoid drawableTrapezoid = DrawableTrapezoid(trapezoid);
    setDrawableTrapezoidVertices(*this, drawableTrapezoid);
    setDrawableTrapezoidColor(drawableTrapezoid);

    trapezoids[id] = drawableTrapezoid;
}

/**
 * @brief Delete all the trapezoids stored in the drawable trapezoidal map
 */
void DrawableTrapezoidalMap::clear() {
    trapezoids.clear();
    idHighlightedTrapezoid = Trapezoid::NO_ID;
}



/* Helper functions */

/**
 * @brief Compute the vertices of a drawable trapezoid
 * @param[in] dTrapMap The drawable trapezoidal map
 * @param[in,out] dTrap The drawable trapezoid
 */
void setDrawableTrapezoidVertices(const DrawableTrapezoidalMap &dTrapMap, DrawableTrapezoid &dTrap)
{
    // The actual segments and points defining the trapezoid
    cg3::Segment2d segmentT, segmentB;
    cg3::Point2d pointL, pointR;

    if(dTrap.getIdPointL() == Trapezoid::NO_ID || dTrap.getIdPointR() == Trapezoid::NO_ID)
        assert(dTrap.getIdSegmentT() == Trapezoid::NO_ID && dTrap.getIdSegmentB() == Trapezoid::NO_ID);

    /* Retrieve the points and segments from the dataset */

    // Retrieve the left point
    if (dTrap.getIdPointL() != Trapezoid::NO_ID)
        pointL = dTrapMap.getRefTrapezoidalMapDataset()->getPoint(dTrap.getIdPointL());
    else
        pointL = cg3::Point2d(dTrapMap.getBoundingBox().min().x(), dTrapMap.getBoundingBox().max().y());

    // Retrieve the right point
    if (dTrap.getIdPointR() != Trapezoid::NO_ID)
        pointR = dTrapMap.getRefTrapezoidalMapDataset()->getPoint(dTrap.getIdPointR());
    else
        pointR = cg3::Point2d(dTrapMap.getBoundingBox().max().x(), dTrapMap.getBoundingBox().max().y());

    // Retrieve the top segment
    if (dTrap.getIdSegmentT() != Trapezoid::NO_ID) {
        segmentT = dTrapMap.getRefTrapezoidalMapDataset()->getSegment(dTrap.getIdSegmentT());
        if (segmentT.p1().x() > segmentT.p2().x()) segmentT = cg3::Segment2d(segmentT.p2(), segmentT.p1());
    }
    else
        segmentT = cg3::Segment2d(cg3::Point2d(dTrapMap.getBoundingBox().min().x(), dTrapMap.getBoundingBox().max().y()),
                                  cg3::Point2d(dTrapMap.getBoundingBox().max().x(), dTrapMap.getBoundingBox().max().y()));

    // Retrieve the bottom segment
    if (dTrap.getIdSegmentB() != Trapezoid::NO_ID) {
        segmentB = dTrapMap.getRefTrapezoidalMapDataset()->getSegment(dTrap.getIdSegmentB());
        if (segmentB.p1().x() > segmentB.p2().x()) segmentB = cg3::Segment2d(segmentB.p2(), segmentB.p1());
    }
    else
        segmentB = cg3::Segment2d(cg3::Point2d(dTrapMap.getBoundingBox().min().x(), dTrapMap.getBoundingBox().min().y()),
                                  cg3::Point2d(dTrapMap.getBoundingBox().max().x(), dTrapMap.getBoundingBox().min().y()));

    /* Compute the intersections between the segments and the vertical lines passing through the points */

    // Segment slopes
    double mT = (segmentT.p2().y()-segmentT.p1().y()) / (segmentT.p2().x()-segmentT.p1().x());
    double mB = (segmentB.p2().y()-segmentB.p1().y()) / (segmentB.p2().x()-segmentB.p1().x());

    dTrap.setVertexTL(cg3::Point2d(pointL.x(), segmentT.p1().y() + mT * (pointL.x() - segmentT.p1().x()))); // Top-left
    dTrap.setVertexTR(cg3::Point2d(pointR.x(), segmentT.p2().y() - mT * (segmentT.p2().x() - pointR.x()))); // Top-right
    dTrap.setVertexBR(cg3::Point2d(pointR.x(), segmentB.p2().y() - mB * (segmentB.p2().x() - pointR.x()))); // Bottom-right
    dTrap.setVertexBL(cg3::Point2d(pointL.x(), segmentB.p1().y() + mB * (pointL.x() - segmentB.p1().x()))); // Bottom-left
}

/**
 * @brief setDrawableTrapezoidColor
 * @param[in,out] dTrap
 */
void setDrawableTrapezoidColor(DrawableTrapezoid &dTrap)
{
    /* Set the drawable trapezoid color with random:
     *  - hue in the interval [0,359]
     *  - saturation in the interval [128,255]
     *  - value in the interval [128,255]
     * Avoid the use of gray/black color, used instead for vertical lines and higlighted trapezoids */
    dTrap.setColor(rand() % 359, 128 + rand() % 128, 128 + rand() % 128);
}

} // End namespace gasprj
