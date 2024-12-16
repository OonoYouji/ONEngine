#pragma once

#include "../BaseRenderable/BaseRenderable.h"


/// ===================================================
/// modelの表示に使うRenderable
/// ===================================================
class ModelRenderable : public BaseRenderable {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ModelRenderable();
	~ModelRenderable();

	void Initialize() override;
	void Draw()       override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};

