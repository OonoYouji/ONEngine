#include "BoxCollider.h"

#include "GraphicManager/ModelManager/ModelManager.h"
#include "GameObjectManager/GameObjectManager.h"


void BoxCollider::Initialize() {
	colliderType_ = COLLIDER_TYPE_BOX;
}

void BoxCollider::Update() {

	UpdateMatrix();
	CalculationOrientations();

	transform_->scale = size_;
	obbMatTransform_ = Mat4::MakeAffine(Vec3::kOne, transform_->rotate, transform_->position);
	obbMatTransform_ *= GetOwner()->GetMatTransform();
}

void BoxCollider::Draw() {
#ifdef _DEBUG
	if(isDrawCollider_) {
		cube_->Draw(transform_.get(), kWireFrame);
	}
#endif // _DEBUG
}



void BoxCollider::CreateCollider(const Model* model) {

	/// positionのみをlistに集める
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

	/// positionのlistから最小と最大の頂点を探す
	Vec3 min, max;
	max.x = Vec3::MaxDotVector(Vec3(1, 0, 0), vertices).x;
	max.y = Vec3::MaxDotVector(Vec3(0, 1, 0), vertices).y;
	max.z = Vec3::MaxDotVector(Vec3(0, 0, 1), vertices).z;

	min.x = Vec3::MinDotVector(Vec3(1, 0, 0), vertices).x;
	min.y = Vec3::MinDotVector(Vec3(0, 1, 0), vertices).y;
	min.z = Vec3::MinDotVector(Vec3(0, 0, 1), vertices).z;

	size_ = (max - min) / 2.0f;

	/// 各軸への法線
	orientatinos_[0] = { 1, 0, 0 };
	orientatinos_[1] = { 0, 1, 0 };
	orientatinos_[2] = { 0, 0, 1 };

	/// 当たり判定を描画する用のモデル
	cube_ = ModelManager::CreateCube();

	/// transformを初期化
	transform_.reset(new Transform);
	transform_->Initialize();
	transform_->position = Vec3::Lerp(max, min, 0.5f);
	transform_->scale = size_;
	transform_->UpdateMatrix();
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
		{+1.0f, -1.0f, +1.0f}
	};

	for(auto& v : result) {
		v = Mat4::Transform(v, transform_->matTransform);
	}

	return result;
}

void BoxCollider::SetSize(const Vec3& _size) {
	size_ = _size;
}

void BoxCollider::CalculationOrientations() {
	orientatinos_[0] = Mat4::TransformNormal({ 1.0f, 0.0f, 0.0f }, transform_->matTransform);
	orientatinos_[1] = Mat4::TransformNormal({ 0.0f, 1.0f, 0.0f }, transform_->matTransform);
	orientatinos_[2] = Mat4::TransformNormal({ 0.0f, 0.0f, 1.0f }, transform_->matTransform);
}
