/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Thursday, October 12, 2017 - 11:11:30
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <dh2320lab1/cubeanimator.h>

namespace inviwo
{

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo CubeAnimator::processorInfo_
{
    "org.inviwo.CubeAnimator",      // Class identifier
    "Cube Animator",                // Display name
    "DH2320",                 // Category
    CodeState::Experimental,  // Code state
    Tags::None,               // Tags
};

const ProcessorInfo CubeAnimator::getProcessorInfo() const
{
    return processorInfo_;
}


CubeAnimator::CubeAnimator()
    :Processor()
    , meshIn_("meshIn")
    , meshOut_("meshOut")
    , radius_("radius", "Radius", 6, 1, 8)
{
    addPort(meshIn_);
    addPort(meshOut_);
    addProperty(radius_);
}


void CubeAnimator::process()
{
    // Clone the input mesh
    auto mesh = meshIn_.getData()->clone();

    //  
    auto matrix = mesh->getWorldMatrix();

    matrix *= glm::translate(vec3(radius_.get(), 0, 0));
	
    mesh->setWorldMatrix(matrix);
	
    meshOut_.setData(mesh);
}

} // namespace

