#include "drawable_trapezoidalmap.h"

#include <GL/gl.h>

namespace gasprj {

DrawableTrapezoidalMap::DrawableTrapezoidalMap(cg3::Point2d boundingBoxCornerBL,
                                               cg3::Point2d boundingBoxCornerTR) :
    TrapezoidalMap(boundingBoxCornerBL, boundingBoxCornerTR), highlightedTrapezoidId(NO_ID)
{

}

void DrawableTrapezoidalMap::draw() const
{
    // Always use the same set of randomly changing colors
    std::srand(RANDOM_COLORS_SEED);
    size_t i = 0;
    for (const gasprj::Trapezoid& trapezoid : getTrapezoids()) {
        // Define a random color
        double r = double(std::rand()) / RAND_MAX;
        double g = double(std::rand()) / RAND_MAX;
        double b = double(std::rand()) / RAND_MAX;
        glColor4d(r, g, b, i == highlightedTrapezoidId ? TRANSPARENCY_SELECTED : TRANSPARENCY_UNSELECTED);
        // Draw a trapezoid as a 4 vertex polygon, with random colors
        glBegin(GL_POLYGON);
        glVertex2d(trapezoid.getVertexTL().x(), trapezoid.getVertexTL().y());
        glVertex2d(trapezoid.getVertexTR().x(), trapezoid.getVertexTR().y());
        glVertex2d(trapezoid.getVertexBR().x(), trapezoid.getVertexBR().y());
        glVertex2d(trapezoid.getVertexBL().x(), trapezoid.getVertexBL().y());
        glEnd();
        i++;
    }
}

cg3::Point3d DrawableTrapezoidalMap::sceneCenter() const
{
    const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return cg3::Point3d(boundingBox.center().x(), boundingBox.center().y(), 0);
}

double DrawableTrapezoidalMap::sceneRadius() const
{
    const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return boundingBox.diag();
}


size_t DrawableTrapezoidalMap::getHighlightedTrapezoidId() const {
    return highlightedTrapezoidId;
}

void DrawableTrapezoidalMap::setHighlightedTrapezoidId(size_t id) {
    highlightedTrapezoidId = id;
}

} // End namespace gasprj
