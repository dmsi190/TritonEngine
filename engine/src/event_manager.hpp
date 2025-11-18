// event_manager.hpp

#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include "types.hpp"

namespace realware
{
    namespace app
    {
        class cApplication;
    }

    namespace game
    {
        class cGameObject;
    }

    namespace utils
    {
        class cBuffer;

        enum class eEventType
        {
            NONE,
            KEY_PRESS
        };

        using EventFunction = std::function<void(cBuffer* const data)>;

        class cEvent
        {
            friend class mEvent;

        public:
            cEvent(const eEventType& type, const EventFunction& function);
            ~cEvent() = default;

            void Invoke(cBuffer* const data);
            inline game::cGameObject* GetReceiver() { return _receiver; }
            inline eEventType GetType() { return _type; }
            inline EventFunction& GetFunction() { return _function; }

        private:
            game::cGameObject* _receiver = nullptr;
            eEventType _type = eEventType::NONE;
            EventFunction _function;
        };

        class mEvent
        {
        public:
            explicit mEvent(const app::cApplication* const app);
            ~mEvent() = default;
            
            void Subscribe(const game::cGameObject* receiver, cEvent& event);
            void Unsubscribe(const game::cGameObject* receiver, cEvent& event);
            void Send(const eEventType& type);
            void Send(const eEventType& type, cBuffer* const data);

        private:
            app::cApplication* _app = nullptr;
            std::unordered_map<eEventType, std::vector<cEvent>> _listeners;
        };
    }
}