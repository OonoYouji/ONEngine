/// engine
#include "Engine/GameFramework/GameFramework.h"

/// framework config
#include "GameLoop/GameLoop.h"
#include "Scene/Factory/SceneFactory.h"


#include "Engine/Graphics/Shader/GraphicsPipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName   = L"TwoEngine",
		.windowSize   = Vec2(1280, 720),
		.gameLoop     = new GameLoop()
	});

	

	{	/// pso create test
		ShaderCompiler compiler;
		compiler.Initialize();

		Shader shader;
		shader.Initialize(&compiler);
		shader.CompileShader(L"Assets/Shader/Line2D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Assets/Shader/Line2D.ps.hlsl", L"vs_6_0", Shader::Type::ps);

		std::unique_ptr<GraphicsPipeline> pso = std::make_unique<GraphicsPipeline>();
		pso->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pso->AddInputElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	}


	gameFramework->Run();

	gameFramework->Finalize();

	return 0;
}