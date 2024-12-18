#pragma once

#include <list>
#include <vector>
#include <memory>

#include <imgui.h>

#include "BaseGameObject.h"

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

	static uint32_t GetInstanceCount(const std::string& tag);
	static std::list<BaseGameObject*> GetGameObjectList(const std::string& tag);
	

	/// <summary>
	/// すべてのオブジェクトを消去する
	/// </summary>
	static void DestoryAll();
	
	static bool IsAliveObject(BaseGameObject* object);

	static void AddObjectsToObjectsCopy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	void GameObjectInitialize(int sceneId);

	/// <summary>
	///  オブジェクトのUpdate呼び出し
	/// </summary>
	void Update();

	/// <summary>
	/// オブジェクトのLastUpdate呼び出し
	/// </summary>
	void LastUpdate();

	void Object3dDraw(int layerId);


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

	void Hierarchy(ImGuiWindowFlags _windowFlags);
	void Inspector(ImGuiWindowFlags _windowFlags);

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

	const uint32_t kMaxInstanceCount_ = 1280u;
	std::vector<std::unique_ptr<BaseGameObject>> objects_;
	BaseGameObject* selectObject_ = nullptr;

	std::list<BaseGameObject*> destoryList_;
	std::list<BaseGameObject*> addObjectList_;

private:
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator=(const GameObjectManager&) = delete;
	GameObjectManager& operator=(GameObjectManager&&) = delete;
};