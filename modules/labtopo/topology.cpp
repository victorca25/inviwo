/*********************************************************************
*  Author  : Anke Friederici
*
*  Project : KTH Inviwo Modules
*
*  License : Follows the Inviwo BSD license model
**********************************************************************/

#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <labtopo/integrator.h>
#include <labtopo/interpolator.h>
#include <labtopo/topology.h>
#include <labtopo/utils/gradients.h>
#include <labtopo/labtopomoduledefine.h>
#include <labtopo/utils/tnt/jama_eig.h>

namespace inviwo
{

const vec4 Topology::ColorsCP[6] =
    {
        vec4(1, 1, 0, 1),  // Saddle
        vec4(0, 0, 1, 1),  // AttractingNode
        vec4(1, 0, 0, 1),  // RepellingNode
        vec4(0.5, 0, 1, 1),// AttractingFocus
        vec4(1, 0.5, 0, 1),// RepellingFocus
        vec4(0, 1, 0, 1)   // Center
};

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo Topology::processorInfo_{
    "org.inviwo.Topology",  // Class identifier
    "Vector Field Topology",// Display name
    "KTH Lab",              // Category
    CodeState::Experimental,// Code state
    Tags::None,             // Tags
};

const ProcessorInfo Topology::getProcessorInfo() const
{
    return processorInfo_;
}

Topology::Topology()
    : Processor(), outMesh("meshOut"), inData("inData")
// TODO: Initialize additional properties
// propertyName("propertyIdentifier", "Display Name of the Propery",
// default value (optional), minimum value (optional), maximum value (optional), increment (optional));
// propertyIdentifier cannot have spaces
{
    // Register Ports
    addPort(outMesh);
    addPort(inData);

    // TODO: Register additional properties
    // addProperty(propertyName);
}

void Topology::process()
{
    // Get input
    if (!inData.hasData())
    {
        return;
    }
    auto vol = inData.getData();

    // Retreive data in a form that we can access it
    const VolumeRAM* vr = vol->getRepresentation<VolumeRAM>();
    uvec3 dims = vr->getDimensions();

    // Initialize mesh, vertices and index buffers for the two streamlines and the
    auto mesh = std::make_shared<BasicMesh>();
    std::vector<BasicMesh::Vertex> vertices;
    // Either add all line segments to this index buffer (one large buffer),
    // or use several index buffers with connectivity type adjacency.
    auto indexBufferSeparatrices = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
    auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);

    // TODO: Compute the topological skeleton of the input vector field.
    // Find the critical points and color them according to their type.
    // Integrate all separatrices.
    // You can use your previous integration code (copy it over or call it from <lablic/integrator.h>).

    // Looping through all values in the vector field.
    for (int y = 0; y < dims[1]; ++y)
        for (int x = 0; x < dims[0]; ++x){
            dvec2 vectorValue = vr->getAsDVec2(uvec3(x, y, 0));
            
            //Jane: get the vector of vertex
            vec2 point00 = Interpolator::sampleFromField(vol.get(), vec2(x,y));
            vec2 point10 = Interpolator::sampleFromField(vol.get(), vec2(x+1,y));
            vec2 point01 = Interpolator::sampleFromField(vol.get(), vec2(x,y+1));
            vec2 point11 = Interpolator::sampleFromField(vol.get(), vec2(x+1,y+1));
            //Find zero possible cell
            if(point00[0]>=0&&point10[0]>=0&&point01[0]>=0&&point11[0]>=0 ||point00[0]<=0&&point10[0]<=0&&point01[0]<=0&&point11[0]<=0||point00[1]>=0&&point10[1]>=0&&point01[1]>=0&&point11[1]>=0||point00[1]<=0&&point10[1]<=0&&point01[1]<=0&&point11[1]<=0)
            {}else{
                // Divide the square into four parts
                // Use Change of Sign to find
                float thresold = 0.1;
                float distance = 0.5;
                vec2 zeropossiblepoint = vec2(x,y);
                while(distance>thresold){
//                    LogProcessorInfo("Jacobian(0,0) is " << zeropossiblepoint <<  ". Distance is " << distance<< ".");
                    zeropossiblepoint = Integrator::findzeropossibility(vol.get(),zeropossiblepoint,distance);
                    distance = distance/2.0;
                }
                mat2 Jacobian = Interpolator::sampleJacobian(vol.get(), zeropossiblepoint);
                LogProcessorInfo("Jacobian(0,0) is " << zeropossiblepoint <<  ". Distance is " << Jacobian<< ".");
                util::EigenResult result;
                if((Jacobian[0][0]*Jacobian[1][1] - Jacobian[0][1]*Jacobian[1][0])!=0)
                {
                    result = util::eigenAnalysis(Jacobian);
                }
            }
        }

    mesh->addVertices(vertices);
    outMesh.setData(mesh);
}

}// namespace
