/// engine
#include "Engine/GameFramework/GameFramework.h"

/// game loop
#include "GameLoop/GameLoop.h"

#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	{	/// shader compile test

		Shader shader;
		ShaderCompiler compiler;

		compiler.Initialize();
		shader.Initialize(&compiler);	

		shader.CompileShader(L"./Resources/Shader/Vertex.vs.hlsl", L"vs_6_0", Shader::Type::vs);

	}

	std::unique_ptr<GameFramework> gameFramework = std::make_unique<GameFramework>();
	gameFramework->Initialize(GameFrameworkConfig{
		.windowName	= L"TwoEngine",
		.windowSize = Vec2(1280, 720),
		.gameLoop   = new GameLoop()
	});

	gameFramework->Run();

	gameFramework->Finalize();

	return 0;
}