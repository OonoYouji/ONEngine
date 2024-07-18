#pragma once

#include <ModelManager.h>
#include <PipelineState.h>


/// ===================================================
/// モデル
/// ===================================================
class Model final {
public:

	Model();
	~Model();

public:

	/// <summary>
	/// FillModeのセット
	/// </summary>
	/// <param name="fillMode"></param>
	void SetFillMode(FillMode fillMode);

	/// <summary>
	/// FillModeのゲッタ
	/// </summary>
	/// <returns></returns>
	inline FillMode GetFillMode() const { return fillMode_; }

private:

	FillMode fillMode_;

};