#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include <limits>
#include <vector>

namespace gasprj {

/**
 * @brief The trapezoid data structure
 *
 * This class defines a trapezoid of a trapezoidal map, for the specific case of segments in general position.
 * Every trapezoid is uniquely identified by its left and right points and its top and bottom segments.
 * Every trapezoid has a maximum of 4 adjacient trapezoids (top-left, top-right, bottom-left, bottom-right) and stores
 * a reference to the DAG leaf which represent it.
 */
class Trapezoid
{
public:
    /**
     * @brief Definition of no reference to any data
     */
    static constexpr size_t NO_ID = std::numeric_limits<size_t>::max();

    /* Constructors */
    Trapezoid();
    Trapezoid(size_t idSegmentT, size_t idSegmentB, size_t idPointL, size_t idPointR,
              size_t idTrapezoidTL, size_t idTrapezoidTR, size_t idTrapezoidBL, size_t idTrapezoidBR, size_t idDagLeaf);

    /* Getters and setters */
    size_t getIdSegmentT() const;
    void setIdSegmentT(size_t id);
    size_t getIdSegmentB() const;
    void setIdSegmentB(size_t id);
    size_t getIdPointL() const;
    void setIdPointL(size_t id);
    size_t getIdPointR() const;
    void setIdPointR(size_t id);

    size_t getIdAdjacencyTL() const;
    void setIdAdjacencyTL(size_t id);
    size_t getIdAdjacencyTR() const;
    void setIdAdjacencyTR(size_t id);
    size_t getIdAdjacencyBL() const;
    void setIdAdjacencyBL(size_t id);
    size_t getIdAdjacencyBR() const;
    void setIdAdjacencyBR(size_t id);

    size_t getIdDagLeaf() const;
    void setIdDagLeaf(size_t id);

protected:
    /* Attributes */
    size_t idSegmentT, idSegmentB, idPointL, idPointR;
    size_t idAdjacentTrapezoidTL, idAdjacentTrapezoidTR, idAdjacentTrapezoidBL, idAdjacentTrapezoidBR;
    size_t idDagLeaf;
};

} // End namespace gasprj

#endif // TRAPEZOID_H
