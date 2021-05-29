#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include <vector>

#include <cg3/geometry/point2.h>

#include "data_structures/trapezoidalmap_dataset.h"

namespace gasprj {

#define NO_ID std::numeric_limits<size_t>::max()

class Trapezoid
{
public:
    Trapezoid();
    Trapezoid(size_t idSegmentT, size_t idSegmentB, size_t idPointL, size_t idPointR,
              size_t idTrapezoidTL, size_t idxTrapezoidTR, size_t idTrapezoidBR, size_t idxTrapezoidBL);

    size_t getIdSegmentT();
    void setIdSegmentT(size_t id);
    size_t getIdSegmentB();
    void setIdSegmentB(size_t id);
    size_t getIdPointL();
    void setIdPointL(size_t id);
    size_t getIdPointR();
    void setIdPointR(size_t id);

    size_t getIdTrapezoidTL();
    void setIdTrapezoidTL(size_t id);
    size_t getIdTrapezoidTR();
    void setIdTrapezoidTR(size_t id);
    size_t getIdTrapezoidBR();
    void setIdTrapezoidBR(size_t id);
    size_t getIdTrapezoidBL();
    void setIdTrapezoidBL(size_t id);

    void updateVertices(TrapezoidalMapDataset& trapMapData);
    const cg3::Point2d& getVertexTL() const;
    const cg3::Point2d& getVertexTR() const;
    const cg3::Point2d& getVertexBR() const;
    const cg3::Point2d& getVertexBL() const;

private:
    // Trapezoid identifiers
    size_t idSegmentT, idSegmentB, idPointL, idPointR;

    // Adjacencies
    size_t idTrapezoidTL, idxTrapezoidTR, idTrapezoidBR, idxTrapezoidBL;

    // Polygon vertices (precomputed for efficiency)
    cg3::Point2d vertexTL, vertexTR, vertexBR, vertexBL;
};

} // End namespace gasprj

#endif // TRAPEZOID_H
