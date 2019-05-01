#pragma once

#include <Engine/Application.h>
#include <Engine/Window.h>
#include <Renderer/Renderer.h>
#include <Renderer/Resources/Buffer.h>
#include <Renderer/Resources/Texture.h>
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
        
        void SetupRenderPass();
        
        void PushToEngine(Summit::SummitEngine& engine);
        void PopFromEngine(Summit::SummitEngine& engine);
        
        void UpdateCamera();
        
        void OnEarlyUpdate(const Summit::FrameData& data) override;
        void OnUpdate(const Summit::FrameData& data) override;
        void OnLateUpdate(const Summit::FrameData& data) override;
        
        // Rendering
        void OnDepthPrePass();
        void OnEarlyRender();
        void OnRender();
        
    private:
        void OnMouseEvent(Core::MouseEvent& event);
        
        void PrepareSponza();
        void PrepareCube();
        void PrepareChalet();
        
    private:
        Renderer::Pipeline pipeline;
        Renderer::Pipeline depthPrePassPipeline;
        Renderer::Pipeline mQuadPipeline;
        
        Renderer::RenderPass mAdvancedRenderPass;
        
        std::unique_ptr<Application::Window> mWindow;
        
        std::unique_ptr<Renderer::Object3d> mObject;
        std::unique_ptr<Renderer::Object3d> mQuad;
        std::unique_ptr<Renderer::Texture> mTexture;
        
        Renderer::Camera mCamera;
        
        Renderer::Buffer mUniformBuffer;
        Summit::SummitEngine* mEngine{ nullptr };
    };
}
