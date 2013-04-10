#ifndef IVW_EVENT_H
#define IVW_EVENT_H

#include <inviwo/core/inviwocoredefine.h>
#include <inviwo/core/inviwo.h>
#include <inviwo/core/io/serialization/ivwserializable.h>

namespace inviwo {

class IVW_CORE_API Event : public IvwSerializable {
public:
    enum Modifier {
        MODIFIER_NONE  =      0,
        MODIFIER_ALT   = 1 << 1,
        MODIFIER_CTRL  = 1 << 2,
        MODIFIER_SHIFT = 1 << 3,
        COUNT
    };

    Event();
    virtual ~Event();

    inline int button() const { return button_; }
    inline Event::Modifier modifier() const { return modifier_; }
    inline std::string modifierName() const { return modifierName_; }
    inline std::string buttonName() const { return buttonName_; }
    virtual void serialize(IvwSerializer& s) const;
    virtual void deserialize(IvwDeserializer& s);

protected:
    Event::Modifier modifier_;
    int button_;
    std::string modifierName_;
    std::string buttonName_;
    std::string modifierNames_[COUNT];
};

} // namespace

#endif // IVW_EVENT_H