#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// externals
#include <jit/jit.h>

/// engine
#include "ECSGroup.h"
#include "../Entity/Collection/EntityCollection.h"
#include "../Entity/Prefab/EntityPrefabCollection.h"
#include "../Component/Collection/ComponentCollection.h"
#include "../System/SystemCollection/SystemCollection.h"

#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"

class CameraComponent;

void SetEntityComponentSystemPtr(ECSGroup* _gameGroup, ECSGroup* _debugGroup);
ECSGroup* GetEntityComponentSystemPtr();

/// ///////////////////////////////////////////////////
/// ECSの基盤クラス
/// ///////////////////////////////////////////////////
class EntityComponentSystem final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityComponentSystem(class DxManager* _pDxManager);
	~EntityComponentSystem();

	void Initialize(class AssetCollection* _assetCollection);
	void Update();
	void OutsideOfUpdate();

	void DebuggingUpdate();

	/// ----- group  ----- ///

	/// 追加
	ECSGroup* AddECSGroup(const std::string& _name);

	/// 取得
	ECSGroup* GetECSGroup(const std::string& _name) const;
	ECSGroup* GetCurrentGroup() const;

	/// main cameraの設定
	void MainCameraSetting();

	/// 現在のグループ
	void SetCurrentGroupName(const std::string& _name);
	const std::string& GetCurrentGroupName() const;

	/// すべてのECSグループの取得
	const std::unordered_map<std::string, std::unique_ptr<ECSGroup>>& GetECSGroups() const;

	/// ----- prefab ----- ///

	void ReloadPrefab(const std::string& _prefabName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other objects ----- ///
	class AssetCollection* pAssetCollection_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	/// ----- groups ----- ///
	std::unordered_map<std::string, std::unique_ptr<ECSGroup>> ecsGroups_;
	ECSGroup* debugGroup_ = nullptr;
	std::string currentGroupName_;

};


/// =============================================
/// お助け関数群
/// =============================================

/// @brief 親のエンティティが有効かどうかを再帰的にチェックする
/// @param _entity 対象のエンティティ
bool CheckParentEntityEnable(GameEntity* _entity);

/// @brief このコンポーネントが有効かどうかをチェックする
/// @param _component 対象のコンポーネント
/// @return true: 有効, false: 無効
bool CheckComponentEnable(IComponent* _component);


/// =============================================
/// monoを使ったC#スクリプトエンジンのコンポーネント
/// =============================================

namespace MONO_INTERNAL_METHOD {

	/// エンティティのidからEntityを取得
	GameEntity* GetEntityById(int32_t _entityId, const std::string& _groupName);

	/// @brief Componentの追加
	/// @param _entityId 対象エンティティ
	/// @param _monoTypeName Componentの型名
	/// @param _groupName ECSGroupの名前
	/// @return 追加したComponentのポインタの整数
	uint64_t InternalAddComponent(int32_t _entityId, MonoString* _monoTypeName, MonoString* _groupName);

	/// @brief Componentの取得
	/// @param _entityId 対象のエンティティID
	/// @param _monoTypeName Componentの型名
	/// @param _groupName ECSGroupの名前
	/// @return ゲットしたComponentのポインタの整数
	uint64_t InternalGetComponent(int32_t _entityId, MonoString* _monoTypeName, MonoString* _groupName);

	/// @brief エンティティの名前の取得
	/// @param _entityId 対象のエンティティID
	/// @param _groupName ECSGroupの名前
	/// @return 取得した名前の文字列ポインタ
	const char* InternalGetName(int32_t _entityId, MonoString* _groupName);

	/// @brief エンティティの命名
	/// @param _entityId 対象のエンティティID
	/// @param _name 新規の名前
	/// @param _groupName ECSGroupの名前
	void InternalSetName(int32_t _entityId, MonoString* _name, MonoString* _groupName);

	/// @brief エンティティの子のIDを取得
	/// @param _entityId 対象のエンティティID
	/// @param _childIndex 子のインデックス
	/// @param _groupName ECSGroupの名前
	/// @return 見つかった子エンティティのID
	int32_t InternalGetChildId(int32_t _entityId, uint32_t _childIndex, MonoString* _groupName);

	/// @brief 子エンティティの数を取得する
	/// @param _entityId 親エンティティID
	/// @param _groupName ECSGroupの名前
	/// @return 見つかった子エンティティの数
	int32_t InternalGetChildrenCount(int32_t _entityId, MonoString* _groupName);

	/// @brief エンティティの親のIDを取得
	/// @param _entityId 対象のエンティティID
	/// @param _groupName ECSGroupの名前
	/// @return 見つかった親エンティティのID
	int32_t InternalGetParentId(int32_t _entityId, MonoString* _groupName);

	/// @brief エンティティの親の設定
	/// @param _entityId エンティティID
	/// @param _parentId 親エンティティID
	/// @param _groupName ECSGroupの名前
	void InternalSetParent(int32_t _entityId, int32_t _parentId, MonoString* _groupName);

	/// @brief C#スクリプトの追加
	/// @param _entityId 対象のエンティティID
	/// @param _scriptName 追加するスクリプト名
	/// @param _groupName ECSGroupの名前
	void InternalAddScript(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName);

	/// @brief C#スクリプトの取得
	/// @param _entityId 対象のエンティティID
	/// @param _scriptName ゲットするスクリプト名
	/// @param _groupName ECSGroupの名前
	/// @return 見つかったかどうか
	bool InternalGetScript(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName);

	/// @brief エンティティの生成
	/// @param _entityId 作成されたエンティティIDのポインタ
	/// @param _prefabName 作成するPrefab名
	/// @param _groupName ECSGroupの名前
	void InternalCreateEntity(int32_t* _entityId, MonoString* _prefabName, MonoString* _groupName);

	/// @brief エンティティの削除
	/// @param _ecsGroupName ECSGroupの名前
	/// @param _entityId 対象のエンティティID
	void InternalDestroyEntity(MonoString* _ecsGroupName, int32_t _entityId);


	/// @brief エンティティの有効/無効の取得
	/// @param _entityId 対象のエンティティ
	/// @param _ecsGroupName ECSGroupの名前
	/// @return true: 有効, false: 無効
	bool InternalGetEnable(int32_t _entityId, MonoString* _ecsGroupName);

	/// @brief エンティティの有効/無効の設定
	/// @param _entityId 対象のエンティティID
	/// @param _enable 設定する値
	/// @param _ecsGroupName ECSGroupの名前
	void InternalSetEnable(int32_t _entityId, bool _enable, MonoString* _ecsGroupName);

} // namespace MONO_INTERNAL_METHOD
