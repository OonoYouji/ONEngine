#pragma once

/// engine
#include "GameObjectManager/GameObjectManager.h"

class PointLight : public BaseGameObject {
public:

	/// ===================================================
	/// public : sub class
	/// ===================================================

	struct BufferData final {
		Vec4 color;      /// 色
		Vec3 position;	 /// 座標
		float intencity; /// 輝度
		float radius;    /// ライトの届く最大距離
		float decay;     /// 減衰率
		int active;      /// 有効かどうか
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PointLight() { CreateTag(this); }
	~PointLight() {}

	void Initialize() override;
	void Update() override;
	void Debug() override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	BufferData data_;


public:

	/// ===================================================
	/// public : getter
	/// ===================================================

	/// <summary>
	/// ライトのバッファデータを取得
	/// </summary>
	/// <returns></returns>
	const BufferData& GetData() const { return data_; }

	/// <summary>
	/// ライトの色を設定
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vec4& color) { data_.color = color; }

	/// <summary>
	/// ライトの輝度を設定
	/// </summary>
	/// <param name="intencity"></param>
	void SetIntencity(float intencity) { data_.intencity = intencity; }

	/// <summary>
	/// ライトの範囲を設定
	/// </summary>
	/// <param name="radius"></param>
	void SetRadius(float radius) { data_.radius = radius; }

	/// <summary>
	/// ライトの減衰率を設定
	/// </summary>
	/// <param name="decay"></param>
	void SetDecay(float decay) { data_.decay = decay; }

	/// <summary>
	/// ライトの有効かどうかを設定
	/// </summary>
	/// <param name="active"></param>
	void SetActive(bool active) { data_.active = active; }

};