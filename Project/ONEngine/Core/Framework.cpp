#include "Framework.h"

#include "ONEngine.h"

void Framework::Run() {
	Initialize();

	while(ONEngine::GetIsRunning()) {
		ONEngine::Update();
		Update();

		ONEngine::PreDraw();
		Draw();
		ONEngine::PostDraw();
	}

	Finalize();

}
