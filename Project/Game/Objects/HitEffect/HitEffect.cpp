#include "HitEffect.h"

/// std
#include <numbers>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "FrameManager/Time.h"
#include "Math/Random.h"

/// user
#include "Objects/Camera/GameCamera.h"


HitEffect::HitEffect(GameCamera* _camera)
	: pCamera_(_camera) {
	CreateTag(this);
}

HitEffect::~HitEffect() {}

void HitEffect::Initialize() {

	TextureManager::GetInstance()->Load("circle", "circle.png");

	/// 各parameterの初期化
	/// 連番画像の最大枚数
	frameMaxIndex_ = 5;

	time_ = 0.0f;
	frameDuration_ = 0.01f;


	/// MeshRendererの初期化
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("HitEffect");
	meshRenderer_->SetMaterial("HitEffect-sheet.png");
	meshRenderer_->SetColor({ 1,1,1,1 });
	meshRenderer_->SetIsLighting(false);
	meshRenderer_->SetUVScale({ 1.0f / static_cast<float>(frameMaxIndex_), 1.0f});


	/// MeshInstancingRendererの初期化
	Model*    plane = ModelManager::CreatePlane();
	Material& material = plane->GetMaterials().front();
	material.SetColor({ 1, 1, 1, 0.2f });
	material.SetIsLighting(true);
	material.SetTextureName("circle");

	meshInstancingRenderer_ = AddComponent<MeshInstancingRenderer>(64);
	meshInstancingRenderer_->SetModel(plane);


	/// EffectElementの初期化
	for(size_t i = 0; i < 64; i++) {
		elements_.push_back(EffectElement());
		EffectElement& element = elements_.back();
		
		element.transform.position = {};
		element.transform.rotate.x = std::numbers::pi_v<float> * -0.5f;
		element.velocity = Random::Vec3({ -1.0f, 0.0f, -1.0f }, Vec3::kOne * 1.0f);
		element.velocity = element.velocity.Normalize() * Random::Float(1.0f, 5.0f);
	}

	
	/// MeshInstancingRendererにEffectElementのtransformを追加
	meshInstancingRenderer_->ResetTransformArray();
	for(auto& element : elements_) {
		meshInstancingRenderer_->AddTransform(&element.transform);
	}

}

void HitEffect::Update() {

	/// ビルボード処理、scaleを加算する
	pTransform_->rotate = pCamera_->GetRotate();
	pTransform_->scale += Vec3::kOne * 0.5f;

	/// timeを更新して、アニメーションを再生する
	time_ += Time::DeltaTime();
	if(time_ > frameDuration_) {
		time_ = 0.0f;
		frameIndex_++;
		meshRenderer_->SetUVPosition({ 1.0f / static_cast<float>(frameMaxIndex_)*frameIndex_, 0.0f });
	}


	/// 各エフェクトのエレメントを更新する
	for(auto& element : elements_) {
		element.transform.position += element.velocity * Time::DeltaTime();
		element.transform.Update();
		element.transform.matTransform *= pTransform_->matTransform;
	}


	/// 6フレーム経過したら、このオブジェクトを削除する
	if(frameIndex_ >= frameMaxIndex_) {
		Destory();
	}

}

