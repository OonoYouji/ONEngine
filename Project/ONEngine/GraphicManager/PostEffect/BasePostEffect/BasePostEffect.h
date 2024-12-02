#pragma once

/// std
#include <string>


/// ===================================================
/// ポストエフェクトの基底クラス
/// ===================================================
class BasePostEffect {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BasePostEffect(const std::string& _uavTextureName);
	~BasePostEffect() = default;

	virtual void Initialize() = 0;
	virtual void Execution()  = 0;

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================
	
	std::string uavTextureName_;

public:

	void SetUAVTextureName(const std::string& _uavTextureName);

};