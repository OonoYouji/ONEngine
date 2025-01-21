#pragma once

/// std
#include <vector>
#include <memory>

/// engine
#include "../Renderer/Interface/IRenderer.h"
#include "../Shader/ShaderCompiler.h"


/* TODO: RenderingFrameworkクラスの実装
 *
 * ここにRendererのリストを持たせる
 * RendererはIRendererを継承している
 *
*/


/// ===================================================
/// 描画のフレームワーククラス
/// ===================================================
class RenderingFramework final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	RenderingFramework();
	~RenderingFramework();

	/// @brief 初期化
	void Initialize(class DxDevice* _dxDevice);

	/// @brief Rendererの作成を行う
	/// @tparam T 作成するRendererの型
	/// @return 生成したRendererのポインタ
	template<class T>
	T* GenerateRenderer() requires std::is_base_of_v<IRenderer, T>;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<ShaderCompiler>         shaderCompiler_;
	std::vector<std::unique_ptr<IRenderer>> renderers_; ///< レンダラーの配列

	class DxDevice* dxDevice_ = nullptr;

};


/// ===================================================
/// methods
/// ===================================================

template<class T>
inline T* RenderingFramework::GenerateRenderer() requires std::is_base_of_v<IRenderer, T> {
	/// rednererの作成
	std::unique_ptr<T> renderer = std::make_unique<T>();
	renderer->Initialize(shaderCompiler_.get(), dxDevice_);

	/// return用のポインタを取得
	T* result = renderer.get();

	/// リストに追加
	renderers_.push_back(std::move(renderer));

	return result;
}
