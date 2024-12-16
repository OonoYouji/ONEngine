#include "GameRenderingFramework.h"

#include "../Renderers/ModelRenderer.h"


void GameRenderingFramework::Initialize() {

	AddRenderer("Model", std::unique_ptr<ModelRenderer>());


}

void GameRenderingFramework::Update() {

}
