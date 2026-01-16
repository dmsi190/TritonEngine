// object.cpp

#include "object.hpp"
#include "event_manager.hpp"
#include "context.hpp"

namespace realware
{
	cIdentifier::cIdentifier(const std::string& id) : _id(id) {}

    cFactoryObject::~cFactoryObject()
    {
        delete _identifier;
    }

    void cFactoryObject::Unsubscribe(eEventType type, cGameObject* receiver)
    {
        cEventDispatcher* dispatcher = _context->GetSubsystem<cEventDispatcher>();
        dispatcher->Unsubscribe(type, receiver);
    }

    void cFactoryObject::Send(eEventType type)
    {
        cEventDispatcher* dispatcher = _context->GetSubsystem<cEventDispatcher>();
        dispatcher->Send(type);
    }

    void cFactoryObject::Send(eEventType type, cDataBuffer* data)
    {
        cEventDispatcher* dispatcher = _context->GetSubsystem<cEventDispatcher>();
        dispatcher->Send(type, data);
    }
}