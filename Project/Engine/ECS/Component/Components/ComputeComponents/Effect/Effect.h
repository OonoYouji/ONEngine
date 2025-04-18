#pragma once

/// std
#include <vector>
#include <string>

/// engine
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "../../Interface/IComponent.h"
#include "../Transform/Transform.h"

/// ///////////////////////////////////////////////////
/// Effectクラス
/// ///////////////////////////////////////////////////
class Effect : public IComponent {
	friend class EffectUpdateSystem; ///< @brief EffectUpdateSystemからアクセスを許可  
public:
	/// ============================================  
	/// public : sub class  
	/// ============================================  

	/// /////////////////////////////////////////////  
	/// @brief Effectの一要素  
	/// /////////////////////////////////////////////  
	struct Element final {
		Transform transform; /// 座標(SRT)  
		Vector4   color;     /// RGBA 0.0f ~ 1.0f  
		float     lifeTime;  /// エフェクトの寿命  
		Vector3   velocity;  /// エフェクトの移動速度  
	};

public:
	/// ===================================================  
	/// public : methods  
	/// ===================================================  
	Effect() = default;
	~Effect() = default;

	/// @brief 新しい要素の作成
	/// @param _color 
	void CreateElement(const Vector4& _color = Vector4::kWhite);
	void CreateElement(const Vector3& _velocity, const Vector3& _color);

	/// @brief Elementを削除する
	/// @param _index 削除する要素のインデックス
	void RemoveElement(size_t _index) {
		if (_index < elements_.size()) {
			elements_.erase(elements_.begin() + _index);
		}
	}

private:
	/// ===================================================  
	/// private : objects  
	/// ===================================================  

	size_t maxEffectCount_ = 1000;
	std::string meshPath_;
	std::vector<Element> elements_;

public:
	/// ===================================================  
	/// public : accessors  
	/// ===================================================  

	/// @brief メッシュパスを設定  
	/// @param _path メッシュパス  
	void SetMeshPath(const std::string& _path) { meshPath_ = _path; }

	/// @brief 最大エフェクト数を設定  
	/// @param _maxCount 最大エフェクト数  
	void SetMaxEffectCount(size_t _maxCount) {
		maxEffectCount_ = _maxCount;
		elements_.reserve(maxEffectCount_);
	}



	/// @brief メッシュパスを取得
	/// @return メッシュパス
	const std::string& GetMeshPath() const { return meshPath_; }

	/// @brief エフェクト要素を取得
	/// @return エフェクト要素のリスト
	const std::vector<Element>& GetElements() const { return elements_; }
};

	