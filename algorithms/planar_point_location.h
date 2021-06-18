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

/* Helper functions */
void updateOneCrossedTrapezoid(const cg3::Segment2d &segment, size_t idCrossedTrap, TrapezoidalMap &trapMap, DAG &dag);
void updateMoreCrossedTrapezoids(const cg3::Segment2d &segment, const std::vector<size_t> &crossedTraps,
                                 TrapezoidalMap &trapMap, DAG &dag);
void crossedTrapezoids(const cg3::Segment2d &segment, const TrapezoidalMap &trapMap, const DAG &dag,
                       std::vector<size_t> &crossedTraps);
size_t queryToBuildTrapezoidalMap(const cg3::Segment2d &segment, const TrapezoidalMap trapMap, const DAG &dag);
bool doesOverlapL(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool doesOverlapR(const cg3::Segment2d &segment, size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointTL(size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointBL(size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointTR(size_t idTrapezoid, const TrapezoidalMap &trapMap);
bool hasEndpointBR(size_t idTrapezoid, const TrapezoidalMap &trapMap);

} // End namespace gasprj

#endif // PLANAR_POINT_LOCATION_H
