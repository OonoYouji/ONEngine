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
namespace ONEngine {

/**
 * @class Effect
 * @brief 簡易パーティクル（エフェクト粒子）の生成、寿命管理、ビルボード制御、および放出パラメータを規定するエフェクトコンポーネントクラス
 */
class Effect : public IComponent {
	friend class EffectUpdateSystem; ///< @brief EffectUpdateSystemからアクセスを許可  
public:
	/// ============================================  
	/// public : sub class  
	/// ============================================  

	/**
	 * @enum EmitType
	 * @brief 粒子の放出トリガー判定方式
	 */
	enum class EmitType {
		Time,     ///< 時間経過で指定する場合
		Distance, ///< 移動距離で指定する場合
	};

	/**
	 * @enum BlendMode
	 * @brief GPU描画時のカラーブレンドステート指定
	 */
	enum class BlendMode {
		Normal,   ///< 通常ブレンド（アルファ）
		Add,      ///< 加算合成
		Sub,      ///< 減算合成
		Multiply, ///< 乗算合成
		Screen,   ///< スクリーン合成
	};

	/**
	 * @struct Element
	 * @brief 個々の放出されたエフェクト粒子（要素）が持つ実体パラメータ
	 */
	struct Element final {
		Transform transform; ///< 座標(SRT)  
		Color   color;     ///< RGBA 0.0f ~ 1.0f  
		float     lifeTime;  ///< エフェクトの残り寿命  
		Vector3   velocity;  ///< エフェクトの移動速度  
	};

	/**
	 * @struct DistanceEmitData
	 * @brief 距離放出判定（EmitType::Distance）で使用される状態データ
	 */
	struct DistanceEmitData final {
		Vector3 currentPosition;
		Vector3 nextPosition;
		float moveLength;
		float emitDistance;
		float emitInterval;
	};

	/**
	 * @struct TimeEmitData
	 * @brief 時間放出判定（EmitType::Time）で使用される状態データ
	 */
	struct TimeEmitData final {
		float emitTime;
		float emitInterval;
	};


public:
	/// ===================================================  
	/// public : methods  
	/// ===================================================  

	/**
	 * @brief コンストラクタ
	 */
	Effect();

	/**
	 * @brief デストラクタ
	 */
	~Effect() = default;

	/**
	 * @brief 新しいエフェクト粒子要素を生成（基本パラメータのみ）します。
	 */
	void CreateElement(const Vector3& _position, const Color& _color = Color::kWhite);

	/**
	 * @brief 新しいエフェクト粒子要素を生成（初期速度付き）します。
	 */
	void CreateElement(const Vector3& _position, const Vector3& _velocity, const Color& _color);

	/**
	 * @brief 新しいエフェクト粒子要素を詳細指定（サイズ・角度・速度・カラー等）で生成します。
	 */
	void CreateElement(const Vector3& _position, const Vector3& _scale, const Vector3& _rotate, const Vector3& _velocity, const Color& _color);

	/**
	 * @brief 指定インデックスのエフェクト粒子要素を削除します。
	 */
	void RemoveElement(size_t _index);

	/**
	 * @brief 粒子の自動放出（CreateParticle）が有効かを判定します。
	 */
	bool GetIsCreateParticle() const { return isCreateParticle_; }

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

	/**
	 * @brief 粒子描画に使用するメッシュアセットパスを設定します。
	 */
	void SetMeshPath(const std::string& _path) { meshPath_ = _path; }

	/**
	 * @brief 粒子描画に使用するテクスチャアセットパスを設定します。
	 */
	void SetTexturePath(const std::string& _path) { texturePath_ = _path; }

	/**
	 * @brief エフェクトのメインモジュール設定を設定します。
	 */
	void SetMainModule(const EffectMainModule& _module);

	/**
	 * @brief 放出形状（EmitShape）を設定します。
	 */
	void SetEmitShape(const EffectEmitShape& _shape);

	/**
	 * @brief 放出トリガー判定方式（EmitType）を設定します。
	 */
	void SetEmitType(EmitType _type);

	/**
	 * @brief 最大エフェクト要素数を設定します。
	 */
	void SetMaxEffectCount(size_t _maxCount);

	/**
	 * @brief 距離放出パラメータを設定します。
	 */
	void SetEmitTypeDistance(float _interval, size_t _emitInstanceCount);

	/**
	 * @brief 距離放出データ構造体を直接設定します。
	 */
	void SetEmitTypeDistance(const DistanceEmitData& _data);

	/**
	 * @brief 時間放出パラメータを設定します。
	 */
	void SetEmitTypeTime(const TimeEmitData& _data, size_t _emitInstanceCount);

	/**
	 * @brief 時間放出データ構造体を直接設定します。
	 */
	void SetEmitTypeTime(const TimeEmitData& _data);

	/**
	 * @brief 一度に放出する個数（インスタンス数）を設定します。
	 */
	void SetEmitInstanceCount(size_t _emitInstanceCount);

	/**
	 * @brief 放出粒子の初期寿命（秒）を設定します。
	 */
	void SetLifeLeftTime(float _time);

	/**
	 * @brief 粒子要素更新時のカスタムコールバックを設定します。
	 */
	void SetElementUpdateFunc(std::function<void(Element*)> _func);

	/**
	 * @brief ビルボード描画（カメラ正対）の有効・無効を設定します。
	 */
	void SetUseBillboard(bool _use);

	/**
	 * @brief 粒子の自動放出の有効・無効を設定します。
	 */
	void SetIsCreateParticle(bool _isCreateParticle);

	/**
	 * @brief カラーブレンドステート（BlendMode）を設定します。
	 */
	void SetBlendMode(BlendMode _blendMode);

	/**
	 * @brief 開始時の初期サイズを設定します。
	 */
	void SetStartSize(const Vector3& _size);

	/**
	 * @brief 開始時の初期サイズ範囲を設定します。
	 */
	void SetStartSize(const Vector3& _size1, const Vector3& _size2);

	/**
	 * @brief 開始時の初期回転角度を設定します。
	 */
	void SetStartRotate(const Vector3& _rotate);

	/**
	 * @brief 開始時の初期回転角度範囲を設定します。
	 */
	void SetStartRotate(const Vector3& _rotate1, const Vector3& _rotate2);

	/**
	 * @brief 開始時の初期カラーを設定します。
	 */
	void SetStartColor(const Color& _color);

	/**
	 * @brief 開始時の初期カラー範囲を設定します。
	 */
	void SetStartColor(const Color& _color1, const Color& _color2);

	/**
	 * @brief 開始時の初期速度を設定します。
	 */
	void SetStartSpeed(float _speed);

	/**
	 * @brief 開始時の初期速度範囲を設定します。
	 */
	void SetStartSpeed(float _speed1, float _speed2);

	/**
	 * @brief 放出形状を「球体（Sphere）」に設定します。
	 */
	void SetEmitShape(const Vector3& _center, float _radius);

	/**
	 * @brief 放出形状を「箱（Cube）」に設定します。
	 */
	void SetEmitShape(const Vector3& _center, const Vector3& _size);

	/**
	 * @brief 放出形状を「コーン（Cone）」に設定します。
	 */
	void SetEmitShape(const Vector3& _apex, float _angle, float _radius, float _height);




	/**
	 * @brief 粒子の自動放出が有効かを判定します。
	 */
	bool IsCreateParticle() const;
	size_t GetMaxEffectCount() const;
	const std::string& GetMeshPath() const;
	const std::string& GetTexturePath() const;


	/**
	 * @brief 現在アクティブな粒子要素の配列を取得します。
	 */
	const std::vector<Element>& GetElements() const;

	/**
	 * @brief 設定されているブレンドモードを取得します。
	 */
	BlendMode GetBlendMode() const;

	/**
	 * @brief メインモジュールのポインタを取得します。
	 */
	EffectMainModule* GetMainModule();

	/**
	 * @brief メインモジュールの読み取り専用参照を取得します。
	 */
	const EffectMainModule& GetMainModule() const;

	/**
	 * @brief 放出形状モジュールのポインタを取得します。
	 */
	EffectEmitShape* GetEmitShape();

	/**
	 * @brief 放出形状モジュールの読み取り専用参照を取得します。
	 */
	const EffectEmitShape& GetEmitShape() const;

	/**
	 * @brief 放出トリガー判定方式（EmitType）を取得します。
	 */
	int GetEmitType() const;

	/**
	 * @brief 距離放出パラメータの読み取り専用参照を取得します。
	 */
	const DistanceEmitData& GetDistanceEmitData() const;

	/**
	 * @brief 時間放出パラメータの読み取り専用参照を取得します。
	 */
	const TimeEmitData& GetTimeEmitData() const;

	/**
	 * @brief 一度放出する個数（インスタンス数）を取得します。
	 */
	size_t GetEmitInstanceCount() const;

};


namespace ComponentDebug {
	/**
	 * @brief エディタ用：Effectコンポーネントのデバッグ表示（Gui描画等）処理を行います。
	 */
	void EffectDebug(Effect* _effect);
} 

} /// ONEngine
