/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Wednesday, September 20, 2017 - 12:04:15
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#pragma once

#include <labstreamlines/labstreamlinesmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <inviwo/core/datastructures/volume/volume.h>


namespace inviwo
{

class IVW_MODULE_LABSTREAMLINES_API Integrator
{ 
//Friends
//Types
public:

//Construction / Deconstruction
public:
    Integrator();
    virtual ~Integrator() = default;

//Methods
public:
    static vec2 sampleFromField(const Volume* vol, const vec2& position);

    // TODO: Implement the methods below (one integration step with either Euler or 
    // Runge-Kutte of 4th order integration method)
    // Pass any other properties that influence the integration process
    // Examples would be the stepsize, inegreation direction, ...
    //static vec2 RK4(const VolumeRAM* vr, size3_t dims, const vec2& position, ...);
    //static vec2 Euler(const VolumeRAM* vr, size3_t dims, const vec2& position, ...);
	static double arclength(const vec2 & position, const vec2 & position2);
	static vec2 RK4(const Volume* vr, const vec2& currentPoint, float stepsize, bool directionfield);
};

} // namespace
