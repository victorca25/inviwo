/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Friday, October 13, 2017 - 14:02:53
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <dh2320lab3/dh2320lab3module.h>
#include <dh2320lab3/raytracer.h>

namespace inviwo {

DH2320Lab3Module::DH2320Lab3Module(InviwoApplication* app) : InviwoModule(app, "DH2320Lab3") {
    registerProcessor<Raytracer>();
}

} // namespace
