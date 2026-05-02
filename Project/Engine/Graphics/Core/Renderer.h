#pragma once

#include <vector>
#include <string>
#include "Engine/Core/Math/Math.h"

namespace Engine::Graphics {

///
/// 描画リクエストのデータ構造
///
struct RenderRequest {
	std::string meshPath;
	Engine::Math::Vector3 position;
	Engine::Math::Vector3 rotation;
	Engine::Math::Vector3 scale;
};

///
/// 光源データ
///
struct DirectionalLightData {
	Engine::Math::Vector3 color;
	float intensity;
	Engine::Math::Vector3 direction;
};

struct PointLightData {
	Engine::Math::Vector3 position;
	Engine::Math::Vector3 color;
	float intensity;
	float radius;
};

///
/// 簡易レンダラークラス
///
class Renderer {
public:
	static Renderer& GetInstance() {
		static Renderer instance;
		return instance;
	}

	void PushRequest(const RenderRequest& request) {
		queue_.push_back(request);
	}

	void AddDirectionalLight(const DirectionalLightData& light) {
		dirLights_.push_back(light);
	}

	void AddPointLight(const PointLightData& light) {
		pointLights_.push_back(light);
	}

	void ClearQueue() {
		queue_.clear();
		dirLights_.clear();
		pointLights_.clear();
	}

	const std::vector<RenderRequest>& GetQueue() const {
		return queue_;
	}

	const std::vector<DirectionalLightData>& GetDirectionalLights() const { return dirLights_; }
	const std::vector<PointLightData>& GetPointLights() const { return pointLights_; }

private:
	std::vector<RenderRequest> queue_;
	std::vector<DirectionalLightData> dirLights_;
	std::vector<PointLightData> pointLights_;
};

} // namespace Engine::Graphics
