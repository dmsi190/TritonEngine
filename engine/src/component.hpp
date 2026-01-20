// component.hpp

#pragma once

#include "object.hpp"
#include "types.hpp"

namespace triton
{
    class cComponent : public iObject
    {
        TRITON_OBJECT(cComponent)

    public:
        explicit cComponent(cContext* context);
        virtual ~cComponent() override = default;
    };
}