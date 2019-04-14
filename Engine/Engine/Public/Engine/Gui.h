#pragma once

#include <Renderer/Image.h>
#include <Renderer/Resources/Buffer.h>
#include <Renderer/Renderer.h>
#include <Renderer/VertexBuffer.h>

#include <Event/Event.h>

class IMouseEventHandler
{
public:
    ~IMouseEventHandler() = default;
    
    virtual void OnMouseEvent(Core::MouseEvent& event) = 0;
};

namespace Summit
{
    struct FrameData;
}

namespace Renderer
{
    class View;
}

namespace Summit::UI
{
    using VertexBufferType = Renderer::VertexBufferPTCI<Vector2f, Vector2f, uint32_t, uint16_t>;
    
    class Gui : public IMouseEventHandler
    {
    public:
        Gui(Renderer::View& parent);
        ~Gui();
        
        void StartFrame(const FrameData& fd);
        void FinishFrame();
        
    private:
        void OnMouseEvent(Core::MouseEvent& event) override;
        
    public:
        std::unique_ptr<Renderer::Image> mFontTexture;
        Renderer::Buffer mUniformBuffer;
        Renderer::Pipeline mGuiPipeline;
        
        VertexBufferType mVertexBuffer;
        
        void* mGuiContext{ nullptr };
        
    protected:
        sigslot::connection mMouseEventConnection;
    };
}
