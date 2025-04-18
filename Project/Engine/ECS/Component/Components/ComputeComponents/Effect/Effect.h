#pragma once

/// std
#include <vector>
#include <string>
//#include <memory>
#include <optional>

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

	enum class EmitType {
		Time,     ///< 時間で指定する場合
		Distance, ///< 距離で指定する場合
	};

	/// /////////////////////////////////////////////  
	/// @brief Effectの一要素  
	/// /////////////////////////////////////////////  
	struct Element final {
		Transform transform; /// 座標(SRT)  
		Vector4   color;     /// RGBA 0.0f ~ 1.0f  
		float     lifeTime;  /// エフェクトの寿命  
		Vector3   velocity;  /// エフェクトの移動速度  
	};

	/// /////////////////////////////////////////////
	/// 出現するまでの距離を指定する場合のデータ
	/// /////////////////////////////////////////////
	struct DistanceEmitData final {
		Vec3 currentPosition;
		Vec3 nextPosition;
		float moveLength;
		float emitDistance;
		float emitInterval;
	};

	/// /////////////////////////////////////////////
	/// 出現するまでの時間を指定する場合のデータ
	/// /////////////////////////////////////////////
	struct TimeEmitData final {
		float emitTime;
		float emitInterval;
	};

public:
	/// ===================================================  
	/// public : methods  
	/// ===================================================  
	Effect() = default;
	~Effect() = default;

	/// @brief 新しい要素の作成
	/// @param _color 
	void CreateElement(const Vector3& _position, const Vector4& _color = Vector4::kWhite);
	void CreateElement(const Vector3& _position, const Vector3& _velocity, const Vector4& _color);

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
	std::string texturePath_;
	std::vector<Element> elements_;

	float lifeLeftTime_;
	float startSpeed_;

	EmitType emitType_;
	DistanceEmitData distanceEmitData_;
	TimeEmitData timeEmitData_;

	size_t emitInstanceCount_; /// emitごとに生成するインスタンス数

public:
	/// ===================================================  
	/// public : accessors  
	/// ===================================================  

	/// @brief メッシュパスを設定  
	/// @param _path メッシュパス  
	void SetMeshPath(const std::string& _path) { meshPath_ = _path; }

	/// @brief テクスチャパスを設定  
	/// @param _path テクスチャパス  
	void SetTexturePath(const std::string& _path) { texturePath_ = _path; }

	/// @brief 最大エフェクト数を設定  
	/// @param _maxCount 最大エフェクト数  
	void SetMaxEffectCount(size_t _maxCount) {
		maxEffectCount_ = _maxCount;
		elements_.reserve(maxEffectCount_);
	}

	/// @brief 距離でのエミットタイプを設定  
	/// @param _interval エミット間隔  
	/// @param _emitInstanceCount エミットごとのインスタンス数  
	void SetEmitTypeDistance(float _interval, size_t _emitInstanceCount);

	/// @brief 時間でのエミットタイプを設定  
	/// @param _data 時間エミットデータ  
	void SetEmitType(const TimeEmitData& _data);

	/// @brief 開始速度を設定  
	/// @param _speed 開始速度  
	void SetStartSpeed(float _speed) { startSpeed_ = _speed; }

	/// @brief 残り寿命を設定  
	/// @param _time 残り寿命  
	void SetLifeLeftTime(float _time) { lifeLeftTime_ = _time; }



	/// @brief メッシュパスを取得
	/// @return メッシュパス
	const std::string& GetMeshPath() const { return meshPath_; }

	const std::string& GetTexturePath() const { return texturePath_; }

	/// @brief エフェクト要素を取得
	/// @return エフェクト要素のリスト
	const std::vector<Element>& GetElements() const { return elements_; }
};

