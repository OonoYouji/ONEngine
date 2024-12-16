#pragma once

/// std
#include <memory>
#include <string>
#include <unordered_map>


/// ===================================================
/// 描画を行う順番やRendererの設定を行う基底クラス
/// ===================================================
class BaseRenderingFramework {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~BaseRenderingFramework() = default;

	virtual void Initialize() = 0;
	virtual void Update()     = 0;


	/// <summary>
	/// 描画に使うRendererを追加する
	/// </summary>
	/// <param name="_name">     : mapのkey           </param>
	/// <param name="_renderer"> : mapに追加するobject </param>
	void AddRenderer(const std::string& _name, std::unique_ptr<class BaseRenderer> _renderer);



protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	std::unordered_map<std::string, std::unique_ptr<class BaseRenderer>> renderers_;



public:
	
	/// ===================================================
	/// public : accessors
	/// ===================================================


	/// <summary>
	/// Rendererの取得
	/// </summary>
	/// <param name="_name"> : mapのkey             </param>
	/// <returns>            : Rendererのインスタンス </returns>
	BaseRenderer* GetRenderer(const std::string& _name) const {
		return renderers_.at(_name).get();
	}

};