#pragma once

#include <memory>

#include <PipelineState.h>


/// =========================================================
/// パーティクル
/// =========================================================
class Particle final {
public:

	Particle() {}
	~Particle() {}


	/// =========================================================
	/// public : static methods
	/// =========================================================

	/// <summary>
	/// staticメンバの初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// staticメンバの終了
	/// </summary>
	static void StaticFinalize();



private:

	/// =========================================================
	/// private : static objects
	/// =========================================================

	static std::unique_ptr<PipelineState> sPipeline_;
	static PipelineState::Shader sShader_;



};