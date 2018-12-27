#pragma once

#include <Engine/Engine.h>
#include "Window.h"

namespace Engine
{
	class SummitEngine : public IEngine
	{
	public:
		SummitEngine();

        virtual void Initialize() override;
		virtual void StartFrame() override;
		virtual void Update() override;
		virtual void EndFrame() override;
        virtual void DeInitialize() override;
        
    private:
        std::unique_ptr<App::Window> mWindow;
        std::unique_ptr<App::Window> mSecondWindow;
	};
}
