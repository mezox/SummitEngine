#include <Engine/Engine.h>

#include <iostream>

int main()
{
	Engine::EngineServiceLocator::Provide(Engine::CreateEngineService());

	Engine::EngineServiceLocator::Service()->Initialize();

	while (1)
	{
		Engine::EngineServiceLocator::Service()->StartFrame();
		Engine::EngineServiceLocator::Service()->Update();
		Engine::EngineServiceLocator::Service()->EndFrame();
	}

	Engine::EngineServiceLocator::Provide(nullptr);

	return EXIT_SUCCESS;
}