#include "PlayerMoveLocus.h"

/// std
#include <format>

/// engine
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "FrameManager/Time.h"
#include "Math/Easing.h"

/// user
#include "Objects/Player/Player.h"

PlayerMoveLocus::PlayerMoveLocus(Player* _player)
	: pPlayer_(_player) {
	CreateTag(this);
}

PlayerMoveLocus::~PlayerMoveLocus() {}

void PlayerMoveLocus::Initialize() {

	renderer_ = AddComponent<MeshInstancingRenderer>(16);
	renderer_->SetModel("Player");
	renderer_->CreateMaterial("white2x2");
	Material* material = renderer_->GetMaterial();
	material->SetColor({ 1.0f, 1.0f, 1.0f, 0.2f });

	transformWithLifeTimes_.reserve(16);
	transformWithLifeTimes_.clear();

	durationTime_       = 0.01f;
	timeSubScalerValue_ = 1.0f;
}

void PlayerMoveLocus::Update() {


	/// 要素の更新
	size_t i = 0;
	for(auto itr = transformWithLifeTimes_.begin(); itr != transformWithLifeTimes_.end(); ) {

		Transform& transform = (*itr).second;
		float&     lifeTime  = (*itr).first;


		transform.scale = Vec3::kOne * (lifeTime);
		transform.scale = Vec3::Lerp(Vec3::kOne, {}, Ease::Out::Quart(1.0f - lifeTime));

		transform.Update();

		/// life timeの更新、0.0f以下になったら削除
		lifeTime -= timeSubScalerValue_  * Time::DeltaTime();
		if (lifeTime < 0.0f) {
			itr = transformWithLifeTimes_.erase(itr);
		} else {
			++itr;
		}

		i++;
	}

	renderer_->ResetTransformArray();
	for(auto& transformWithLifeTime : transformWithLifeTimes_) {
		renderer_->AddTransform(&transformWithLifeTime.second);
	}




	
	/// Playerから速度を取得、速度が0.1f以下の場合はreturn
	const Vec3& velocity = pPlayer_->GetVelocity();
	if (velocity.Len() < 0.1f) {
		return;
	}

	currentTime_ += Time::DeltaTime();
	if (currentTime_ > durationTime_) {
		if (transformWithLifeTimes_.size() >= 16) {
			transformWithLifeTimes_.erase(transformWithLifeTimes_.begin());
		}

		currentTime_ = 0.0f;

		Transform transform;
		transform.position = Mat4::Transform({}, pPlayer_->GetMesh()->GetMatTransform());
		transform.rotate   = pPlayer_->GetMesh()->GetRotate();
		transform.Update();

		transformWithLifeTimes_.push_back(std::make_pair(1.0f, std::move(transform)));
	}

}

void PlayerMoveLocus::Debug() {

	ImGui::DragFloat("duration time", &durationTime_, 0.001f);
	ImGui::DragFloat("time sub scaler value", &timeSubScalerValue_, 0.001f);

	ImGui::Separator();

	for (auto& transformWithLifeTime : transformWithLifeTimes_) {
		Transform&          transform = transformWithLifeTime.second;
		const std::string&& id        = std::format("##{:p}", reinterpret_cast<void*>(&transform));

		ImGui::DragFloat3(("position" + id).c_str(), &transform.position.x, 0.01f);
		ImGui::DragFloat3(("rotate" + id).c_str(),   &transform.rotate.x, 0.01f);
		ImGui::DragFloat3(("scale" + id).c_str(),    &transform.scale.x, 0.01f);

		ImGui::Spacing();
	}

}

