#include "drawable_trapezoid.h"

namespace gasprj {

/* Private static constants */
const float DrawableTrapezoid::TRAPEZOID_TRANSPARENCY = 0.25;
const cg3::Color DrawableTrapezoid::COLOR_TRAPEZOID_SELECTED = cg3::Color(0.5*255, 0.5*255, 0.5*255, 0.75*255);
const cg3::Color DrawableTrapezoid::COLOR_VERTICAL_LINE = cg3::Color(0.1*255, 0.1*255, 0.1*255, 0.75*255);
const float DrawableTrapezoid::WIDTH_VERTICAL_LINE = 0.50;



/**
 * @brief Draw the trapezoid
 */
void DrawableTrapezoid::draw() const
{
    // Define the width of the vertical lines
    glLineWidth(DrawableTrapezoid::WIDTH_VERTICAL_LINE);

    // Define the color of the vertical segments
    glColor4d(DrawableTrapezoid::COLOR_VERTICAL_LINE.redF(), DrawableTrapezoid::COLOR_VERTICAL_LINE.greenF(),
              DrawableTrapezoid::COLOR_VERTICAL_LINE.blueF(), DrawableTrapezoid::COLOR_VERTICAL_LINE.alphaF());

    // Draw the vertical segments
    glBegin(GL_LINES);
    glVertex2d(this->vertexTL.x(), this->vertexTL.y());
    glVertex2d(this->vertexBL.x(), this->vertexBL.y());
    glVertex2d(this->vertexTR.x(), this->vertexTR.y());
    glVertex2d(this->vertexBR.x(), this->vertexBR.y());
    glEnd();

    // Define the trapezoid color
    if (this->highlighted)
        glColor4d(DrawableTrapezoid::COLOR_TRAPEZOID_SELECTED.redF(), DrawableTrapezoid::COLOR_TRAPEZOID_SELECTED.greenF(),
                  DrawableTrapezoid::COLOR_TRAPEZOID_SELECTED.blueF(), DrawableTrapezoid::COLOR_TRAPEZOID_SELECTED.alphaF());
    else
        glColor4d(this->color.redF(), this->color.greenF(), this->color.blueF(), DrawableTrapezoid::TRAPEZOID_TRANSPARENCY);

    // Draw the trapezoid
    glBegin(GL_POLYGON);
    glVertex2d(this->vertexTL.x(), this->vertexTL.y());
    glVertex2d(this->vertexTR.x(), this->vertexTR.y());
    glVertex2d(this->vertexBR.x(), this->vertexBR.y());
    glVertex2d(this->vertexBL.x(), this->vertexBL.y());
    glEnd();
}

} // End namespace gasprj
