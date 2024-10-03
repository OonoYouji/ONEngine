#include "Scene_Game.h"


#include <ImGuiManager/ImGuiManager.h>

#include "GameCamera.h"
#include "Player/Player.h"
#include "Particle/ParticleSystem.h"
#include <Component/MeshRenderer/MeshRenderer.h>

#include <Input.h>


class Demo : public BaseGameObject {
	Vec3 velocity;
	Quaternion rotateX;
	Quaternion rotateY;
	float rotateXAngle;
	float rotateYAngle;
	Transform centerTransform;
public:
	Demo() { CreateTag(this); }
	~Demo() {}
	void Initialize() override {
		pTranform_->rotateOrder = QUATERNION;
		pTranform_->quaternion = {1,0,0,0};
		auto mr = AddComponent<MeshRenderer>();
		mr->SetModel("TestObject");
		mr->SetMaterial("uvChecker");
		centerTransform.Initialize();
		centerTransform.rotateOrder = QUATERNION;
		SetParent(&centerTransform);
	}
	void Update() override {
		velocity = {
				static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
				static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
				0.0f
		};

		rotateXAngle += velocity.y * 0.01f;
		rotateYAngle += velocity.x * 0.01f;

		rotateX = MakeAxisRotate({ 1.0f, 0.0f, 0.0f }, velocity.y * 0.5f);
		rotateY = MakeAxisRotate({ 0.0f, 1.0f, 0.0f }, velocity.x * 0.5f);
		centerTransform.quaternion *= rotateX * rotateY;
		centerTransform.Update();
	}

	void Debug() override {
		if(ImGui::TreeNodeEx("debug")) {

			ImGui::DragFloat3("velocity", &velocity.x, 0.0f);
			ImGui::DragFloat4("rotateX", &rotateX.x, 0.0f);
			ImGui::DragFloat4("rotateY", &rotateY.x, 0.0f);

			ImGui::Separator();

			ImGui::DragFloat4("parent quaternion", &centerTransform.quaternion.x, 0.0f);
			ImGui::DragFloat4("this   quaternion", &pTranform_->quaternion.x, 0.0f);

			ImGui::TreePop();
		}
	}

	Quaternion MakeAxisRotate(const Vec3& axis, float angle) {
		float halfAngle = angle * 0.5f;
		float sinHalfAngle = sin(halfAngle);

		Vector3 normalizedAxis = axis.Normalize();

		float w = cos(halfAngle);
		float x = normalizedAxis.x * sinHalfAngle;
		float y = normalizedAxis.y * sinHalfAngle;
		float z = normalizedAxis.z * sinHalfAngle;

		return Quaternion(x, y, z, w);
	}
};


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	//Player* p1 = new Player;
	//p1->Initialize();

	Demo* demo = new Demo;
	demo->Initialize();

	//p1->SetParent(demo->GetTransform());
	
	mainCamera_->SetPosition({ 0.0f, 0.0f, -6.49f * 4 });
	mainCamera_->SetRotate({ 0.066f, -0.258f, 0.0f });
	mainCamera_->UpdateMatrix();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

}


