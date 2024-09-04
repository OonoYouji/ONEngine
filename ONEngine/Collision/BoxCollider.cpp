#define NOMINMAX
#include "BoxCollider.h"

#include <vector>
#include <cassert>

#include <ModelManager.h>
#include <BaseGameObject.h>

#include "SphereCollider.h"


void BoxCollider::Initialize(BaseGameObject* gameObejct, Model* model) {
	BaseCollider::Initialize(gameObejct, model);

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

	size_ = (max - min) / 2.0f;

	orientatinos_[0] = { 1, 0, 0 };
	orientatinos_[1] = { 0, 1, 0 };
	orientatinos_[2] = { 0, 0, 1 };


	cube_ = ModelManager::CreateCube();

	transform_.Initialize();
	transform_.position = Vec3::Lerp(max, min, 0.5f);
	transform_.scale = size_;

	UpdateMatrix();

}

void BoxCollider::Update() {
	UpdateOrientatinos();
	UpdateMatrix();
	obbMatTransform_ = Mat4::MakeRotate(transform_.rotate) * Mat4::MakeTranslate(transform_.position);
	obbMatTransform_ *= gameObject_->GetMatTransform();
}

void BoxCollider::Draw() {
	if(cube_) {
		cube_->Draw(&transform_, kWireFrame);
	}
}

std::vector<Vec3> BoxCollider::GetVertices() const {
	std::vector<Vec3> result = {
		{-1.0f, +1.0f, -1.0f},
		{+1.0f, +1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		{+1.0f, -1.0f, -1.0f},
		{-1.0f, +1.0f, +1.0f},
		{+1.0f, +1.0f, +1.0f},
		{-1.0f, -1.0f, +1.0f},
		{+1.0f, -1.0f, +1.0f},
	};

	for(auto& v : result) {
		v = Mat4::Transform(v, transform_.matTransform);
	}

	return result;
}

void BoxCollider::UpdateOrientatinos() {
	orientatinos_[0] = Mat4::TransformNormal({ 1.0f, 0.0f, 0.0f }, transform_.matTransform);
	orientatinos_[1] = Mat4::TransformNormal({ 0.0f, 1.0f, 0.0f }, transform_.matTransform);
	orientatinos_[2] = Mat4::TransformNormal({ 0.0f, 0.0f, 1.0f }, transform_.matTransform);
}
