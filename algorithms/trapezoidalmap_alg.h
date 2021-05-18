#ifndef TRAPEZOIDALMAP_ALG_H
#define TRAPEZOIDALMAP_ALG_H

#include <cg3/geometry/segment2.h>

#include "data_structures/dag.h"
#include "drawables/drawable_trapezoidalmap_dataset.h"

namespace gasprj {

void addSegmentToTrapezoidalMap();
size_t queryTrapezoidalMap(const cg3::Point2d& point, const TrapezoidalMapDataset& trapMapData,
                           const gasprj::DAG& dag);

} // End namespace gasprj

#endif // TRAPEZOIDALMAP_ALG_H
