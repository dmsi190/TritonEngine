// application.cpp

#include <iostream>
#include "application.hpp"
#include "graphics.hpp"
#include "context.hpp"
#include "camera_manager.hpp"
#include "render_manager.hpp"
#include "render_context.hpp"
#include "sound_context.hpp"
#include "font_manager.hpp"
#include "sound_manager.hpp"
#include "filesystem_manager.hpp"
#include "physics_manager.hpp"
#include "gameobject_manager.hpp"
#include "texture_manager.hpp"
#include "memory_pool.hpp"
#include "event_manager.hpp"
#include "thread_manager.hpp"
#include "input.hpp"
#include "time.hpp"
#include "log.hpp"

using namespace types;

namespace realware
{
    iApplication::iApplication(cContext* context) : iObject(context)
    {
        _engine = std::make_shared<cEngine>(_context);
    }

    iApplication::~iApplication()
    {
    }

    void iApplication::Run()
    {
        _engine->Initialize();

        Setup();

        auto gfx = _context->GetSubsystem<cGraphics>();
        auto input = _context->GetSubsystem<cInput>();
        auto camera = _context->GetSubsystem<cCamera>();
        auto time = _context->GetSubsystem<cTime>();
        auto physics = _context->GetSubsystem<cPhysics>();

        time->BeginFrame();

        while (GetRunState() == K_FALSE)
        {
            time->Update();
            physics->Simulate();
            camera->Update();
            gfx->CompositeFinal();
            input->SwapBuffers();
            input->PollEvents();
        }

        time->EndFrame();

        Stop();
    }
}