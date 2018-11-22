#pragma once

#include <memory>

namespace Engine
{
	class IEngine
	{
	public:
		virtual ~IEngine() = default;

        virtual void Initialize() = 0;
        virtual void StartFrame() = 0;
		virtual void Update() = 0;
		virtual void EndFrame() = 0;
        virtual void DeInitialize() = 0;
	};

    std::shared_ptr<IEngine> CreateEngineService();

	class EngineServiceLocator
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