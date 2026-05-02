///// engine
//#include "Engine/Core/GameFramework/GameFramework.h"
//#include "Engine/Core/Utility/Tools/Log.h"
//
//
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
//	ONEngine::Console console;
//
//	std::unique_ptr<ONEngine::GameFramework> gameFramework = std::make_unique<ONEngine::GameFramework>();
//	gameFramework->Initialize(ONEngine::GameFrameworkConfig{
//		.windowName = L"TwoEngine",
//		.windowSize = ONEngine::Vector2::HD,
//	});
//
//	gameFramework->Run();
//
//	return 0;
//}


#include <Windows.h>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/MeshRenderer.h"
#include "Engine/ECS/Systems/RenderSystem.h"
#include "Engine/ECS/Components/Camera.h"
#include "Engine/ECS/Components/Light.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Engine::Core::Window window;
	window.Initialize(L"ThreeEngine", Engine::Math::Vector2Int::HD);

	Engine::Graphics::GraphicsEngine& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
	graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

	// ECSの初期化
	Engine::ECS::Registry registry;
	Engine::ECS::RenderSystem renderSystem;

	// テスト用のエンティティ作成
	auto entity = registry.CreateEntity();
	registry.AddComponent<Engine::ECS::Transform>(entity, { .position = { 0.0f, 0.0f, 5.0f } });
	registry.AddComponent<Engine::ECS::MeshRenderer>(entity, { .meshPath = "Assets/Models/cube.obj" });

	// カメラの作成
	auto cameraEntity = registry.CreateEntity();
	registry.AddComponent<Engine::ECS::Transform>(cameraEntity, { .position = { 0.0f, 0.0f, -10.0f } });
	registry.AddComponent<Engine::ECS::Camera>(cameraEntity);

	// ライトの作成
	auto lightEntity = registry.CreateEntity();
	registry.AddComponent<Engine::ECS::DirectionalLight>(lightEntity);

	Engine::Math::Vector4 clearColor = { 0.1f, 0.25f, 0.5f, 1.0f };

	while(true) {
		window.Update();

		if(window.GetIsProcessEnd()) {
			break;
		}

		// ECSの更新
		renderSystem.Update(registry);

		graphicsEngine.BeginFrame();
		graphicsEngine.Clear(clearColor);
		
		// ここで本来はRenderer::GetInstance().GetQueue()を使って描画を行う

		graphicsEngine.EndFrame();

		// キューのクリア
		Engine::Graphics::Renderer::GetInstance().ClearQueue();
	}

	graphicsEngine.Shutdown();
	window.Shutdown();

	return 0;
}