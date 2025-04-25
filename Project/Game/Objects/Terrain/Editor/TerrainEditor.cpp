#include "TerrainEditor.h"

/// engine
#include "Engine/Core/Utility/Utility.h"

TerrainEditor::TerrainEditor(Terrain* _terrain) : terrain_(_terrain){
	Assert(terrain_ != nullptr, "TerrainEditor::Initialize: terrain_ is null");
}

TerrainEditor::~TerrainEditor() {}



void TerrainEditor::Initialize() {

	Vector2 mousePosition = Input::GetMousePosition();

}

void TerrainEditor::Update() {

}
