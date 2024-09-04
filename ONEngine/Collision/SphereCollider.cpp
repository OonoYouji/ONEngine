#include "SphereCollider.h"

#include <ModelManager.h>


void SphereCollider::Initialize(BaseGameObject* gameObject, Model* model) {
	BaseCollider::Initialize(gameObject, model);

	const std::vector<Mesh::VertexData>& vertexPosNormalUvs = model->GetMeshes().front().GetVertices();
	std::vector<Vec3> vertices;
	for(auto& vertexPosNormalUv : vertexPosNormalUvs) {
		Vec3 position = {
			vertexPosNormalUv.position.x,
			vertexPosNormalUv.position.y,
			vertexPosNormalUv.position.z
		};

		vertices.push_back(position);
	}

	Vec3 min, max;
	max.x = Vec3::MaxDotVector(Vec3(1, 0, 0), vertices).x;
	max.y = Vec3::MaxDotVector(Vec3(0, 1, 0), vertices).y;
	max.z = Vec3::MaxDotVector(Vec3(0, 0, 1), vertices).z;

	min.x = Vec3::MinDotVector(Vec3(1, 0, 0), vertices).x;
	min.y = Vec3::MinDotVector(Vec3(0, 1, 0), vertices).y;
	min.z = Vec3::MinDotVector(Vec3(0, 0, 1), vertices).z;

	Vec3 diff = max - min;

	sphere_ = ModelManager::Load("Sphere");

	transform_.Initialize();
	transform_.position = Vec3::Lerp(max, min, 0.5f);
	transform_.scale = diff - transform_.position;
	radius_ = transform_.scale.x;

	UpdateMatrix();
}

void SphereCollider::Update() {
	UpdateMatrix();
}

void SphereCollider::Draw() {
	sphere_->Draw(&transform_, kWireFrame);
}

