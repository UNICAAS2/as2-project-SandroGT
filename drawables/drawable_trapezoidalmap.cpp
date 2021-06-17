#include "drawable_trapezoidalmap.h"

#include <GL/gl.h>

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

namespace gasprj {

DrawableTrapezoidalMap::DrawableTrapezoidalMap(TrapezoidalMapDataset *const trapezoidalMapDataset,
                                               const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR) :
    TrapezoidalMap(trapezoidalMapDataset, boundingBoxCornerBL, boundingBoxCornerTR),
    idHighlightedTrapezoid(Trapezoid::NO_ID)
{
}

void DrawableTrapezoidalMap::draw() const
{
    // Cycle over all the drawable trapezoids
    size_t i = 0;
    for (const DrawableTrapezoid &dTrap : trapezoids) {

        // Define the trapezoid color
        if (i == idHighlightedTrapezoid)
            glColor4d(0.3*255, 0.3*255, 0.3*255, TRANSPARENCY_SELECTED);
        else
            glColor4d(dTrap.getColorR(), dTrap.getColorG(), dTrap.getColorB(), TRANSPARENCY_UNSELECTED);

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

cg3::Point3d DrawableTrapezoidalMap::sceneCenter() const
{
    return cg3::Point3d(this->boundingBox.center().x(), this->boundingBox.center().y(), 0);
}

double DrawableTrapezoidalMap::sceneRadius() const
{
    return this->boundingBox.diag();
}

size_t DrawableTrapezoidalMap::getIdHighlightedTrapezoid() const
{
    return idHighlightedTrapezoid;
}

void DrawableTrapezoidalMap::setIdHighlightedTrapezoid(size_t id)
{
    idHighlightedTrapezoid = id;
}

Trapezoid &DrawableTrapezoidalMap::getTrapezoid(size_t id)
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

const Trapezoid &DrawableTrapezoidalMap::getTrapezoid(size_t id) const
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

size_t DrawableTrapezoidalMap::size() const
{
    return trapezoids.size();
}

void DrawableTrapezoidalMap::addTrapezoid(const Trapezoid &trapezoid)
{
    DrawableTrapezoid drawableTrapezoid = DrawableTrapezoid(trapezoid);
    setDrawableTrapezoidVertices(*this, drawableTrapezoid);
    setDrawableTrapezoidColor(drawableTrapezoid);

    trapezoids.push_back(drawableTrapezoid);
}

void DrawableTrapezoidalMap::overwriteTrapezoid(const Trapezoid &trapezoid, size_t id)
{
    assert(id < trapezoids.size());
    DrawableTrapezoid drawableTrapezoid = DrawableTrapezoid(trapezoid);
    setDrawableTrapezoidVertices(*this, drawableTrapezoid);
    setDrawableTrapezoidColor(drawableTrapezoid);

    trapezoids[id] = drawableTrapezoid;
}

void DrawableTrapezoidalMap::clear() {
    trapezoids.clear();
    idHighlightedTrapezoid = Trapezoid::NO_ID;
}

} // End namespace gasprj
