// event_types.hpp

#include <functional>

namespace triton
{
    class cContext;
    class cDataBuffer;

    using EventFunction = std::function<void(cContext* context, cDataBuffer* data)>;

    enum class eEventType
    {
        NONE,

        KEY_PRESS,

        FRAME_BEGIN,
        FRAME_UPDATE,
        FRAME_END
    };
}