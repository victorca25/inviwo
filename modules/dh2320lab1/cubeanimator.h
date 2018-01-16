/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Thursday, October 12, 2017 - 11:11:30
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#pragma once

#include <dh2320lab1/dh2320lab1moduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/properties/ordinalproperty.h>

namespace inviwo
{

/** \docpage{org.inviwo.CubeAnimator, Cube Animator}
    ![](org.inviwo.CubeAnimator.png?classIdentifier=org.inviwo.CubeAnimator)

    Explanation of how to use the processor.
    
    ### Inports
      * __<Inport1>__ <description>.
    
    ### Outports
      * __<Outport1>__ <description>.
    
    ### Properties
      * __<Prop1>__ <description>.
      * __<Prop2>__ <description>
*/


/** \class CubeAnimator
    \brief VERY_BRIEFLY_DESCRIBE_THE_PROCESSOR
    
    DESCRIBE_THE_PROCESSOR_FROM_A_DEVELOPER_PERSPECTIVE

    @author Himangshu Saikia
*/
class IVW_MODULE_DH2320LAB1_API CubeAnimator : public Processor
{ 
//Friends
//Types
public:

//Construction / Deconstruction
public:
    CubeAnimator();
    virtual ~CubeAnimator() = default;

//Methods
public:
    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

protected:
    ///Our main computation function
    virtual void process() override;

//Ports
public:
    MeshInport meshIn_;
    MeshOutport meshOut_;

//Properties
public:
    FloatProperty radius_;

//Attributes
private:

};

} // namespace
