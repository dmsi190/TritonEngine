// application.hpp

#pragma once

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>
#include <chrono>
#include "object.hpp"
#include "engine.hpp"
#include "../../thirdparty/glm/glm/glm.hpp"
#include "types.hpp"

namespace realware
{
    class iRenderContext;
    class iSoundContext;
    class cRenderer;
    class cFontManager;
    class mCamera;
    class mGameObject;
    class mRender;
    class mTexture;
    class mSound;
    class mFont;
    class mPhysics;
    class mFileSystem;
    class mEvent;
    class mThread;

    struct sApplicationDescriptor
    {
        struct sWindowDescriptor
        {
            std::string _title = "DefaultWindow";
            types::u32 _width = 640;
            types::u32 _height = 480;
            types::boolean _isFullscreen = types::K_FALSE;
        };

        sWindowDescriptor _windowDesc;
        types::u32 _memoryPoolByteSize = 1 * 1024 * 1024;
        types::u32 _memoryPoolReservedAllocations = 65536;
        types::u32 _memoryPoolAlignment = 64;
        types::u32 _textureAtlasWidth = 1920;
        types::u32 _textureAtlasHeight = 1080;
        types::u32 _textureAtlasDepth = 16;
        types::u32 _vertexBufferSize = 65536;
        types::u32 _indexBufferSize = 65536;
        types::u32 _maxRenderOpaqueInstanceCount = 65536;
        types::u32 _maxRenderTransparentInstanceCount = 65536;
        types::u32 _maxRenderTextInstanceCount = 65536;
        types::u32 _maxMaterialCount = 65536;
        types::u32 _maxLightCount = 65536;
        types::u32 _maxTextureAtlasTextureCount = 65536;
        types::u32 _maxGameObjectCount = 65536;
        types::u32 _maxPhysicsSceneCount = 4;
        types::u32 _maxPhysicsSubstanceCount = 256;
        types::u32 _maxPhysicsActorCount = 65536;
        types::u32 _maxPhysicsControllerCount = 4;
        types::u32 _maxSoundCount = 65536;
        types::u32 _maxTextureCount = 65536;
        types::u32 _maxVectorElementCount = 65536;
    };

    class iApplication : public iObject
    {
        REALWARE_CLASS(iApplication)

    public:
        enum class eMouseButton
        {
            LEFT,
            RIGHT,
            MIDDLE
        };

        explicit iApplication(cContext* context);
        ~iApplication();

        virtual void Setup() = 0;
        virtual void Stop() = 0;

        void Run();

        inline types::boolean GetRunState() const { return glfwWindowShouldClose((GLFWwindow*)_window); }
        inline iRenderContext* GetRenderContext() const { return _renderContext; }
        inline iSoundContext* GetSoundContext() const { return _soundContext; }
        inline mCamera* GetCameraManager() const { return _camera; }
        inline mTexture* GetTextureManager() const { return _texture; }
        inline mRender* GetRenderManager() const { return _render; }
        inline mFont* GetFontManager() const { return _font; }
        inline mSound* GetSoundManager() const { return _sound; }
        inline mFileSystem* GetFileSystemManager() const { return _fileSystem; }
        inline mPhysics* GetPhysicsManager() const { return _physics; }
        inline mGameObject* GetGameObjectManager() const { return _gameObject; }
        inline mEvent* GetEventManager() const { return _event; }
        inline mThread* GetThreadManager() const { return _thread; }
        inline void* GetWindow() const { return _window; }
        inline glm::vec2 GetWindowSize() const { return glm::vec2(_desc._windowDesc._width, _desc._windowDesc._height); }
        inline const std::string& GetWindowTitle() const { return _desc._windowDesc._title; }
        inline const HWND& GetWindowHWND() const { return glfwGetWin32Window((GLFWwindow*)_window); }
        types::f32 GetDeltaTime() const { return _deltaTime; }
        sApplicationDescriptor* GetDesc() const { return &_desc; }

    protected:
        mutable sApplicationDescriptor _desc = {};
        void* _window = nullptr;
        iRenderContext* _renderContext = nullptr;
        iSoundContext* _soundContext = nullptr;
        mCamera* _camera = nullptr;
        mRender* _render = nullptr;
        mTexture* _texture = nullptr;
        mFont* _font = nullptr;
        mSound* _sound = nullptr;
        mFileSystem* _fileSystem = nullptr;
        mPhysics* _physics = nullptr;
        mGameObject* _gameObject = nullptr;
        mEvent* _event = nullptr;
        mThread* _thread = nullptr;
        types::f32 _deltaTime = 0.0;
        std::chrono::steady_clock::time_point _timepointLast;
        types::boolean _isFocused = types::K_FALSE;
        glm::vec2 _cursorPosition = glm::vec2(0.0f);
        std::shared_ptr<cEngine> _engine;
    };
}