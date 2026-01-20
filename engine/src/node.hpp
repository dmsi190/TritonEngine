// node.hpp

#pragma once

#include <vector>
#include "object.hpp"
#include "types.hpp"

namespace triton
{
    class cNode : public iObject
    {
        TRITON_OBJECT(cNode)

    public:
        explicit cNode(cContext* context);
        virtual ~cNode() override = default;

    private:
        std::vector<cComponent*> _components = {};
        std::vector<cNode*> _children = {};
    };
}