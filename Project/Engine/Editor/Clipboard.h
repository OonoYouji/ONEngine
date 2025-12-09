#pragma once

/// std
#include <memory>

/// externals
#include <nlohmann/json.hpp>

/// ////////////////////////////////////////////////
/// クリップボードにコピーするデータの基底クラス
/// ////////////////////////////////////////////////
class IClipboardData {
public:
	/// ==============================================
	/// public : virtual methods
	/// ==============================================
	virtual ~IClipboardData() = default;
	virtual const std::type_info& GetType() const = 0;
	virtual std::unique_ptr<IClipboardData> Clone() const = 0;
};

/// ////////////////////////////////////////////////
/// クリップボードにコピーするデータのテンプレートクラス
/// ////////////////////////////////////////////////
template<typename T>
class ClipboardData : public IClipboardData {
public:
	/// ==============================================
	/// public : methods
	/// ==============================================

	ClipboardData(const T& _data) {
		jsonData = _data;
	}

	/// type_infoを取得
	const std::type_info& GetType() const override {
		return typeid(T);
	}

	/// クローンを作成
	std::unique_ptr<IClipboardData> Clone() const override {
		T v = jsonData.get<T>();
		return std::make_unique<ClipboardData<T>>(v);
	}

	T* GetValue() {
		value = jsonData.get<T>();
		return &value;
	}

	/// ==============================================
	/// public : objects
	/// ==============================================

	T value;
	nlohmann::json jsonData;
};

/// /////////////////////////////////////////////////
/// エディタ等でコピーしたデータを一時的に保存するクラス
/// /////////////////////////////////////////////////
class Clipboard {
public:
	/// ==============================================
	/// public : methods
	/// ==============================================

	/// クリップボードにデータをセット
	template <typename T>
	void Set(const T& _value) {
		data_ = std::make_unique<ClipboardData<T>>(_value);
	}

	/// クリップボードからデータを取得
	template <typename T>
	T* Get() {
		if (data_ && data_->GetType() == typeid(T)) {
			ClipboardData<T>* typedData = static_cast<ClipboardData<T>*>(data_.get());
			return typedData->GetValue();
		}
		return nullptr;
	}

	/// クリップボードにデータがあるか
	bool HasData() const {
		return data_ != nullptr;
	}

	/// クリップボードのデータの型情報を取得
	const std::type_info& GetDataType() const {
		if (data_) {
			return data_->GetType();
		}
		return typeid(void);
	}

private:
	/// ==============================================
	/// private : objects
	/// ==============================================

	std::unique_ptr<IClipboardData> data_;

};
