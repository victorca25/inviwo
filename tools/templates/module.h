/*********************************************************************
 *  Author  : <author>
 *  Init    : <datetime>
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#pragma once

#include <modules/<lname>/<lname>moduledefine.h>
#include <inviwo/core/common/inviwomodule.h>

namespace inviwo
{

class IVW_MODULE_<uname>_API <name>Module : public InviwoModule
{
public:
    <name>Module(InviwoApplication* app);
    virtual ~<name>Module() = default;
};

} // namespace
