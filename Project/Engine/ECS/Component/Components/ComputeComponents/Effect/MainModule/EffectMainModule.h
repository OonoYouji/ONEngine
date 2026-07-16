#pragma once

/// std
#include <variant>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// //////////////////////////////////////////////////
/// Effectのmainモジュール
/// //////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class EffectMainModule
 * @brief エフェクト粒子の初期化（初期速度、サイズ、回転、カラー、寿命等）や物理的な挙動に関する設定データを管理するモジュールクラス
 */
class EffectMainModule final {
	friend class Effect;
	friend class EffectUpdateSystem;
public:
	/// ===================================================
	/// public : sub class
	/// ===================================================

	enum {
		Constant,
		TwoConstant,
	};


	/**
	 * @struct ConstantData
	 * @brief 単一の定数値を表す構造体
	 */
	template<typename T>
	struct ConstantData {
		ConstantData() = default;
		ConstantData(const T& _data) : constant(_data) {}

		T constant;
	};

	/**
	 * @struct TwoConstantData
	 * @brief 最小値と最大値のペアによる範囲指定を表す構造体
	 */
	template<typename T>
	struct TwoConstantData {
		TwoConstantData() = default;
		TwoConstantData(const std::pair<T, T>& _data) : constant(_data) {}
		TwoConstantData(const T& _data1, const T& _data2) : constant(std::make_pair(_data1, _data2)) {}

		std::pair<T, T> constant;
	};

	/// @brief 定数または範囲データ
	template<typename T>
	using Value = std::variant<ConstantData<T>, TwoConstantData<T>>;

	/**
	 * @brief Valueから最小・最大範囲（単一値の場合は同じ値のペア）を展開取得します。
	 * @param value 対象データ
	 * @return 最小・最大値のペア
	 */
	template<typename T>
	std::pair<T, T> GetValue(const Value<T>& value) const {
		return std::visit([](auto&& _arg) -> std::pair<T, T> {
			using ArgType = std::decay_t<decltype(_arg)>;
			if constexpr (std::is_same_v<ArgType, ConstantData<T>>) {
				return std::make_pair(_arg.constant, _arg.constant);
			} else if constexpr (std::is_same_v<ArgType, TwoConstantData<T>>) {
				// 2つの定数の場合、最初の値を返す  
				return _arg.constant;
			}
			},
			value
		);
	}

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	EffectMainModule();

	/**
	 * @brief デストラクタ
	 */
	~EffectMainModule();


private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	float lifeLeftTime_ = 0.0f; ///< 残り寿命
	float startSpeed_ = 0.0f;   ///< 開始速度

	Value<float> speedStartData_;    ///< 開始速度のデータ
	Value<Vector3> sizeStartData_;   ///< 開始サイズのデータ
	Value<Vector3> rotateStartData_; ///< 開始回転のデータ
	Value<Color> colorStartData_;    ///< 開始色のデータ

	float gravityModifier_ = 0.0f; ///< 重力の影響



public:
	/// ====================================================
	/// public : accessors
	/// ====================================================

	/**
	 * @brief 残り寿命を取得します。
	 */
	float GetLifeLeftTime() const { return lifeLeftTime_; }

	/**
	 * @brief 開始速度を取得します。
	 */
	float GetStartSpeed() const { return startSpeed_; }

	/**
	 * @brief 重力影響倍率を取得します。
	 */
	float GetGravityModifier() const { return gravityModifier_; }

	/**
	 * @brief 残り寿命を設定します。
	 */
	void SetLifeLeftTime(float _time) { lifeLeftTime_ = _time; }

	/**
	 * @brief 開始速度を設定します。
	 */
	void SetStartSpeed(float _speed) { startSpeed_ = _speed; }

	/**
	 * @brief 重力影響倍率を設定します。
	 */
	void SetGravityModifier(float _gravity) { gravityModifier_ = _gravity; }

	/**
	 * @brief 開始速度の定数データを設定します。
	 */
	void SetSpeedStartData(const ConstantData<float>& _data) { speedStartData_ = _data; }

	/**
	 * @brief 開始速度の範囲指定データを設定します。
	 */
	void SetSpeedStartData(const TwoConstantData<float>& _data) { speedStartData_ = _data; }

	/**
	 * @brief 開始サイズの定数データを設定します。
	 */
	void SetSizeStartData(const ConstantData<Vector3>& _data) { sizeStartData_ = _data; }

	/**
	 * @brief 開始サイズの範囲指定データを設定します。
	 */
	void SetSizeStartData(const TwoConstantData<Vector3>& _data) { sizeStartData_ = _data; }

	/**
	 * @brief 開始回転角度の定数データを設定します。
	 */
	void SetRotateStartData(const ConstantData<Vector3>& _data) { rotateStartData_ = _data; }

	/**
	 * @brief 開始回転角度の範囲指定データを設定します。
	 */
	void SetRotateStartData(const TwoConstantData<Vector3>& _data) { rotateStartData_ = _data; }

	/**
	 * @brief 開始カラーの定数データを設定します。
	 */
	void SetColorStartData(const ConstantData<Color>& _data) { colorStartData_ = _data; }

	/**
	 * @brief 開始カラーの範囲指定データを設定します。
	 */
	void SetColorStartData(const TwoConstantData<Color>& _data) { colorStartData_ = _data; }




	/**
	 * @brief 開始速度データ範囲を取得します。
	 */
	std::pair<float, float> GetSpeedStartData() const {
		return GetValue(speedStartData_);
	}

	/**
	 * @brief 開始サイズデータ範囲を取得します。
	 */
	std::pair<Vector3, Vector3> GetSizeStartData() const {
		return GetValue(sizeStartData_);
	}

	/**
	 * @brief 開始回転角度データ範囲を取得します。
	 */
	std::pair<Vector3, Vector3> GetRotateStartData() const {
		return GetValue(rotateStartData_);
	}

	/**
	 * @brief 開始カラーデータ範囲を取得します。
	 */
	std::pair<Color, Color> GetColorStartData() const {
		return GetValue(colorStartData_);
	}




};

} /// ONEngine
