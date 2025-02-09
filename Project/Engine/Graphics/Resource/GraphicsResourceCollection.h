#pragma once

/// std
#include <memory>

/// engine
#include "Loader/GraphicsResourceLoader.h"

/// ===================================================
/// グラフィクスリソースのコレクション
/// ===================================================
class GraphicsResourceCollection final {
public:

	/// ===================================================
	/// public : sub class, enum
	/// ===================================================

	/// @brief resourceの種類
	enum class type {
		texture,
		model,
	};


public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsResourceCollection();
	~GraphicsResourceCollection();

	void Initialize(class DxManager* _dxManager);

	void Load(const std::string& _filePath, type _type);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<GraphicsResourceLoader> resourceLoader_;

};

