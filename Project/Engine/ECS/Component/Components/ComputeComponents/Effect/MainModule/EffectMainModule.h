#pragma once

/// std
#include <variant>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// //////////////////////////////////////////////////
/// Effectのmainモジュール
/// //////////////////////////////////////////////////
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


	/// @brief 定数データ
	template<typename T>
	struct ConstantData {
		ConstantData() = default;
		ConstantData(const T& _data) : constant(_data) {}

		T constant;
	};

	/// @brief 2つの定数データ
	template<typename T>
	struct TwoConstantData {
		TwoConstantData() = default;
		TwoConstantData(const std::pair<T, T>& _data) : constant(_data) {}
		TwoConstantData(const T& _data1, const T& _data2) : constant(std::make_pair(_data1, _data2)) {}

		std::pair<T, T> constant;
	};

	/// @brief 値の型
	template<typename T>
	using Value = std::variant<ConstantData<T>, TwoConstantData<T>>;

	/// @brief Valueの中の値を取得する  
	/// @tparam T 値の型  
	/// @param value Value型のデータ  
	/// @return 中の値  
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

	EffectMainModule();
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

	float GetLifeLeftTime() const { return lifeLeftTime_; }
	float GetStartSpeed() const { return startSpeed_; }
	float GetGravityModifier() const { return gravityModifier_; }

	void SetLifeLeftTime(float _time) { lifeLeftTime_ = _time; }
	void SetStartSpeed(float _speed) { startSpeed_ = _speed; }
	void SetGravityModifier(float _gravity) { gravityModifier_ = _gravity; }

	void SetSpeedStartData(const ConstantData<float>& _data) { speedStartData_ = _data; }
	void SetSpeedStartData(const TwoConstantData<float>& _data) { speedStartData_ = _data; }

	void SetSizeStartData(const ConstantData<Vector3>& _data) { sizeStartData_ = _data; }
	void SetSizeStartData(const TwoConstantData<Vector3>& _data) { sizeStartData_ = _data; }

	void SetRotateStartData(const ConstantData<Vector3>& _data) { rotateStartData_ = _data; }
	void SetRotateStartData(const TwoConstantData<Vector3>& _data) { rotateStartData_ = _data; }

	void SetColorStartData(const ConstantData<Color>& _data) { colorStartData_ = _data; }
	void SetColorStartData(const TwoConstantData<Color>& _data) { colorStartData_ = _data; }




	std::pair<float, float> GetSpeedStartData() const {
		return GetValue(speedStartData_);
	}

	std::pair<Vector3, Vector3> GetSizeStartData() const {
		return GetValue(sizeStartData_);
	}

	std::pair<Vector3, Vector3> GetRotateStartData() const {
		return GetValue(rotateStartData_);
	}

	std::pair<Color, Color> GetColorStartData() const {
		return GetValue(colorStartData_);
	}




};
