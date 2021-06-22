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
inline DrawableTrapezoidalMap::DrawableTrapezoidalMap(TrapezoidalMapDataset *const trapezoidalMapDataset,
                                               const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR) :
    TrapezoidalMap(trapezoidalMapDataset, boundingBoxCornerBL, boundingBoxCornerTR),
    idHighlightedTrapezoid(Trapezoid::NO_ID)
{
}

/**
 * @brief Draw the trapezoidal map
 */
inline void DrawableTrapezoidalMap::draw() const
{
    // Cycle over all the drawable trapezoids
    for (const DrawableTrapezoid &drawableTrapezoid : trapezoids)
        drawableTrapezoid.draw();
}

/**
 * @brief Get the center of the scene
 * @return The center of the scene as a 3D point
 */
inline cg3::Point3d DrawableTrapezoidalMap::sceneCenter() const
{
    return cg3::Point3d(this->boundingBox.center().x(), this->boundingBox.center().y(), 0);
}

/**
 * @brief Get the radius of the scene
 * @return The radius of the scene
 */
inline double DrawableTrapezoidalMap::sceneRadius() const
{
    return this->boundingBox.diag();
}

/**
 * @brief Get the ID of the highlighted trapezoid
 * @return The ID of the highlighted trapezoid
 */
inline size_t DrawableTrapezoidalMap::getIdHighlightedTrapezoid() const
{
    return idHighlightedTrapezoid;
}

/**
 * @brief Set the ID of the highlighted trapezoid
 * @param[in] id The new ID of the highlighted trapezoid
 */
inline void DrawableTrapezoidalMap::setIdHighlightedTrapezoid(size_t id)
{
    assert(id != Trapezoid::NO_ID ? id < trapezoids.size() : true);
    // Un-select the previous highlighted trapezoid
    if (idHighlightedTrapezoid != Trapezoid::NO_ID) trapezoids[idHighlightedTrapezoid].setHighlighted(false);
    // Highlights the new trapezoid
    idHighlightedTrapezoid = id;
    if (idHighlightedTrapezoid != Trapezoid::NO_ID) trapezoids[idHighlightedTrapezoid].setHighlighted(true);
}

/**
 * @brief Get a reference to the specified drawable trapezoid from the drawable trapezoidal map
 * @param[in] id The ID of the drawable trapezoid
 * @return A reference to the specified drawable trapezoid
 */
inline Trapezoid &DrawableTrapezoidalMap::getTrapezoid(size_t id)
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

/**
 * @brief Get a non-modifiable reference to the specified drawable trapezoid from the drawable trapezoidal map
 * @param[in] id The ID of the drawable trapezoid
 * @return A costant reference to the specified drawable trapezoid
 */
inline const Trapezoid &DrawableTrapezoidalMap::getTrapezoid(size_t id) const
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

/**
 * @brief Get the number of drawable trapezoids in the drawable trapezoidal map
 * @return The number of drawable trapezoids in the drawable trapezoidal map
 */
inline size_t DrawableTrapezoidalMap::size() const
{
    return trapezoids.size();
}

/**
 * @brief Add a new drawable trapezoid to the drawable trapezoidal map
 * @param[in] trapezoid The new trapezoid, converted to its drawable version
 */
inline void DrawableTrapezoidalMap::addTrapezoid(const Trapezoid &trapezoid)
{
    DrawableTrapezoid drawableTrapezoid = DrawableTrapezoid(trapezoid);
    setDrawableTrapezoidVertices(drawableTrapezoid);
    setDrawableTrapezoidColor(drawableTrapezoid);

    trapezoids.push_back(drawableTrapezoid);
}

/**
 * @brief Overwrite the specified drawable trapezoid in the drawable trapezoidal map
 * @param[in] trapezoid The new trapezoid, converted to its drawable version
 * @param[in] id The ID of the drawable trapezoid to overwrite
 */
inline void DrawableTrapezoidalMap::overwriteTrapezoid(const Trapezoid &trapezoid, size_t id)
{
    assert(id < trapezoids.size());
    DrawableTrapezoid drawableTrapezoid = DrawableTrapezoid(trapezoid);
    setDrawableTrapezoidVertices(drawableTrapezoid);
    setDrawableTrapezoidColor(drawableTrapezoid);

    trapezoids[id] = drawableTrapezoid;
}

/**
 * @brief Delete all the trapezoids stored in the drawable trapezoidal map
 */
inline void DrawableTrapezoidalMap::clear() {
    trapezoids.clear();
    idHighlightedTrapezoid = Trapezoid::NO_ID;
}



/* Internal methods declaration */

/**
 * @brief Set a random color to the drawable trapezoid
 * @param[in,out] dTrap The drawable trapezoid
 *
 * Set the drawable trapezoid color with random:
 *  - hue in the interval [0,359]
 *  - saturation in the interval [192,255]
 *  - value in the interval [192,255]
 * Avoid the use of gray/black color, used instead for vertical lines and higlighted trapezoids
 */
inline void DrawableTrapezoidalMap::setDrawableTrapezoidColor(DrawableTrapezoid &dTrap)
{
    dTrap.setColor(rand() % 359, 128 + rand() % 64, 128 + rand() % 64);
}

} // End namespace gasprj
