/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Tuesday, October 17, 2017 - 10:25:10
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#pragma once

#include <dh2320lab3/dh2320lab3moduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <dh2320lab3/rayintersection.h>
#include <dh2320lab3/light.h>

namespace inviwo {
class RayIntersection;
class Light;

/** \class Material
    \brief Material defined by color and reflectence

    @author Himangshu Saikia
*/
class IVW_MODULE_DH2320LAB3_API Material {
    //Friends
    //Types
public:

    //Construction / Deconstruction
public:
    Material(const vec3& color = vec3(0.5, 0.5, 0.5), double reflectance = 0.0) :
                                                                                mColor(color)
                                                                                , mReflectance(reflectance) {
    }

    virtual ~Material() = default;

    //Methods
public:
    const vec3& color() const { return mColor; }
    virtual vec4 shade(const RayIntersection& intersection, const Light& light) const = 0;
    //Attributes
private:
    vec3 mColor;
    double mReflectance;
};

} // namespace
