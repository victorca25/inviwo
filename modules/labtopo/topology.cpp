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
#include <cmath>

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
        auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
        
        // TODO: Compute the topological skeleton of the input vector field.
        // Find the critical points and color them according to their type.
        // Integrate all separatrices.
        // You can use your previous integration code (copy it over or call it from <lablic/integrator.h>).
        
        //Hara: Variables Needed to Create Seperatices
        float stepsize = 0.1;
        int numberofstep = 200;
        float minArcLength = 0.01;
        bool directionfield = true;
        vec2 priorpoint;
        vec2 currentpoint;
        vec2 StopPoint = vec2(0, 0);
        
        
        //    vec2 *saddle=new vec2[dims[1]*dims[0]];
        // Looping through all values in the vector field.
        int bufferindex = 0;
        
        std::vector<BasicMesh::Vertex> verticesSeparatrices;
        std::vector<BasicMesh::Vertex>** VertexArray = new std::vector<BasicMesh::Vertex>*[dims[0]*dims[1]];
        for (int i = 0; i < dims[0]*dims[1]; i++) {
            VertexArray[i] = new std::vector<BasicMesh::Vertex>[4];
        }
        
        int saddlepoint = 0;
        
        for (int y = 0; y < dims[1]; ++y)
            for (int x = 0; x < dims[0]; ++x){
                bufferindex++;
                dvec2 vectorValue = vr->getAsDVec2(uvec3(x, y, 0));
                
                //Jane: get the vector of vertex
                vec2 point00 = Interpolator::sampleFromField(vol.get(), vec2(x,y));
                vec2 point10 = Interpolator::sampleFromField(vol.get(), vec2(x+1,y));
                vec2 point01 = Interpolator::sampleFromField(vol.get(), vec2(x,y+1));
                vec2 point11 = Interpolator::sampleFromField(vol.get(), vec2(x+1,y+1));
                //Find zero possible cell
                if((point00[0]>=0&&point10[0]>=0&&point01[0]>=0&&point11[0]>=0) || (point00[0]<=0&&point10[0]<=0&&point01[0]<=0&&point11[0]<=0) || (point00[1]>=0&&point10[1]>=0&&point01[1]>=0&&point11[1]>=0) || (point00[1]<=0&&point10[1]<=0&&point01[1]<=0&&point11[1]<=0))
                {}else{
                    vec2 zeropossiblepoint = Integrator::findzeropossibility(vol.get(), vec2(x, y),0.5);
     
                    vec2 zerovector = Interpolator::sampleFromField(vol.get(), zeropossiblepoint);
                    
                    if (abs(zerovector.x) < 0.1 && abs(zerovector.y) < 0.1) {
                        mat2 Jacobian = Interpolator::sampleJacobian(vol.get(), zeropossiblepoint);
                        float det = Jacobian[0][0] * Jacobian[1][1] - Jacobian[0][1] * Jacobian[1][0];
                        util::EigenResult result;
                        // Jane: get the real part and imaginary part of eigenvalue
                        float R1, R2, I1, I2;
                        if (Jacobian[0][0] * Jacobian[1][1] - Jacobian[0][1] * Jacobian[1][0] != 0)
                        {
                            result = util::eigenAnalysis(Jacobian);
                            R1 = result.eigenvaluesRe[0];
                            R2 = result.eigenvaluesRe[1];
                            I1 = result.eigenvaluesIm[0];
                            I2 = result.eigenvaluesIm[1];
                            if (((R1<0 && R2>0)||(R1>0 && R2<0)) && I1 == 0 && I2 == 0) {
                                LogProcessorInfo("saddle point");
                                
                                //Add Points to Display
                                //                            vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(255, 0, 0, 1) });
                                //                            indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                //                            bufferindex++;
                                saddlepoint++;
                                
                                auto indexBufferSeparatrices = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
                                for(int m=0;m<2;m++){
                                    vec2 startPoint = zeropossiblepoint;
                                    VertexArray[saddlepoint][m].push_back({ vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
                                        vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                    indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                    bufferindex++;
                                    // TODO: Create one stream line from the given start point
                                    float stepsize=0.1;
                                    
                                    //Jane: Using RK4 method to draw stream lines
                                    vec2 currentPoint_RK4 = startPoint + stepsize*result.eigenvectors[m];
                                    LogProcessorInfo("startpoint" << currentPoint_RK4 << ".");
                                    VertexArray[saddlepoint][m].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 0),
                                        vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                    indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                    bufferindex++;
                                    for (int j = 0; j < 200; j++) {
                                        currentPoint_RK4 = Integrator::RK4(vol.get(), currentPoint_RK4, stepsize, 0);
                                        VertexArray[saddlepoint][m].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 1),
                                            vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                        indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                        bufferindex++;
                                    }
                                    
                                    mesh->addVertices(VertexArray[saddlepoint][m]);
                                }
                                
                                for(int n=2;n<4;n++){
                                    vec2 startPoint = zeropossiblepoint;
                                    VertexArray[saddlepoint][n].push_back({ vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
                                        vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                    indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                    bufferindex++;
                                    // TODO: Create one stream line from the given start point
                                    float stepsize=0.1;
                                    
                                    //Jane: Using RK4 method to draw stream lines
                                    vec2 currentPoint_RK4 = startPoint + float(-0.1)*stepsize*result.eigenvectors[n-2];
                                    LogProcessorInfo("startpoint" << currentPoint_RK4 << ".");
                                    VertexArray[saddlepoint][n].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 0),
                                        vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                    indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                    bufferindex++;
                                    for (int j = 0; j < 200; j++) {
                                        currentPoint_RK4 = Integrator::RK4(vol.get(), currentPoint_RK4, stepsize, 0);
                                        VertexArray[saddlepoint][n].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 1),
                                            vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                        indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                        bufferindex++;
                                    }
                                    
                                    mesh->addVertices(VertexArray[saddlepoint][n]);
                                }
                                
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), ColorsCP[0] });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                
                                //Add Separatices
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                
                                //Travel In Direction of EigenVector
                                for (int i = 0; i < 3; i++) {
                                    vec2 initialdirection = result.eigenvectors[i];
                                    LogProcessorInfo("EigenVectors!!!!" << result.eigenvectors[i]);
                                    currentpoint = zeropossiblepoint + initialdirection*stepsize;
                                    vertices.push_back({ vec3(currentpoint.x / (dims.x - 1), currentpoint.y / (dims.y - 1), 0), vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                    indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                    bufferindex++;
                                    for (int j = 1; j < numberofstep; j++) {
                                        priorpoint = currentpoint;
                                        currentpoint = Integrator::RK4(vol.get(), currentpoint, stepsize, directionfield);
                                        if (Integrator::arclength(priorpoint, currentpoint) < minArcLength) break;//break loop if the velocity is too slow
                                        if (currentpoint == StopPoint) {
                                            break;
                                        }
                                        else {
                                            vertices.push_back({ vec3(currentpoint.x / (dims.x - 1), currentpoint.y / (dims.y - 1), 0), vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                            indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                            bufferindex++;
                                        }
                                    }
                                    vertices.push_back({ vec3(currentpoint.x / (dims.x - 1), currentpoint.y / (dims.y - 1), 0), vec3(0), vec3(0), vec4(255, 255, 255, 1) });
                                    indexBufferSeparatrices->add(static_cast<std::uint32_t>(bufferindex));
                                    bufferindex++;
                                }
                                
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1>0 && R2>0 && I1 == 0 && I2 == 0) {
                                LogProcessorInfo("Repelling node");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), ColorsCP[2] });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1<0 && R2<0 && I1 == 0 && I2 == 0) {
                                LogProcessorInfo("Attracting node");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), ColorsCP[1] });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (abs(R1) < 0.01 && R1 == R2 && I1 == -I2 && I1>0 && I2<0) {
                                LogProcessorInfo("Center");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), ColorsCP[5] });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1>0 && R2>0 && R1 == R2 && I1 == -I2 && I1 != 0) {
                                LogProcessorInfo("Repelling focus");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), ColorsCP[4] });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1<0 && R2<0 && R1 == R2 && I1 == -I2 && I1 != 0) {
                                LogProcessorInfo("Attracting focus");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), ColorsCP[3] });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
        
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1>0 && R2>0 && I1 == 0 && I2 == 0) {
                                LogProcessorInfo("Repelling node");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(0, 255, 0, 1) });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1<0 && R2<0 && I1 == 0 && I2 == 0) {
                                LogProcessorInfo("Attracting node");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(0, 0, 255, 1) });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1>0 && R2>0 && R1 == R2 && I1 == -I2 && I1 != 0) {
                                LogProcessorInfo("Repelling focus");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(0, 128, 128, 1) });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1 == 0 && R1 == R2 && I1 == -I2 && I1>0 && I2<0) {
                                LogProcessorInfo("Center");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(128, 128, 0, 1) });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                            else if (R1<0 && R2<0 && R1 == R2 && I1 == -I2 && I1 != 0) {
                                LogProcessorInfo("Attracting focus");
                                //Add Points to Display
                                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(128, 0, 128, 1) });
                                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                                bufferindex++;
                                LogProcessorInfo("zero is " << zeropossiblepoint << "det is " << det << ".");
                            }
                        }
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

    
//    vec2 *saddle=new vec2[dims[1]*dims[0]];
    // Looping through all values in the vector field.
    int bufferindex = 0;
    
    for (int y = 0; y < dims[1]; ++y)
        for (int x = 0; x < dims[0]; ++x){
            dvec2 vectorValue = vr->getAsDVec2(uvec3(x, y, 0));
            
            //Jane: get the vector of vertex
            vec2 point00 = Interpolator::sampleFromField(vol.get(), vec2(x,y));
            vec2 point10 = Interpolator::sampleFromField(vol.get(), vec2(x+1,y));
            vec2 point01 = Interpolator::sampleFromField(vol.get(), vec2(x,y+1));
            vec2 point11 = Interpolator::sampleFromField(vol.get(), vec2(x+1,y+1));
            //Find zero possible cell
            if((point00[0]>0&&point10[0]>0&&point01[0]>0&&point11[0]>0) || (point00[0]<0&&point10[0]<0&&point01[0]<0&&point11[0]<0) || (point00[1]>0&&point10[1]>0&&point01[1]>0&&point11[1]>0) || (point00[1]<0&&point10[1]<0&&point01[1]<0&&point11[1]<0))
            {}else{
            
                // Divide the square into four parts
                // Use Change of Sign to find
                float thresold = 0.000001;
                float distance = 0.5;
                
                vec2 zeropossiblepoint = vec2(x,y);
                while(distance>thresold){
//                    LogProcessorInfo("Jacobian(0,0) is " << zeropossiblepoint <<  ". Distance is " << distance<< ".");
                    zeropossiblepoint = Integrator::findzeropossibility(vol.get(),zeropossiblepoint,distance);
                    distance = distance/2.0;
                }
                
//                //Add Points to Display
//                vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(255, 0, 0, 1) });
//                indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
//                bufferindex++;
                
                mat2 Jacobian = Interpolator::sampleJacobian(vol.get(), zeropossiblepoint);
                //LogProcessorInfo("Jacobian(0,0) is " << zeropossiblepoint <<  ". Distance is " << Jacobian<< ".");
                LogProcessorInfo("x is " << zeropossiblepoint.x <<  ". y is " << zeropossiblepoint.y << ".");
                float det = Jacobian[0][0]*Jacobian[1][1] - Jacobian[0][1]*Jacobian[1][0];
                util::EigenResult result;
                // Jane: get the real part and imaginary part of eigenvalue
                float R1,R2,I1,I2;
                if(Jacobian[0][0]*Jacobian[1][1] - Jacobian[0][1]*Jacobian[1][0]!=0)
                {
                    result = util::eigenAnalysis(Jacobian);
                    R1 = result.eigenvaluesRe[0];
                    R2 = result.eigenvaluesRe[1];
                    I1 = result.eigenvaluesIm[0];
                    I2 = result.eigenvaluesIm[1];
                    if(R1<0 && R2>0 && I1==0 && I2==0){
                        LogProcessorInfo("saddle point");
//                        Add Points to Display
                        vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(255, 0, 0, 1) });
                        indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                        bufferindex++;
                        LogProcessorInfo("zero is "<< zeropossiblepoint<< "det is " << det<< ".");
                    }else if(R1>0 && R2>0 && I1==0 && I2==0){
                        LogProcessorInfo("Repelling node");
                        //Add Points to Display
                        vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(0, 255, 0, 1) });
                        indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                        bufferindex++;
                        LogProcessorInfo("zero is "<< zeropossiblepoint<< "det is " << det<< ".");
                    }else if(R1<0 && R2<0 && I1==0 && I2==0){
                        LogProcessorInfo("Attracting node");
                        //Add Points to Display
                        vertices.push_back({ vec3(zeropossiblepoint.x / (float)(dims.x - 1), zeropossiblepoint.y / (float)(dims.y - 1), 0), vec3(0), vec3(0), vec4(0, 0, 255, 1) });
                        indexBufferPoints->add(static_cast<std::uint32_t>(bufferindex));
                        bufferindex++;
                        LogProcessorInfo("zero is "<< zeropossiblepoint<< "det is " << det<< ".");
                    }

>>>>>>> 48a4fdd4536f7ca986e71b8a3aef719d68cd1ca8
                }
            }
        
        mesh->addVertices(vertices);
        outMesh.setData(mesh);
    }
    
}// namespace

