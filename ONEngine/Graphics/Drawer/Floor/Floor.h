#pragma once

#include <memory>

#include <PipelineState.h>
#include <Transform.h>
#include <Material.h>
#include <Mesh.h>

#include <Vector2.h>

using namespace Microsoft::WRL;


/// ===================================================
/// デバッグ用のグリッド描画
/// ===================================================
class Floor final {
public:

	Floor();
	~Floor();

	void Initialize();

	void Draw();

private:

	std::unique_ptr<PipelineState> pipeline_ = nullptr;
	PipelineState::Shader shader_;
	Transform transform_;
	Material material_;
	Mesh mesh_;

	ComPtr<ID3D12Resource> uvTransformBuffer_ = nullptr;
	Mat4* matUvTransform_ = nullptr;

	Vec2 scale_ = Vec2::kOne;
	float rotate_ = 0.0f;
	Vec2 translate_{};
	

};