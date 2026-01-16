// input.cpp

#include <windows.h>
#include <GLFW/glfw3.h>
#include "input.hpp"
#include "context.hpp"
#include "graphics.hpp"

using namespace types;

namespace realware
{
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        const cpuword keyBufferMask = 0xFF;
        key &= keyBufferMask;

        cContext* context = (cContext*)glfwGetWindowUserPointer(window);
        cInput* input = context->GetSubsystem<cInput>();

        if (action == GLFW_PRESS)
            input->SetKey(key, K_TRUE);
        else if (action == GLFW_RELEASE)
            input->SetKey(key, K_FALSE);
    }

    void WindowFocusCallback(GLFWwindow* window, int focused)
    {
        cContext* context = (cContext*)glfwGetWindowUserPointer(window);
        cInput* input = context->GetSubsystem<cInput>();

        if (focused)
        {
            if (input->GetWindowFocus() == K_FALSE)
                input->SetWindowFocus(K_TRUE);
        }
        else
        {
            input->SetWindowFocus(K_FALSE);
        }
    }

    void WindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        cContext* context = (cContext*)glfwGetWindowUserPointer(window);
        cInput* input = context->GetSubsystem<cInput>();
        cGraphics* gfx = context->GetSubsystem<cGraphics>();

        input->_window->_width = width;
        input->_window->_height = height;

        gfx->ResizeRenderTargets(glm::vec2(width, height));
    }

    void CursorCallback(GLFWwindow* window, double xpos, double ypos)
    {
        cContext* context = (cContext*)glfwGetWindowUserPointer(window);
        cInput* input = context->GetSubsystem<cInput>();

        input->SetCursorPosition(glm::vec2(xpos, ypos));
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        cContext* context = (cContext*)glfwGetWindowUserPointer(window);
        cInput* input = context->GetSubsystem<cInput>();

        if (action == GLFW_RELEASE)
            input->SetMouseKey(button, 0);
        else if (action == GLFW_PRESS)
            input->SetMouseKey(button, 1);
    }

    void cWindow::SetWindow(GLFWwindow* window, types::usize width, types::usize height)
    {
        _window = window;
        _width = width;
        _height = height;
    }
    
    cInput::cInput(cContext* context) : iObject(context) {}

    cInput::~cInput()
    {
        if (_window)
            _context->Destroy<cWindow>(_window);
    }

    void cInput::SwapBuffers()
    {
        glfwSwapBuffers(_window->GetWindow());
    }

	void cInput::PollEvents()
	{
		glfwPollEvents();
	}

    glm::vec2 cInput::GetMonitorSize() const
    {
        return glm::vec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }

	void cInput::SetWindow(usize width, usize height, const std::string& title)
	{
        if (_window)
            return;

        _window = _context->Create<cWindow>(_context);

        if (_initialized == types::K_FALSE)
        {
            _initialized = types::K_TRUE;

            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        if (fullscreen == K_FALSE)
        {
            _window->SetWindow(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr), width, height);
        }
        else
        {
            glfwWindowHint(GLFW_DECORATED, 0);

            const glm::vec2 monitorSize = GetMonitorSize();
            const usize newWidth = monitorSize.x;
            const usize newHeight = monitorSize.y;
            _window->SetWindow(glfwCreateWindow(newWidth, newHeight, title.c_str(), glfwGetPrimaryMonitor(), nullptr), newWidth, newHeight);
        }

        GLFWwindow* glfwWindow = _window->GetWindow();

        glfwSetWindowUserPointer(glfwWindow, _context);

        if (!glfwWindow)
        {
            Print("Error: incompatible GL version!");
            return;
        }

        glfwMakeContextCurrent(glfwWindow);

        glfwSwapInterval(1);

        glfwSetKeyCallback(glfwWindow, &KeyCallback);
        glfwSetWindowFocusCallback(glfwWindow, &WindowFocusCallback);
        glfwSetWindowSizeCallback(glfwWindow, &WindowSizeCallback);
        glfwSetCursorPosCallback(glfwWindow, &CursorCallback);
        glfwSetMouseButtonCallback(glfwWindow, &MouseButtonCallback);
	}
}