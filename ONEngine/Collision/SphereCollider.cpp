#include "SphereCollider.h"

#include <ModelManager.h>
#include <BaseGameObject.h>


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

	Vec3 max = {};
	for(auto& vertex : vertices) {
		if(max.Len() < vertex.Len()) {
			max = vertex;
		}
	}

	sphere_ = ModelManager::Load("Sphere");
	radius_ = max.Len();
	
	transform_.Initialize();
	transform_.scale = { radius_,radius_,radius_ };
	transform_.UpdateMatrix();


}

void SphereCollider::Update() {
	transform_.position = gameObject_->GetPosition();
	transform_.UpdateMatrix();
}

void SphereCollider::Draw() {
	sphere_->Draw(&transform_, kWireFrame);
}

