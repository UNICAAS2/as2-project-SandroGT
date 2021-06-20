#include "drawable_trapezoid.h"

#include <GL/gl.h>

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

#include "drawables/drawable_trapezoidalmap.h"

namespace gasprj {

/**
 * @brief Default constructor of a drawable trapezoid
 */
DrawableTrapezoid::DrawableTrapezoid(Trapezoid trapezoid) :
    Trapezoid(trapezoid), highlighted(false)
{
}

/**
 * @brief Draw the trapezoid
 */
void DrawableTrapezoid::draw() const
{
    // Define the width of the vertical lines
    glLineWidth(DrawableTrapezoidalMap::WIDTH_VERTICAL_LINE);

    // Define the color of the vertical segments
    glColor4d(DrawableTrapezoidalMap::COLOR_VERTICAL_LINE.redF(), DrawableTrapezoidalMap::COLOR_VERTICAL_LINE.greenF(),
              DrawableTrapezoidalMap::COLOR_VERTICAL_LINE.blueF(), DrawableTrapezoidalMap::COLOR_VERTICAL_LINE.alphaF());

    // Draw the vertical segments
    glBegin(GL_LINES);
    glVertex2d(this->vertexTL.x(), this->vertexTL.y());
    glVertex2d(this->vertexBL.x(), this->vertexBL.y());
    glVertex2d(this->vertexTR.x(), this->vertexTR.y());
    glVertex2d(this->vertexBR.x(), this->vertexBR.y());
    glEnd();

    // Define the trapezoid color
    if (this->highlighted)
        glColor4d(DrawableTrapezoidalMap::COLOR_TRAPEZOID_SELECTED.redF(), DrawableTrapezoidalMap::COLOR_TRAPEZOID_SELECTED.greenF(),
                  DrawableTrapezoidalMap::COLOR_TRAPEZOID_SELECTED.blueF(), DrawableTrapezoidalMap::COLOR_TRAPEZOID_SELECTED.alphaF());
    else
        glColor4d(this->color.redF(), this->color.greenF(), this->color.blueF(), DrawableTrapezoidalMap::TRAPEZOID_TRANSPARENCY);

    // Draw the trapezoid
    glBegin(GL_POLYGON);
    glVertex2d(this->vertexTL.x(), this->vertexTL.y());
    glVertex2d(this->vertexTR.x(), this->vertexTR.y());
    glVertex2d(this->vertexBR.x(), this->vertexBR.y());
    glVertex2d(this->vertexBL.x(), this->vertexBL.y());
    glEnd();
}

/**
 * @brief Get the center of the scene
 * @return The center of the scene as a 3D point
 */
cg3::Point3d DrawableTrapezoid::sceneCenter() const
{
    return cg3::Point3d(this->boundingBox.center().x(), this->boundingBox.center().y(), 0);
}

/**
 * @brief Get the radius of the scene
 * @return The radius of the scene
 */
double DrawableTrapezoid::sceneRadius() const
{
    return this->boundingBox.diag();
}

/**
 * @brief Get the color of the drawable trapezoid
 * @return The color of the drawable trapezoid
 */
const cg3::Color &DrawableTrapezoid::getColor() const
{
    return color;
}

/**
 * @brief Get the red channel of the color
 * @return The red color as a number in the interval [0,1]
 */
double DrawableTrapezoid::getColorR() const
{
    return color.redF();
}

/**
 * @brief Get the green channel of the color
 * @return The green color as a number in the interval [0,1]
 */
double DrawableTrapezoid::getColorG() const
{
    return color.greenF();
}

/**
 * @brief Get the blue channel of the color
 * @return The blue color as a number in the interval [0,1]
 */
double DrawableTrapezoid::getColorB() const
{
    return color.blueF();
}

/**
 * @brief Set the drawable trapezoid color using the HSV model
 * @param[in] h The hue in the interval [0,359]
 * @param[in] s The saturation in the interval [0,255]
 * @param[in] v The value in the interval [0,255]
 */
void DrawableTrapezoid::setColor(int h, int s, int v)
{
    // About QColor HSV (https://linux.die.net/man/3/qcolor)
    color.setHsv(h, s, v);
}

/**
 * @brief Check if the trapezoid is higlighted or not
 * @return True if the trapezoid is higlighted, false otherwise
 */
bool DrawableTrapezoid::isHighlighted() const
{
    return highlighted;
}

/**
 * @brief Set whether the trapezoid should be higlighted or not
 * @param[in] highlight True to highlight the trapezoid, false to not
 */
void DrawableTrapezoid::setHighlighted(bool highlight)
{
    highlighted = highlight;
}

/**
 * @brief Get the bounding box of the drawable trapezoid
 * @return The bounding box of the drawable trapezoid
 */
const cg3::BoundingBox2 &DrawableTrapezoid::getBoundingBox() const
{
    return boundingBox;
}

/**
 * @brief Set the bounding box of the drawable trapezoid
 * @param[in] boundingBoxCornerBL The bottom left corner of the new bounding box
 * @param[in] boundingBoxCornerTR The bottom right corner of the new bounding box
 */
void DrawableTrapezoid::setBoundingBox(const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR)
{
    boundingBox = cg3::BoundingBox2(boundingBoxCornerBL, boundingBoxCornerTR);
}

/**
 * @brief Get the top-left vertex of the drawable trapezoid
 * @return The top-left vertex of the drawable trapezoid
 */
const cg3::Point2d &DrawableTrapezoid::getVertexTL() const
{
    return vertexTL;
}

/**
 * @brief Set the top-left vertex of the drawable trapezoid
 * @param[in] vertex The new top-left vertex of the drawable trapezoid
 */
void DrawableTrapezoid::setVertexTL(const cg3::Point2d &vertex)
{
    vertexTL = vertex;
}

/**
 * @brief Get the top-right vertex of the drawable trapezoid
 * @return The top-right vertex of the drawable trapezoid
 */
const cg3::Point2d &DrawableTrapezoid::getVertexTR() const
{
    return vertexTR;
}

/**
 * @brief Set the top-right vertex of the drawable trapezoid
 * @param[in] vertex The new top-right vertex of the drawable trapezoid
 */
void DrawableTrapezoid::setVertexTR(const cg3::Point2d &vertex)
{
    vertexTR = vertex;
}

/**
 * @brief Get the bottom-left vertex of the drawable trapezoid
 * @return The bottom-left vertex of the drawable trapezoid
 */
const cg3::Point2d &DrawableTrapezoid::getVertexBL() const
{
    return vertexBL;
}

/**
 * @brief Set the bottom-left vertex of the drawable trapezoid
 * @param[in] vertex The new bottom-left vertex of the drawable trapezoid
 */
void DrawableTrapezoid::setVertexBL(const cg3::Point2d &vertex)
{
    vertexBL = vertex;
}

/**
 * @brief Get the bottom-right vertex of the drawable trapezoid
 * @return The bottom-right vertex of the drawable trapezoid
 */
const cg3::Point2d &DrawableTrapezoid::getVertexBR() const
{
    return vertexBR;
}

/**
 * @brief Set the bottom-right vertex of the drawable trapezoid
 * @param[in] vertex The new bottom-right vertex of the drawable trapezoid
 */
void DrawableTrapezoid::setVertexBR(const cg3::Point2d &vertex)
{
    vertexBR = vertex;
}

} // End namespace gasprj
