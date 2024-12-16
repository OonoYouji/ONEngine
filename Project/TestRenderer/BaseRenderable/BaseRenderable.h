#pragma once


/* ===================================================
	/// memo

	描画に使うRendererをInitializeで設定する



=================================================== */



/// std
#include <string>
#include <unordered_map>
#include <memory>


/// ===================================================
/// 描画されるオブジェクトの基底クラス
/// ===================================================
class BaseRenderable {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================


	virtual ~BaseRenderable() = default;

	virtual void Initialize() = 0;
	virtual void Draw()       = 0;


	/// <summary>
	/// 引数のrenderer mapからrendererを設定する
	/// </summary>
	/// <param name="_rendererMap"></param>
	void SettingRenderer(const std::unordered_map<std::string, std::unique_ptr<BaseRenderable>>& _rendererMap);

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	BaseRenderable* renderer_;     /// rendererのインスタンス
	std::string     rendererName_; /// rendererのkey

};