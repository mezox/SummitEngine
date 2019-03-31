#pragma once

#include <Engine/EngineBase.h>
#include <Event/Signal.h>

namespace Summit
{
    /*!
     * @brief Single render frame data.
     */
    struct ENGINE_API FrameData
    {
        /*!
         * @brief Time since last frame.
         */
        float deltaTime{ 0.0f };
    };
    
    class ENGINE_API SummitEngine
    {
    public:
        SummitEngine();
        ~SummitEngine() {}
        
        void Initialize();
        void StartFrame();
        void Update();
        void EndFrame();
        void DeInitialize();
        
        void Run();
        
    public:
        sigslot::signal<const FrameData&> EarlyUpdate;
        sigslot::signal<const FrameData&> Updatee;
        sigslot::signal<const FrameData&> LateUpdate;
        sigslot::signal<const FrameData&> Render;
        sigslot::signal<const FrameData&> UIRender;
    };

    ENGINE_API std::shared_ptr<SummitEngine> CreateEngineService();

	class ENGINE_API EngineServiceLocator
	{
	public:
		static void Provide(std::shared_ptr<SummitEngine> service)
		{
			mService = std::move(service);
		}

		static std::shared_ptr<SummitEngine> Service()
		{
			return mService;
		}

		static bool Available()
		{
			return !mService;
		}

	private:
		static std::shared_ptr<SummitEngine> mService;
	};
}
