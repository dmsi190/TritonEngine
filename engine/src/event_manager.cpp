// event_manager.cpp

#pragma once

#include "application.hpp"
#include "gameobject_manager.hpp"
#include "event_manager.hpp"
#include "buffer.hpp"

namespace realware
{
    using namespace app;
    using namespace game;
    using namespace types;

    namespace utils
    {
        cEvent::cEvent(const eEventType& type, const EventFunction& function) : _type(type), _function(function)
        {
        }

        void cEvent::Invoke(cBuffer* const data)
        {
            _function(data);
        };

        mEvent::mEvent(const cApplication* const app) : _app((cApplication*)app)
        {
        }

        void mEvent::Subscribe(const cGameObject* receiver, cEvent& event)
        {
            event._receiver = (cGameObject*)receiver;

            const eEventType eventType = event.GetType();
            auto listener = _listeners.find(eventType);
            if (listener == _listeners.end())
                _listeners.insert({ eventType, {}});
            _listeners[eventType].push_back(event);
        }

        void mEvent::Unsubscribe(const cGameObject* receiver, cEvent& event)
        {
            if (_listeners.find(event.GetType()) == _listeners.end())
                return;

            const eEventType eventType = event.GetType();
            auto& events = _listeners[eventType];
            for (auto it = events.begin(); it != events.end();)
            {
                if (it->GetReceiver() == receiver)
                {
                    events.erase(it);
                    return;
                }
            }
        }

        void mEvent::Send(const eEventType& type)
        {
            cBuffer data;

            Send(type, &data);
        }

        void mEvent::Send(const eEventType& type, cBuffer* const data)
        {
            for (auto& event : _listeners[type])
                event.Invoke(data);
        }
    }
}