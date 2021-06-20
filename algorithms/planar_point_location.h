#ifndef PLANAR_POINT_LOCATION_H
#define PLANAR_POINT_LOCATION_H

#include <cg3/geometry/segment2.h>

#include "data_structures/dag.h"
#include "drawables/drawable_trapezoidalmap.h"
#include "drawables/drawable_trapezoidalmap_dataset.h"

namespace gasprj {

/* Builders */
void initTrapezoidalMap(TrapezoidalMap &trapMap, DAG &dag);
void addSegmentToTrapezoidalMap(const cg3::Segment2d &segment, TrapezoidalMap &trapMap, DAG &dag);

/* Query */
size_t queryTrapezoidalMap(const cg3::Point2d &point, const TrapezoidalMap trapMap, const DAG &dag);

} // End namespace gasprj

#endif // PLANAR_POINT_LOCATION_H
