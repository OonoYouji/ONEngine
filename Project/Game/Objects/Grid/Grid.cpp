#include "Grid.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


Grid::Grid() {
	CreateTag(this);
}

Grid::~Grid() {}

void Grid::Initialize() {

	renderer_ = AddComponent<MeshRenderer>();
	renderer_->SetModel("Grid");
	renderer_->SetMaterial("GridTile.png");
	renderer_->SetUVScale({ 30, 30 });

}

void Grid::Update() {

}

