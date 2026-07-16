#pragma once

/// engine
#include "../Entity/Collection/EntityCollection.h"
#include "../Component/Collection/ComponentCollection.h"
#include "../System/SystemCollection/SystemCollection.h"

#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"

namespace ONEngine {

template<typename T>
concept SystemType = std::is_base_of_v<ECSISystem, T>;

/// ///////////////////////////////////////////////////
/// ECSのコレクションのグループ
/// ///////////////////////////////////////////////////
/**
 * @class ECSGroup
 * @brief 1つのシーンまたはシーン単位でのECS（Entity, Component, System）のコレクションインスタンスをまとめて管理し、編集コマンド、更新、検索等のインターフェースを提供するECSマネジメントハブクラス
 */
class ECSGroup {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	/**
	 * @brief コンストラクタ
	 * @param _dxm DX12システムマネージャ
	 */
	ECSGroup(class DxManager* _dxm);
	/**
	 * @brief デストラクタ
	 */
	~ECSGroup();

	/**
	 * @brief グループ名を設定して初期化を行います。
	 * @param _groupName グループ名（シーン名等）
	 */
	void Initialize(const std::string& _groupName);
	/**
	 * @brief 1フレームのECS更新処理（SystemCollectionを介したSystemの更新など）を実行します。
	 */
	void Update();


	/// ----- entity ----- ///

	/**
	 * @brief GUIDを指定して新規のゲームエンティティを生成・追加します。
	 * @param _guid ユニークなGUID
	 * @param _isRuntime 実行時の一時オブジェクトかどうか
	 * @return 生成されたGameEntityポインタ
	 */
	GameEntity* GenerateEntity(const Guid& _guid, bool _isRuntime);
	/**
	 * @brief プレハブアセット名からゲームエンティティをクローン生成・追加します。
	 * @param _prefabName クローン元のプレハブ名
	 * @param _isRuntime 実行時の一時オブジェクトかどうか
	 * @return 生成されたGameEntityポインタ
	 */
	GameEntity* GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime = true);

	/**
	 * @brief GUIDをもとにゲームエンティティを検索して取得します。
	 * @param _guid 検索対象のGUID
	 * @return ゲームエンティティポインタ。存在しない場合は nullptr
	 */
	GameEntity* GetEntityFromGuid(const Guid& _guid);

	/**
	 * @brief ゲームエンティティを削除します（子の削除有無を選択可能）。
	 * @param _entity 削除対象のエンティティ
	 * @param _deleteChildren trueの場合、子エンティティも同時にツリーから削除します
	 */
	void RemoveEntity(GameEntity* _entity, bool _deleteChildren = true);
	/**
	 * @brief グループ内のすべてのゲームエンティティをクリーンアップ（削除）します。
	 */
	void RemoveEntityAll();

	/**
	 * @brief シーン遷移などで破棄しない（DontDestroyOnLoad）エンティティとして登録します。
	 * @param _entity 対象のエンティティ
	 */
	void AddDoNotDestroyEntity(GameEntity* _entity);
	/**
	 * @brief 非破棄エンティティ登録を解除します。
	 * @param _entity 対象のエンティティ
	 */
	void RemoveDoNotDestroyEntity(GameEntity* _entity);

	/**
	 * @brief 名前を指定してエンティティのIDを取得します。
	 * @param _name エンティティ名
	 * @return エンティティID（存在しなければUINT32_MAX等のエラー値）
	 */
	uint32_t GetEntityId(const std::string& _name);

	/**
	 * @brief 指定した名前のエンティティが何体登録されているかをカウントします。
	 * @param _name カウント対象のエンティティ名
	 * @return 登録数
	 */
	uint32_t CountEntity(const std::string& _name);


	/// ----- component ----- ///

	/**
	 * @brief 指定されたコンポーネント型（Comp）を新規生成・アロケートします。
	 * @tparam Comp 登録対象のコンポーネント型
	 * @return 生成された型付きコンポーネントポインタ
	 */
	template<IsComponent Comp>
	Comp* AddComponent();
	/**
	 * @brief コンポーネント名を文字列で指定して、新規コンポーネントを追加します。
	 * @param _compName コンポーネントのクラス名文字列
	 * @return 基底（IComponent）ポインタ
	 */
	IComponent* AddComponent(const std::string& _compName);

	/**
	 * @brief エンティティIDに対応する、指定された型（Comp）のコンポーネントを取得します。
	 * @tparam Comp 取得対象のコンポーネント型
	 * @param _entityId 対象のエンティティID
	 * @return 型付きコンポーネントポインタ
	 */
	template<IsComponent Comp>
	Comp* GetComponent(size_t _entityId);
	/**
	 * @brief 指定された型（Comp）のコンポーネント配列（ComponentArray）を取得します。
	 * @tparam Comp 対象コンポーネント型
	 * @return 配列ポインタ
	 */
	template<IsComponent Comp>
	ComponentArray<Comp>* GetComponentArray();

	/**
	 * @brief 型付きで、指定されたコンポーネントID（配列内インデックス）のコンポーネントを削除します。
	 * @tparam Comp 削除対象のコンポーネント型
	 * @param _compId コンポーネントID
	 */
	template<IsComponent Comp>
	void RemoveComponent(uint32_t _compId);
	/**
	 * @brief ハッシュとコンポーネントIDを指定して、コンポーネントを削除します。
	 * @param _hash コンポーネント型ハッシュ値
	 * @param _compId コンポーネントID
	 */
	void RemoveComponent(size_t _hash, uint32_t _compId);
	/**
	 * @brief 指定したゲームエンティティ（GameEntity）に属するすべてのコンポーネントを一括削除します。
	 * @param _entity 対象のエンティティポインタ
	 */
	void RemoveComponentAll(GameEntity* _entity);

	/**
	 * @brief エンティティに関連付けられたコンポーネント（エディタデータ等）の読み込み・再構築を行います。
	 * @param _entity 対象のエンティティ
	 */
	void LoadComponent(GameEntity* _entity);


	/// ----- system ----- ///

	/**
	 * @brief このグループに新しい制御システム（Sys）を追加します。
	 * @tparam Sys 追加するシステム型
	 * @tparam Args システム生成引数
	 * @param _args 引数リスト
	 */
	template<SystemType Sys, typename... Args>
	void AddSystem(Args... _args);

	/**
	 * @brief 実行時（ランタイム）以外のフェーズで動作するエディタ専用システム群の更新を実行します。
	 */
	void OutsideOfRuntimeUpdateSystems();
	/**
	 * @brief ランタイム（実行中）に動作する全制御システム群の更新を実行します。
	 */
	void RuntimeUpdateSystems();


private:
	/// ===================================================
	/// public : objects
	/// ===================================================

	/// ----- parameters ----- ///
	std::string groupName_;

	/// ----- collections ----- ///
	std::unique_ptr<EntityCollection> entityCollection_;
	std::unique_ptr<ComponentCollection> componentCollection_;
	std::unique_ptr<SystemCollection> systemCollection_;

	/// ----- command ----- ///
	Editor::EntityDataInputCommand componentInputCommand_;


public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// ----- setter ----- ///

	/**
	 * @brief このグループのメイン3Dカメラ（CameraComponent）を設定します。
	 * @param _camera カメラコンポーネントポインタ
	 */
	void SetMainCamera(CameraComponent* _camera);
	/**
	 * @brief このグループのメイン2Dカメラを設定します。
	 * @param _camera カメラコンポーネントポインタ
	 */
	void SetMainCamera2D(CameraComponent* _camera);


	/// ----- getter ----- ///

	/**
	 * @brief エンティティのコンテナ（EntityCollection）オブジェクトを取得します。
	 * @return コレクションへの生ポインタ
	 */
	EntityCollection* GetEntityCollection();
	/**
	 * @brief グループ内の全エンティティのリスト（ユニークポインタ配列）を取得します。
	 * @return エンティティ配列の参照
	 */
	const std::vector<std::unique_ptr<GameEntity>>& GetEntities() const;

	/**
	 * @brief 配列のインデックスから特定のゲームエンティティを取得します。
	 * @param _id インデックスID
	 * @return GameEntityポインタ
	 */
	GameEntity* GetEntity(int32_t _id) const;

	/**
	 * @brief メイン3Dカメラコンポーネント（読み取り専用）を取得します。
	 */
	const CameraComponent* GetMainCamera() const;
	/**
	 * @brief メイン3Dカメラコンポーネントを取得します。
	 */
	CameraComponent* GetMainCamera();
	/**
	 * @brief メイン2Dカメラコンポーネント（読み取り専用）を取得します。
	 */
	const CameraComponent* GetMainCamera2D() const;
	/**
	 * @brief メイン2Dカメラコンポーネントを取得します。
	 */
	CameraComponent* GetMainCamera2D();

	/**
	 * @brief このグループに割り当てられているグループ名を取得します。
	 */
	const std::string& GetGroupName() const;
};

/// ===================================================
/// inline methods
/// ===================================================

template<IsComponent Comp>
inline Comp* ECSGroup::AddComponent() {
	return componentCollection_->AddComponent<Comp>();
}

template<IsComponent Comp>
inline Comp* ECSGroup::GetComponent(size_t _entityId) {
	GameEntity* entity = entityCollection_->GetEntity(_entityId);
	if (entity) {
		return entity->GetComponent<Comp>();
	}

	return nullptr;
}

template<IsComponent Comp>
inline ComponentArray<Comp>* ECSGroup::GetComponentArray() {
	return componentCollection_->GetComponentArray<Comp>();
}

template<IsComponent Comp>
inline void ECSGroup::RemoveComponent(uint32_t _compId) {
	componentCollection_->RemoveComponent<Comp>(_compId);
}

template<SystemType Sys, typename ...Args>
inline void ECSGroup::AddSystem(Args ..._args) {
	systemCollection_->AddSystem(std::make_unique<Sys>(_args...));
}

} /// ONEngine
