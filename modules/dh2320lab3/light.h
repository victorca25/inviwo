/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Tuesday, October 17, 2017 - 10:37:00
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#pragma once

#include <dh2320lab3/dh2320lab3moduledefine.h>
#include <inviwo/core/common/inviwo.h>

namespace inviwo {

/** \class Light
    \brief Light defined by position and spectral intensity

    @author Himangshu Saikia
*/
class IVW_MODULE_DH2320LAB3_API Light {
    //Friends
    //Types
public:

    //Construction / Deconstruction
public:
    Light(const vec3& position = vec3(0, 0, 0), const vec3& spectralIntensity = vec3(1, 1, 1));
    virtual ~Light() = default;

    //Methods
public:
    const vec3& getPosition() const { return mPosition; }
    const vec3& getSpectralIntensity() const { return mSpectralIntensity; }

    //Attributes
private:
    vec3 mPosition;
    vec3 mSpectralIntensity;

};

} // namespace
