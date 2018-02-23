/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Tuesday, October 24, 2017 - 17:17:44
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <dh2320lab3/phongmaterial.h>
#include <dh2320lab3/util.h>

namespace inviwo {

PhongMaterial::PhongMaterial(const vec3& color, const double reflectance, const double shininess,
                             const double Kd, const double Ks) :
                                                               Material(color, reflectance) {
    shininess_ = shininess;
    Kd_ = Kd;
    Ks_ = Ks;
}

vec4 PhongMaterial::shade(const RayIntersection& intersection, const Light& light) const {
    // get normal and light direction
    vec3 N = intersection.getNormal();
    vec3 L = Util::normalize(light.getPosition() - intersection.getPosition());

    double cosNL = std::max(double(dot(N, L)), double(0));

    // Programming Task 2: Extend this method.
    // This method currently implements a Lambert's material with ideal
    // diffuse reflection.
    // Your task is to implement a Phong shading model.
    //
    // Hints:
    //
    // 1. This function should return the sum of diffuse and specular parts as
    //    mentioned in slide 59 of Lecture 6. 

    // 2. The incident radiance from the light source is the light intensity divided by
    //    the quadratic distance of the light source from the point of intersection. (quadratic falloff)
    //
    // 3. The irradiance c_l is obtained by multiplying the incident radiance 
    //    to the cosine of the angle between the vectors L and N.
    //
    // 4. The view vector V is the direction of the ray which intersects the object.
    //
    // 5. The rest of the terms as per the slides are as follows (including normalization)
    //    c_r = material_color / PI
    //    p = mShininess
    //    c_p = r_f0 * (mShininess + 2) / (2 * PI)
    //
    return vec4(Util::scalarMult(cosNL, this->color()), 1.0);
}

} // namespace
