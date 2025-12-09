#pragma once

/// std
#include <vector>
#include <string>
#include <functional>

/// engine
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Color.h"
#include "../../Interface/IComponent.h"
#include "../Transform/Transform.h"
#include "EmitShape/EffectEmitShape.h"
#include "MainModule/EffectMainModule.h"

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

	enum class BlendMode {
		Normal,
		Add,
		Sub,
		Multiply,
		Screen,
	};

	/// /////////////////////////////////////////////  
	/// @brief Effectの一要素  
	/// /////////////////////////////////////////////  
	struct Element final {
		Transform transform; /// 座標(SRT)  
		Color   color;     /// RGBA 0.0f ~ 1.0f  
		float     lifeTime;  /// エフェクトの寿命  
		Vector3   velocity;  /// エフェクトの移動速度  
	};

	/// /////////////////////////////////////////////
	/// 出現するまでの距離を指定する場合のデータ
	/// /////////////////////////////////////////////
	struct DistanceEmitData final {
		Vector3 currentPosition;
		Vector3 nextPosition;
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
	Effect();
	~Effect() = default;

	/// @brief 新しい要素の作成
	/// @param _color 
	void CreateElement(const Vector3& _position, const Color& _color = Color::kWhite);
	void CreateElement(const Vector3& _position, const Vector3& _velocity, const Color& _color);
	void CreateElement(const Vector3& _position, const Vector3& _scale, const Vector3& _rotate, const Vector3& _velocity, const Color& _color);

	/// @brief Elementを削除する
	/// @param _index 削除する要素のインデックス
	void RemoveElement(size_t _index);

private:
	/// ===================================================  
	/// private : objects  
	/// ===================================================  

	bool isCreateParticle_; ///!< これがtrueじゃないとパーティクルが出現しない


	size_t maxEffectCount_ = 1000;
	std::string meshPath_;
	std::string texturePath_;
	std::vector<Element> elements_;

	bool useBillboard_ = false; ///< ビルボードを使用するかどうか

	EffectMainModule mainModule_; ///< メインモジュール
	EffectEmitShape emitShape_;  ///< エミット形状

	EmitType emitType_;
	DistanceEmitData distanceEmitData_;
	TimeEmitData timeEmitData_;

	size_t emitInstanceCount_; /// emitごとに生成するインスタンス数

	std::function<void(Element*)> elementUpdateFunc_ = nullptr; ///< エフェクトの更新関数

	BlendMode blendMode_ = BlendMode::Normal; ///< ブレンドモード



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

	void SetMainModule(const EffectMainModule& _module);
	void SetEmitShape(const EffectEmitShape& _shape);

	void SetEmitType(EmitType _type);

	/// @brief 最大エフェクト数を設定  
	/// @param _maxCount 最大エフェクト数  
	void SetMaxEffectCount(size_t _maxCount);

	/// @brief 距離でのエミットタイプを設定  
	/// @param _interval エミット間隔  
	/// @param _emitInstanceCount エミットごとのインスタンス数  
	void SetEmitTypeDistance(float _interval, size_t _emitInstanceCount);
	void SetEmitTypeDistance(const DistanceEmitData& _data);

	/// @brief 時間でのエミットタイプを設定  
	/// @param _data 時間エミットデータ  
	void SetEmitTypeTime(const TimeEmitData& _data, size_t _emitInstanceCount);
	void SetEmitTypeTime(const TimeEmitData& _data);

	void SetEmitInstanceCount(size_t _emitInstanceCount);

	/// @brief 残り寿命を設定  
	/// @param _time 残り寿命  
	void SetLifeLeftTime(float _time);

	/// @brief 要素の更新関数を設定
	/// @param _func 
	void SetElementUpdateFunc(std::function<void(Element*)> _func);

	/// @brief ビルボードの使用を設定
	/// @param _use true: ビルボードを使用する, false: 使用しない
	void SetUseBillboard(bool _use);

	/// @brief particle を出現させるかのフラグ
	/// @param _isCreateParticle true: 出現できる false: 出現できない
	void SetIsCreateParticle(bool _isCreateParticle);

	void SetBlendMode(BlendMode _blendMode);

	void SetStartSize(const Vector3& _size);
	void SetStartSize(const Vector3& _size1, const Vector3& _size2);

	void SetStartRotate(const Vector3& _rotate);
	void SetStartRotate(const Vector3& _rotate1, const Vector3& _rotate2);

	void SetStartColor(const Color& _color);
	void SetStartColor(const Color& _color1, const Color& _color2);

	void SetStartSpeed(float _speed);
	void SetStartSpeed(float _speed1, float _speed2);

	/// @brief sphereのエミット形状を設定
	/// @param _center 中心
	/// @param _radius 半径
	void SetEmitShape(const Vector3& _center, float _radius);

	/// @brief cubeのエミット形状を設定
	/// @param _center 中心 
	/// @param _size cubeのサイズ
	void SetEmitShape(const Vector3& _center, const Vector3& _size);

	/// @brief coneのエミット形状を設定
	/// @param _apex 天辺の位置
	/// @param _angle coneの角度
	/// @param _radius coneの半径
	/// @param _height coneの高さ
	void SetEmitShape(const Vector3& _apex, float _angle, float _radius, float _height);




	bool IsCreateParticle() const;
	size_t GetMaxEffectCount() const;
	const std::string& GetMeshPath() const;
	const std::string& GetTexturePath() const;


	/// @brief エフェクト要素を取得
	/// @return エフェクト要素のリスト
	const std::vector<Element>& GetElements() const;

	BlendMode GetBlendMode() const;

	EffectMainModule* GetMainModule();
	const EffectMainModule& GetMainModule() const;

	EffectEmitShape* GetEmitShape();
	const EffectEmitShape& GetEmitShape() const;

	int GetEmitType() const;

	const DistanceEmitData& GetDistanceEmitData() const;
	const TimeEmitData& GetTimeEmitData() const;
	size_t GetEmitInstanceCount() const;

};


namespace COMP_DEBUG {
	/// @brief Effectのデバッグ関数
	/// @param _effect デバッグするEffect
	void EffectDebug(Effect* _effect);
} 
