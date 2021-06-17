#include "drawable_trapezoid.h"

#include <GL/gl.h>

#include <cg3/viewer/opengl_objects/opengl_objects2.h>

namespace gasprj {

/**
 * @brief Default constructor of a drawable trapezoid
 */
DrawableTrapezoid::DrawableTrapezoid(Trapezoid trapezoid) :
    Trapezoid(trapezoid)
{
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
