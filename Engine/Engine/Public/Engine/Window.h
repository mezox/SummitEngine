#pragma once

#include "WindowEvent.h"

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

#include "EngineBase.h"

#include <Event/Signal.h>
#include <Renderer/View.h> // TODO: Fwdcl

namespace Application
{
    class ENGINE_API WindowSignalsBase
    {
    public:
        sigslot::signal<const uint16_t, const uint16_t> WindowResized;
        sigslot::signal<const uint16_t, const uint16_t> WindowMoved;
    };
    
    using HandleId = uint32_t;
    
    class ENGINE_API Window : public WindowSignalsBase
    {
        class NativeWindow;
        
    public:
        Window(const std::string& title, const uint16_t width, const uint16_t height);
        virtual ~Window();
        
        void SetTitle(const std::string& title);
        HandleId CreateView(uint16_t w, uint16_t h, uint16_t x, uint16_t y);
        
        Renderer::View* GetView() const { return mView.get(); }
            
    private:
        uint16_t mWidth{ 1920 };
        uint16_t mHeight{ 1080 };
        
        std::unique_ptr<NativeWindow> mNativeWindow;
        std::unique_ptr<Renderer::View> mView;
    };
}
