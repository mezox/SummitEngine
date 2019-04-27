#pragma once

#include <Engine/Application.h>
#include <Engine/Window.h>
#include <Renderer/Renderer.h>
#include <Renderer/Resources/Buffer.h>
#include <Renderer/Image.h>
#include <Renderer/Camera.h>
#include <Renderer/Object3D.h>
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
        Renderer::Pipeline pipeline;
        Renderer::Pipeline depthPrePassPipeline;
        
        std::unique_ptr<Application::Window> mWindow;
        
        std::unique_ptr<Renderer::Object3d> mObject;
        std::unique_ptr<Renderer::Image> mTexture;
        Renderer::Framebuffer mDepthPrePassFB;
        
        Renderer::RenderPass mDepthPrePass;
        Renderer::RenderPass mDefaultRenderPass;
        
        Renderer::Camera mCamera;
        
        Renderer::Buffer mUniformBuffer;
        
        Summit::SummitEngine* mEngine{ nullptr };
    };
}
