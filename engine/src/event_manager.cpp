// event_manager.cpp

#pragma once

#include "application.hpp"
#include "gameobject_manager.hpp"
#include "event_manager.hpp"
#include "buffer.hpp"

using namespace types;

namespace realware
{
    cEvent::cEvent(eEvent type, const EventFunction& function) : _type(type), _function(function)
    {
    }

    void cEvent::Invoke(cBuffer* data)
    {
        _function(data);
    };

    mEvent::mEvent(cApplication* app) : _app(app)
    {
    }

    void mEvent::Subscribe(const cGameObject* receiver, cEvent& event)
    {
        event._receiver = (cGameObject*)receiver;

        const eEvent eventType = event.GetEventType();
        const auto listener = _listeners.find(eventType);
        if (listener == _listeners.end())
            _listeners.insert({ eventType, {}});
        _listeners[eventType].push_back(event);
    }

    void mEvent::Unsubscribe(const cGameObject* receiver, cEvent& event)
    {
        if (_listeners.find(event.GetEventType()) == _listeners.end())
            return;

        const eEvent eventType = event.GetEventType();
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

    void mEvent::Send(eEvent type)
    {
        cBuffer data;

        Send(type, &data);
    }

    void mEvent::Send(eEvent type, cBuffer* data)
    {
        for (auto& event : _listeners[type])
            event.Invoke(data);
    }
}