#pragma once

#include <Engine/Application.h>
#include <Engine/Window.h>
#include <Renderer/Renderer.h>
#include <Renderer/Resources/Buffer.h>
#include <Renderer/Image.h>
#include <Renderer/Camera.h>
#include <Math/Matrix4.h>
#include <Math/Vector2.h>

namespace Summit
{
    class SummitEngine;
}

namespace Demo
{
    class SummitDemo : public Summit::SummitApplication
    {
    public:
        SummitDemo(Summit::SummitEngine& engine);
        virtual ~SummitDemo() = default;
        
        void PushToEngine(Summit::SummitEngine& engine);
        void PopFromEngine(Summit::SummitEngine& engine);
        
        void UpdateCamera();
        
        void OnEarlyUpdate(const Summit::FrameData& data) override;
        void OnUpdate(const Summit::FrameData& data) override;
        void OnLateUpdate(const Summit::FrameData& data) override;
        void OnRender(const Summit::FrameData& data) override;
        void OnUIRender(const Summit::FrameData& data) override;
        
    private:
        void OnMouseEvent(Core::MouseEvent& event);
        
        void PrepareCube();
        void PrepareChalet();
        
    private:
        Renderer::Object3D triangle;
        Renderer::Pipeline pipeline;
        std::unique_ptr<Application::Window> mWindow;
        
        Renderer::Object3D mChalet;
        Renderer::Pipeline mChaletPipeline;
        std::unique_ptr<Renderer::Image> mChaletTexture;
        
        Matrix4 mModel;
        //Matrix4 mView;
        //Matrix4 mProjection;
        
        Renderer::Camera mCamera;
        Vector2<uint16_t> mLastCursorPosition;
        
        Renderer::Buffer mUniformBuffer;
        std::unique_ptr<Renderer::Image> mTexture;
        
        Summit::SummitEngine* mEngine{ nullptr };
    };
}
