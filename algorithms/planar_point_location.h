#ifndef PLANAR_POINT_LOCATION_H
#define PLANAR_POINT_LOCATION_H

#include <cg3/geometry/segment2.h>

#include "data_structures/dag.h"
#include "drawables/drawable_trapezoidalmap.h"
#include "drawables/drawable_trapezoidalmap_dataset.h"

namespace gasprj {

size_t queryTrapezoidalMap(const cg3::Point2d& point, const TrapezoidalMapDataset& trapMapData,
                           const gasprj::DAG& dag);
size_t queryTrapezoidalMap(const cg3::Segment2d& newSegment, const TrapezoidalMapDataset& trapMapData,
                           const gasprj::DAG& dag);
void addSegmentToTrapezoidalMap(const cg3::Segment2d& newSegment, TrapezoidalMapDataset& trapMapData,
                                gasprj::TrapezoidalMap& trapMap, gasprj::DAG& dag);

} // End namespace gasprj

#endif // PLANAR_POINT_LOCATION_H
