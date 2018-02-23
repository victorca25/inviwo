/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Thursday, October 19, 2017 - 12:14:16
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#pragma once

#include <dh2320lab3/dh2320lab3moduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <dh2320lab3/material.h>

namespace inviwo {

/** \class ConstantMaterial
    \brief Material with constant color 
    (shaded according to the color irrespective of lighting)

    @author Himangshu Saikia
*/
class IVW_MODULE_DH2320LAB3_API ConstantMaterial : public Material {
    //Friends
    //Types
public:

    //Construction / Deconstruction
public:
    ConstantMaterial(const vec3& color = vec3(0, 0.4, 0.8));
    virtual ~ConstantMaterial() = default;

    //Methods
public:
    vec4 shade(const RayIntersection& intersection, const Light& light) const override;
    //Attributes
public:

};

} // namespace
