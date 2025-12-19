#include "DissolveMeshRenderer.h"

using namespace ONEngine;

void ComponentDebug::ShowGUI(DissolveMeshRenderer* _dmr) {
	if(!_dmr) {
		return;
	}



}

void ONEngine::from_json(const nlohmann::json& _j, DissolveMeshRenderer& _dmr) {

}

void ONEngine::to_json(nlohmann::json& _j, const DissolveMeshRenderer& _dmr) {
	_j = {
		{ "type", "DissolveMeshRenderer" },
		{ "meshGuid", _dmr.meshGuid_ },
		{ "material", _dmr.material_ },
		{ "dissolveTexture", _dmr.dissolveTexture_ }
	};
}

/// ///////////////////////////////////////////////////
/// ここから DissolveMeshRenderer の定義
/// ///////////////////////////////////////////////////

DissolveMeshRenderer::DissolveMeshRenderer() {}
DissolveMeshRenderer::~DissolveMeshRenderer() {}


