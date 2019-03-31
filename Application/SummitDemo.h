#pragma once

#include <Engine/Application.h>
#include <Renderer/Renderer.h>
#include <Math/Matrix4.h>

namespace Summit
{
    class SummitEngine;
}

namespace Application
{
    class Window;
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
        Renderer::Object3D triangle;
        Renderer::Pipeline pipeline;
        Application::Window* window{ nullptr };
        
        Matrix4 mModel;
        Matrix4 mView;
        Matrix4 mProjection;
    };
}
