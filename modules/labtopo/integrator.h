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

#include <labtopo/labtopomoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <functional>

namespace inviwo {

class IVW_MODULE_LABTOPO_API Integrator {
// Friends
// Types
public:
// Construction / Deconstruction
public:
    Integrator();
    virtual ~Integrator() = default;

// Methods
public:

    // TODO: Build on the last assignment by either copying the integrator code
    // here and in the respective .cpp or include the header from that
    // assignment with #include <lablic/integrator.h> in the files
    // where it is needed.
    // You may want to consider adding a helper function that computes an entire streamline
    // if you have not done so for the last assignments already.
    
    static vec2 findzeropossibility(const Volume* vol, const vec2& position, float distance);
};

}  // namespace inviwo
