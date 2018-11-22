#pragma once

#include <Engine/Engine.h>

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
	};
}
