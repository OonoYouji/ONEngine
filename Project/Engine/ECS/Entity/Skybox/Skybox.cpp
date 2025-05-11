#include "Skybox.h"

/// std
#include <vector>

/// engine
#include "Engine/ECS/Component/Component.h"

Skybox::Skybox() {}
Skybox::~Skybox() {}

void Skybox::Initialize() {

	CustomMeshRenderer* meshRenderer = AddComponent<CustomMeshRenderer>();

	std::vector<Mesh::VertexData> vertices = {
		/// 右面
		{ { -1.0f, -1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }, Vector3::kLeft },
		{ { 1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }, Vector3::kLeft },
		{ { -1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }, Vector3::kLeft },
		{ { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }, Vector3::kLeft },

		/// 左面
		{ { -1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f }, Vector3::kRight },
		{ { -1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }, Vector3::kRight },
		{ { -1.0f, 1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }, Vector3::kRight },
		{ { -1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }, Vector3::kRight },

		/// 前面
		{ { -1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f }, Vector3::kBack },
		{ { 1.0f, -1.0f, -1.0f, 1.0f }, { 1.0f, 0.0f }, Vector3::kBack },
		{ { -1.0f, 1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }, Vector3::kBack },
		{ { 1.0f, 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }, Vector3::kBack },

		/// 後面
		{ { 1.0f, -1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }, Vector3::kFront },
		{ { -1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }, Vector3::kFront },
		{ { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }, Vector3::kFront },
		{ { -1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }, Vector3::kFront },

		/// 上面
		{ { -1.0f, 1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f }, Vector3::kDown },
		{ { 1.0f, 1.0f, -1.0f, 1.0f }, { 1.0f, 0.0f }, Vector3::kDown },
		{ { -1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }, Vector3::kDown },
		{ { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }, Vector3::kDown },

		/// 下面
		{ { -1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f }, Vector3::kUp },
		{ { 1.0f, -1.0f, -1.0f, 1.0f }, { 1.0f, 0.0f }, Vector3::kUp },
		{ { -1.0f, -1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }, Vector3::kUp },
		{ { 1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }, Vector3::kUp }
	};

	std::vector<uint32_t> indices = {
		/// 右面
		0, 1, 2,
		2, 1, 3,
		/// 左面
		4, 5, 6,
		6, 5, 7,
		/// 前面
		8, 9, 10,
		10, 9, 11,
		/// 後面
		12, 13, 14,
		14, 13, 15,
		/// 上面
		16, 17, 18,
		18, 17, 19,
		/// 下面
		20, 21, 22,
		22, 21, 23
	};


	meshRenderer->SetVertices(vertices);
	meshRenderer->SetIndices(indices);
	meshRenderer->SetIsBufferRecreate(true);
}

void Skybox::Update() {



}
