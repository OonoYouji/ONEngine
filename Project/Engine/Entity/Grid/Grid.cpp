#include "Grid.h"

/// engine
#include "Engine/Component/RendererComponents/Primitive/Line3DRenderer.h"

void Grid::Initialize() {

	Line3DRenderer* line3DRenderer = AddComponent<Line3DRenderer>();

	Vector4 color        = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
	int     maxGridNum   = 64;
	float   gridInterval = 2.0f;
	float   lineLength   = 64.0f; 

	/// X軸
	for (int i = -maxGridNum / 2; i <= maxGridNum / 2; i++) {
		if (i == 0) continue;

		line3DRenderer->SetLine(
			Vec3(static_cast<float>(i) * gridInterval, 0, -lineLength),
			Vec3(static_cast<float>(i) * gridInterval, 0, +lineLength),
			color
		);
	}

	/// Z軸
	for (int i = -maxGridNum / 2; i <= maxGridNum / 2; i++) {
		if (i == 0) continue;

		line3DRenderer->SetLine(
			Vec3(-lineLength, 0, static_cast<float>(i) * gridInterval),
			Vec3(+lineLength, 0, static_cast<float>(i) * gridInterval),
			color
		);
	}

	/// 中央だけ色を変える
	color = Vector4(0.75f, 0.0f, 0.0f, 1.0f);
	line3DRenderer->SetLine(
		Vec3(-lineLength, 0, 0),
		Vec3(+lineLength, 0, 0),
		color
	);

	color = Vector4(0.0f, 0.0f, 0.75f, 1.0f);
	line3DRenderer->SetLine(
		Vec3(0, 0, -lineLength),
		Vec3(0, 0, +lineLength),
		color
	);

}

void Grid::Update() {}