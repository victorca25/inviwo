/*********************************************************************
 *  Author  : <author>
 *  Init    : <datetime>
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <<lname>/<lname>module.h>

namespace inviwo
{

using namespace kth;

<name>Module::<name>Module(InviwoApplication* app) : InviwoModule(app, "<name>")
{
    // Add a directory to the search path of the Shadermanager
    // ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:
    
    // Processors
    // registerProcessor<<name>Processor>();
    
    // Properties
    // registerProperty<<name>Property>();
    
    // Readers and writes
    // registerDataReader(util::make_unique<<name>Reader>());
    // registerDataWriter(util::make_unique<<name>Writer>());
    
    // Data converters
    // registerRepresentationConverter(util::make_unique<<name>Disk2RAMConverter>());

    // Ports
    // registerPort<<name>Outport>();
    // registerPort<<name>Inport>();

    // PropertyWidgets
    // registerPropertyWidget<<name>PropertyWidget, <name>Property>("Default");
    
    // Dialogs
    // registerDialog<<name>Dialog>(<name>Outport);
    
    // Other varius things
    // registerCapabilities(util::make_unique<<name>Capabilities>());
    // registerSettings(util::make_unique<<name>Settings>());
    // registerMetaData(util::make_unique<<name>MetaData>());   
    // registerPortInspector("<name>Outport", "path/workspace.inv");
    // registerProcessorWidget(std::string processorClassName, std::unique_ptr<ProcessorWidget> processorWidget);
    // registerDrawer(util::make_unique_ptr<<name>Drawer>());  
}

} // namespace
