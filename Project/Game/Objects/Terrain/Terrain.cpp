#include "Terrain.h"

/// components
#include "ComponentManager/TerrainRenderer/TerrainRenderer.h"


Terrain::Terrain() {
	CreateTag(this);
}

Terrain::~Terrain() {}

void Terrain::Initialize() {
	terranRenderer_ = AddComponent<TerrainRenderer>();

}

void Terrain::Update() {

}

