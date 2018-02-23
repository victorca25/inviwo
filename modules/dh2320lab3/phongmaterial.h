/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Tuesday, October 24, 2017 - 17:17:44
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

/** \class PhongMaterial
    \brief Material that can be lit with the Phong illumination model

    @author Himangshu Saikia
*/
class IVW_MODULE_DH2320LAB3_API PhongMaterial : public Material {
    //Friends
    //Types
public:

    //Construction / Deconstruction
public:
    PhongMaterial(const vec3& color = vec3(0, 0.4, 0.8),
                  const double reflectance = 1.0,
                  const double shininess = 10.0,
                  const double Kd = 1.0,
                  const double Ks = 1.0);
    virtual ~PhongMaterial() = default;

    //Methods
public:
    vec4 shade(const RayIntersection& intersection, const Light& light) const override;
    //Attributes
public:
    double shininess_;
    double Kd_;
    double Ks_;
};

} // namespace
