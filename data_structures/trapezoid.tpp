#include "trapezoid.h"

namespace gasprj {

/**
 * @brief Default constructor of a trapezoid
 *
 * Initialize a trapezoid with no references to any point, segment, trapezoid or DAG leaf.
 */
inline Trapezoid::Trapezoid() :
    idSegmentT(NO_ID), idSegmentB(NO_ID), idPointL(NO_ID), idPointR(NO_ID),
    idAdjacentTrapezoidTL(NO_ID), idAdjacentTrapezoidTR(NO_ID),
    idAdjacentTrapezoidBL(NO_ID), idAdjacentTrapezoidBR(NO_ID),
    idDagLeaf(0)
{
}

/**
 * @brief Complete constructor of a trapezoid
 * @param[in] idSegmentT The reference (ID) of the top segment
 * @param[in] idSegmentB The reference (ID) of the bottom segment
 * @param[in] idPointL The reference (ID) of the left point
 * @param[in] idPointR The reference (ID) of the right point
 * @param[in] idTrapezoidTL The reference (ID) of the top-left adjacent trapezoid
 * @param[in] idTrapezoidTR The reference (ID) of the top-right adjacent trapezoid
 * @param[in] idTrapezoidBL The reference (ID) of the bottom-left adjacent trapezoid
 * @param[in] idTrapezoidBR The reference (ID) of the bottom-right adjacent trapezoid
 * @param[in] idDagLeaf The reference (ID) of the DAG leaf which represent the trapezoid
 *
 * Initialize a trapezoid specifying all its data references.
 */
inline Trapezoid::Trapezoid(size_t idSegmentT, size_t idSegmentB, size_t idPointL, size_t idPointR,
                     size_t idTrapezoidTL, size_t idTrapezoidTR, size_t idTrapezoidBL, size_t idTrapezoidBR, size_t idDagLeaf) :
    idSegmentT(idSegmentT), idSegmentB(idSegmentB), idPointL(idPointL), idPointR(idPointR),
    idAdjacentTrapezoidTL(idTrapezoidTL), idAdjacentTrapezoidTR(idTrapezoidTR),
    idAdjacentTrapezoidBL(idTrapezoidBL), idAdjacentTrapezoidBR(idTrapezoidBR),
    idDagLeaf(idDagLeaf)
{
}

/**
 * @brief Get the ID of the top segment
 * @return The ID of the top segment
 */
inline size_t Trapezoid::getIdSegmentT() const
{
    return idSegmentT;
}

/**
 * @brief Set the ID of the top segment
 * @param[in] id The new ID of the top segment
 */
inline void Trapezoid::setIdSegmentT(size_t id)
{
    idSegmentT = id;
}

/**
 * @brief Get the ID of the bottom segment
 * @return The ID of the bottom segment
 */
inline size_t Trapezoid::getIdSegmentB() const
{
    return idSegmentB;
}

/**
 * @brief Set the ID of the bottom segment
 * @param[in] id The new ID of the bottom segment
 */
inline void Trapezoid::setIdSegmentB(size_t id)
{
    idSegmentB = id;
}

/**
 * @brief Get the ID of the left point
 * @return The ID of the left point
 */
inline size_t Trapezoid::getIdPointL() const
{
    return idPointL;
}

/**
 * @brief Set the ID of the left point
 * @param[in] id The new ID of the left point
 */
inline void Trapezoid::setIdPointL(size_t id)
{
    idPointL = id;
}

/**
 * @brief Get the ID of the right point
 * @return The ID of the right point
 */
inline size_t Trapezoid::getIdPointR() const
{
    return idPointR;
}

/**
 * @brief Set the ID of the right point
 * @param[in] id The new ID of the right point
 */
inline void Trapezoid::setIdPointR(size_t id)
{
    idPointR = id;
}

/**
 * @brief Get the ID of the adjacent top-left trapezoid
 * @return The ID of the adjacent top-left trapezoid
 */
inline size_t Trapezoid::getIdAdjacencyTL() const
{
    return idAdjacentTrapezoidTL;
}

/**
 * @brief Set the ID of the adjacent top-left trapezoid
 * @param[in] id The new ID of the adjacent top-left trapezoid
 */
inline void Trapezoid::setIdAdjacencyTL(size_t id)
{
    idAdjacentTrapezoidTL = id;
}

/**
 * @brief Get the ID of the adjacent top-right trapezoid
 * @return The ID of the adjacent top-right trapezoid
 */
inline size_t Trapezoid::getIdAdjacencyTR() const
{
    return idAdjacentTrapezoidTR;
}

/**
 * @brief Set the ID of the adjacent top-right trapezoid
 * @param[in] id The new ID of the adjacent top-right trapezoid
 */
inline void Trapezoid::setIdAdjacencyTR(size_t id)
{
    idAdjacentTrapezoidTR = id;
}

/**
 * @brief Get the ID of the adjacent bottom-left trapezoid
 * @return The ID of the adjacent bottom-left trapezoid
 */
inline size_t Trapezoid::getIdAdjacencyBL() const
{
    return idAdjacentTrapezoidBL;
}

/**
 * @brief Set the ID of the adjacent bottom-left trapezoid
 * @param[in] id The new ID of the adjacent bottom-left trapezoid
 */
inline void Trapezoid::setIdAdjacencyBL(size_t id)
{
    idAdjacentTrapezoidBL = id;
}

/**
 * @brief Get the ID of the adjacent bottom-right trapezoid
 * @return The ID of the adjacent bottom-right trapezoid
 */
inline size_t Trapezoid::getIdAdjacencyBR() const
{
    return idAdjacentTrapezoidBR;
}

/**
 * @brief Set the ID of the adjacent bottom-right trapezoid
 * @param[in] id The new ID of the adjacent bottom-right trapezoid
 */
inline void Trapezoid::setIdAdjacencyBR(size_t id)
{
    idAdjacentTrapezoidBR = id;
}

/**
 * @brief Get the ID of the DAG leaf related to this trapezoid
 * @return The ID of the DAG leaf related to this trapezoid
 */
inline size_t Trapezoid::getIdDagLeaf() const
{
    return idDagLeaf;
}

/**
 * @brief Set the ID of the DAG leaf related to this trapezoid
 * @param[in] id The new ID of the DAG leaf related to this trapezoid
 */
inline void Trapezoid::setIdDagLeaf(size_t id)
{
    idDagLeaf = id;
}

} // End namespace gasprj
