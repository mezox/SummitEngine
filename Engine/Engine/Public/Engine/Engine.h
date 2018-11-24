#pragma once

#include "EngineBase.h"
#include <memory>

namespace Engine
{
	class ENGINE_API IEngine
	{
	public:
		virtual ~IEngine() = default;

        virtual void Initialize() = 0;
        virtual void StartFrame() = 0;
		virtual void Update() = 0;
		virtual void EndFrame() = 0;
        virtual void DeInitialize() = 0;
	};

    ENGINE_API std::shared_ptr<IEngine> CreateEngineService();

	class ENGINE_API EngineServiceLocator
	{
	public:
		static void Provide(std::shared_ptr<IEngine> service)
		{
			mService = std::move(service);
		}

		static std::shared_ptr<IEngine> Service()
		{
			return mService;
		}

		static bool Available()
		{
			return !mService;
		}

	private:
		static std::shared_ptr<IEngine> mService;
	};
}
