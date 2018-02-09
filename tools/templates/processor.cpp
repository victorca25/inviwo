/*********************************************************************
 *  Author  : <author>
 *  Init    : <datetime>
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <incfile>

namespace inviwo
{
namespace kth
{

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo <name>::processorInfo_
{
    "org.inviwo.<name>",      // Class identifier
    "<dname>",                // Display name
    "Undefined",              // Category
    CodeState::Experimental,  // Code state
    Tags::None,               // Tags
};

const ProcessorInfo <name>::getProcessorInfo() const
{
    return processorInfo_;
}

    
<name>::<name>()
    :Processor()
{
    //addPort();
    //addProperty();
}
    

void <name>::process()
{

}

} // namespace
} // namespace

