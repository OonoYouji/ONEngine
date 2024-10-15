#include "SphereCollider.h"

#include "GraphicManager/ModelManager/ModelManager.h"
#include "GameObjectManager/GameObjectManager.h"


void SphereCollider::Initialize() {}

void SphereCollider::Update() {
	transform_->position = GetOwner()->GetPosition();
	transform_->UpdateMatrix();
}

void SphereCollider::Draw() {
	sphere_->Draw(transform_.get(), kWireFrame);
}


void SphereCollider::CreateCollider(const Model* model) {

	/// positionのlistを作成
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

	/// listから最長の頂点を探す
	Vec3 max = {};
	for(auto& vertex : vertices) {
		if(max.Len() < vertex.Len()) {
			max = vertex;
		}
	}

	/// model
	sphere_ = ModelManager::Load("Sphere");

	/// 最長の頂点の長さが半径になる
	radius_ = max.Len();

	/// transformの初期化
	transform_.reset(new Transform);
	transform_->Initialize();
	transform_->scale = { radius_, radius_, radius_ };
	transform_->UpdateMatrix();
}
