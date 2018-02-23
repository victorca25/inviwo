/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Tuesday, October 17, 2017 - 10:37:00
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <dh2320lab3/light.h>

namespace inviwo {

Light::Light(const vec3& position, const vec3& spectralIntensity) {
    mPosition = position;
    mSpectralIntensity = spectralIntensity;
}

} // namespace
