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

	ClipboardData(const T& _data) : value(_data) {}

	/// type_infoを取得
	const std::type_info& GetType() const override {
		return typeid(T);
	}

	/// クローンを作成
	std::unique_ptr<IClipboardData> Clone() const override {
		return std::make_unique<ClipboardData<T>>(value);
	}

	/// ==============================================
	/// public : objects
	/// ==============================================

	T value;
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
			return &typedData->value;
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
