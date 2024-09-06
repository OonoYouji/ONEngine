#pragma once

#include <list>
#include <memory>

#include <BaseGameObject.h>

/// <summary>
/// BaseGameObjectを管理するクラス
/// </summary>
class GameObjectManager final {
	GameObjectManager() = default;
	~GameObjectManager() = default;
public:

	/// <summary>
	/// インスタンス確保関数
	/// </summary>
	/// <returns></returns>
	static inline GameObjectManager* GetInstance() {
		static GameObjectManager instance;
		return &instance;
	}


	/// <summary>
	/// ゲームオブジェクトのゲット
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	static BaseGameObject* GetGameObject(const std::string& name);

	/// <summary>
	/// すべてのオブジェクトを消去する
	/// </summary>
	static void DestoryAll();
	
	static std::string CreateName(const BaseGameObject* const object);


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();




	/// <summary>
	///  オブジェクトのUpdate呼び出し
	/// </summary>
	void Update();

	/// <summary>
	/// オブジェクトのLastUpdate呼び出し
	/// </summary>
	void LastUpdate();

	/// <summary>
	/// オブジェクトのDraw呼び出し
	/// </summary>
	void Draw();

	/// <summary>
	/// オブジェクトの前景スプライトの描画の呼び出し
	/// </summary>
	void FrontSpriteDraw();

	/// <summary>
	/// オブジェクトの背景スプライトの描画の呼び出し
	/// </summary>
	void BackSpriteDraw();


	void BackSpriteDraw(int layerId);
	void Object3dDraw(int layerId);
	void FrontSpriteDraw(int layerId);


	/////////////////////////////////////////////////////////////////////////////////////////////
	///	基本外部から使用することのない関数
	/////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// BaseGameObjectの追加
	/// </summary>
	/// <param name="object"></param>
	void AddGameObject(BaseGameObject* object);

	/// <summary>
	/// BaseGameObjectの削除
	/// </summary>
	/// <param name="object"></param>
	void SubGameObject(BaseGameObject* object);

	/// <summary>
	/// GameObejctの削除
	/// </summary>
	/// <param name="object"></param>
	void Destory(BaseGameObject* object);


	/// <summary>
	/// オブジェクトのインスタンス数にあった名前を付ける
	/// </summary>
	void ReName();
	


#pragma region ImGui
	/// <summary>
	/// imguiでデバッグ表示
	/// </summary>
	void ImGuiDebug();

	/// <summary>
	/// ImGui::Selectableでchild objectをインデントする
	/// </summary>
	/// <param name="childs"></param>
	void ImGuiSelectChilds(const std::list<BaseGameObject*>& childs);

	/// <summary>
	/// select objcetのデバッグ
	/// </summary>
	void ImGuiSelectObjectDebug();
#pragma endregion

private:

	std::list<std::unique_ptr<BaseGameObject>> objects_;
	BaseGameObject* selectObject_ = nullptr;

	std::list<BaseGameObject*> destoryList_;

private:
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator=(const GameObjectManager&) = delete;
	GameObjectManager& operator=(GameObjectManager&&) = delete;
};