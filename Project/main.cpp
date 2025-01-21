/// engine
#include "Engine/GameFramework/GameFramework.h"

/// framework config
#include "GameLoop/GameLoop.h"
#include "Scene/Factory/SceneFactory.h"

#include "Engine/Graphics/Framework/RenderingFramework.h"
#include "Engine/Graphics/Renderer/Primitive/Line2DRenderer.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName   = L"TwoEngine",
		.windowSize   = Vec2(1280, 720),
		.gameLoop     = new GameLoop()
	});

	{
		std::unique_ptr<RenderingFramework> renderingFramework = std::make_unique<RenderingFramework>();
		renderingFramework->Initialize(gameFramework->GetDxManager());

		renderingFramework->GenerateRenderer<Line2DRenderer>();

	}


	gameFramework->Run();

	gameFramework->Finalize();

	return 0;
}