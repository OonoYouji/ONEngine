#include "TerrainRenderer.h"

/// externals
#include <imgui.h>

/// std
#include <format>



TerrainRenderer::TerrainRenderer() {}
TerrainRenderer::~TerrainRenderer() {}

void TerrainRenderer::Initialize() {}

void TerrainRenderer::Update() {}

void TerrainRenderer::Draw() {}

void TerrainRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}
}
