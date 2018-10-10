/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Wednesday, September 20, 2017 - 12:04:15
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <labtopo/integrator.h>
#include <labtopo/interpolator.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <inviwo/core/datastructures/volume/volumeram.h>

namespace inviwo {

Integrator::Integrator() {}

// TODO: Implementation of the functions defined in the header file integrator.h
    vec2 Integrator::findzeropossibility(const Volume* vol, const vec2& position, float distance){
        vec2 current_point = position;
        for(int i=0; i<2; i++){
            current_point.x = current_point.x+distance*i;
            current_point.y = position.y;
            for(int j=0; j<2; j++){
                current_point.y = current_point.y+distance*j;
                vec2 point00 = Interpolator::sampleFromField(vol, vec2(current_point.x,current_point.y));
                vec2 point10 = Interpolator::sampleFromField(vol, vec2(current_point.x+distance,current_point.y));
                vec2 point01 = Interpolator::sampleFromField(vol, vec2(current_point.x,current_point.y+distance));
                vec2 point11 = Interpolator::sampleFromField(vol, vec2(current_point.x+distance,current_point.y+distance));
                if(point00[0]>=0&&point10[0]>=0&&point01[0]>=0&&point11[0]>=0 ||point00[0]<=0&&point10[0]<=0&&point01[0]<=0&&point11[0]<=0||point00[1]>=0&&point10[1]>=0&&point01[1]>=0&&point11[1]>=0||point00[1]<=0&&point10[1]<=0&&point01[1]<=0&&point11[1]<=0)
                {}else{
                    return current_point;
                }
            }
        }
    }
}  // namespace inviwo
