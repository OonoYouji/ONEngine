#define NOMINMAX
#include "Tornado.h"

/// std
#include <numbers>
#include <string>

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include <Input/Input.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

/// math
#include "Math/Random.h"

/// objects
#include "Objects/Player/Player.h"


void Tornado::Initialize() {

	const uint32_t kParticleMaxNum = 32u;

	particleDataArray_.resize(kParticleMaxNum);
	ParticleSystem* particleSystem = AddComponent<ParticleSystem>(kParticleMaxNum, "rubble");

	/// パーティクルデータの初期化
	for(auto& data : particleDataArray_) {
		data.maxPosY = Random::Float(1.0f, 10.0f);
		data.radius = Random::Float(1.0f, 2.0f);
		data.speed = Random::Float(5.0f, 10.0f);
		data.time = Random::Float(0.0f, 1.0f);
		data.rotate = Random::Vec3(-Vec3::kOne, Vec3::kOne);
		data.scale = Random::Vec3(Vec3::kOne * 0.1f, Vec3::kOne * 0.5f);
	}

	/// パーティクルの挙動
	particleSystem->SetParticleLifeTime(3.0f);
	particleSystem->SetEmittedParticleCount(2);
	particleSystem->SetParticleRespawnTime(0.6f);
	particleSystem->SetUseBillboard(false);

	particleSystem->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();
		ParticleData& data = particleDataArray_[particle->GetID()];

		data.time += Time::DeltaTime();
		transform->rotate = data.rotate;
		transform->scale = data.scale;

		transform->position = {
			std::cos(data.time * data.speed) * data.radius,
			std::sin(data.time * data.speed) * data.radius,
			particle->GetNormLifeTime() * data.maxPosY
		};

		transform->position = Mat4::Transform(transform->position, matRotate_);
		transform->position += GetPosition();
	});




	/// transform initialize
	pTransform_->rotateOrder = QUATERNION;

	quaternionLocalX_ = Quaternion::MakeFromAxis(Vec3::kRight, -std::numbers::pi_v<float> / 2.0f);
	quaternionLocalY_ = Quaternion::MakeFromAxis(Vec3::kUp, 0.0f);
	/// action param initialize
	eacSpeed_ = 0.7f;
	pTransform_->position.z = 4.2f;
	minScale_ = 0.5f;
	maxScale_ = 0.75f;
	scaleScaler_ = minScale_;


	windArray_.resize(10);
	for(auto& wind : windArray_) {
		wind = new Wind;
		wind->Initialize();
		wind->SetParent(pTransform_);
		wind->SetScale(Vec3::kOne * 0.5f);
	}

	windAnimationDataArray_.resize(10);
	for(auto& data : windAnimationDataArray_) {
		data.speed = Random::Float(1.0f, 4.0f) * 4.0f;
	}

}

void Tornado::Update() {

	if(Input::PressKey(KeyCode::Space) || Input::PressPadButton(PadCode::A)) {

		scaleScaler_ = std::min(scaleScaler_ + (eacSpeed_ * Time::DeltaTime()), maxScale_);
	} else {

		scaleScaler_ = std::max(scaleScaler_ - (eacSpeed_ * Time::DeltaTime()), minScale_);
	}

	localYAngle_  += Time::DeltaTime() * zRotateSpeed_;

	quaternionLocalY_       = Quaternion::MakeFromAxis(Vec3::kUp,    localYAngle_);

	pTransform_->quaternion = quaternionLocalX_;
	pTransform_->scale      = Vec3::kOne * scaleScaler_;

	Transform* parent = GetParent();
	if(parent) {
		matRotate_ = parent->matTransform;
		matRotate_.m[3][0] = 0.0f;
		matRotate_.m[3][1] = 0.0f;
		matRotate_.m[3][2] = 0.0f;
	}

	/// 周りの風を回転させる
	for(size_t i = 0; i < 10; ++i) {
		WindAnimationData& data = windAnimationDataArray_[i];
		Wind* wind = windArray_[i];

		data.time += Time::DeltaTime();
		wind->SetRotateY(data.time * data.speed);
	}
}



void Tornado::Debug() {

	if(ImGui::TreeNodeEx("this param", ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec3 wPos = GetPosition();
		ImGui::DragFloat3("world position", &wPos.x, 0.0f);

		ImGui::Separator();

		ImGui::DragFloat4("quaternion local x", &quaternionLocalX_.x, 0.0f);
		ImGui::DragFloat4("quaternion local y", &quaternionLocalX_.y, 0.0f);

		ImGui::TreePop();
	}

	/// this debug
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {
		
		ImGui::DragFloat("eac speed", &eacSpeed_, 0.01f);
		ImGui::DragFloat("z rotate speed", &zRotateSpeed_, 0.5f);

		ImGui::Separator();

		ImGui::DragFloat("scale scaler", &scaleScaler_, 0.01f);
		ImGui::DragFloat("min scale", &minScale_, 0.01f);
		ImGui::DragFloat("max scale", &maxScale_, 0.01f);
		
		ImGui::Separator();

		ImGui::DragFloat("local y angle", &localYAngle_, 0.1f);
		
		

		ImGui::TreePop();
	}


	/// player debug
	if(ImGui::TreeNodeEx("player", ImGuiTreeNodeFlags_DefaultOpen)) {

		void* pointer = reinterpret_cast<void*>(pPlayer_);
		ImGui::DragInt("address", reinterpret_cast<int*>(&pointer));

		ImGui::TreePop();
	}


	if(ImGui::TreeNodeEx("particle data", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

		for(size_t i = 0; i < particleDataArray_.size(); ++i) {

			ParticleData& data = particleDataArray_[i];

			std::string label = std::string("data") + std::to_string(i);
			if(!ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				continue;
			}

			//ImGui::DragFloat("value",  &data.value,  0.05f);
			ImGui::DragFloat("radius", &data.radius, 0.05f);

			ImGui::TreePop();
		}

		ImGui::EndChild();
		ImGui::TreePop();
	}


	if(ImGui::TreeNodeEx("wind debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::TreePop();
	}


}

void Tornado::SetPlayer(Player* _player) {
	pPlayer_ = _player;
	SetParent(pPlayer_->GetbaseTransform());
	UpdateMatrix();
	
}

/// ===================================================
/// wind
/// ===================================================

int Wind::sInstanceCount_ = 0;

Wind::Wind() {
	id_ = sInstanceCount_++;
	CreateTag(this); 
}

void Wind::ResetInstanceCount() {
	sInstanceCount_ = 0;
}

void Wind::Initialize() {
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();

	std::string modelFilePath = std::string("tornade") + std::to_string(id_ + 1);
	meshRenderer->SetModel(modelFilePath);
}

void Wind::Update() {}
