#include "ECSGroup.h"

using namespace ONEngine;

/// engine
#include "AddECSComponentFactoryFunction.h"

ECSGroup::ECSGroup(DxManager* _dxm) {
	/// インスタンスの生成
	entityCollection_ = std::make_unique<EntityCollection>(this, _dxm);
	componentCollection_ = std::make_unique<ComponentCollection>();
	systemCollection_ = std::make_unique<SystemCollection>();
}

ECSGroup::~ECSGroup() {}

/**
 * @brief グループ名を設定して初期化を行います。
 */
void ECSGroup::Initialize(const std::string& _groupName) {
	/// このECSGroupの名前を決める
	groupName_ = _groupName;

	AddComponentFactoryFunction(componentCollection_.get());
}

/**
 * @brief 1フレームのECS更新処理（SystemCollectionを介したSystemの更新など）を実行します。
 */
void ECSGroup::Update() {}

/**
 * @brief GUIDを指定して新規のゲームエンティティを生成・追加します。
 */
GameEntity* ECSGroup::GenerateEntity(const Guid& _guid, bool _isRuntime) {
	return entityCollection_->GenerateEntity(_guid, _isRuntime);
}

/**
 * @brief プレハブアセット名からゲームエンティティをクローン生成・追加します。
 */
GameEntity* ECSGroup::GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime) {
	return entityCollection_->GenerateEntityFromPrefab(_prefabName, _isRuntime);
}


/**
 * @brief GUIDをもとにゲームエンティティを検索して取得します。
 */
GameEntity* ECSGroup::GetEntityFromGuid(const Guid& _guid) {
	/// 例外チェック(無効値なら nullptr を返す)
	if (!_guid.CheckValid()) {
		Console::LogError("ECSGroup::GetEntityFromGuid: Invalid Guid provided.");
		return nullptr;
	}

	const auto& entities = entityCollection_->GetEntities();
	for (const auto& entity : entities) {
		if (entity->GetGuid() == _guid) {
			return entity.get();
		}
	}

	return nullptr;
}

/**
 * @brief ゲームエンティティを削除します（子の削除有無を選択可能）。
 */
void ECSGroup::RemoveEntity(GameEntity* _entity, bool _deleteChildren) {
	/// 例外チェック
	if (_entity == nullptr) {
		Console::LogError("ECSGroup::RemoveEntity: Null entity provided.");
		return;
	}

	entityCollection_->RemoveEntity(_entity, _deleteChildren);
}

/**
 * @brief グループ内のすべてのゲームエンティティをクリーンアップ（削除）します。
 */
void ECSGroup::RemoveEntityAll() {
	entityCollection_->RemoveEntityAll();
}

/**
 * @brief シーン遷移などで破棄しない（DontDestroyOnLoad）エンティティとして登録します。
 */
void ECSGroup::AddDoNotDestroyEntity(GameEntity* _entity) {
	if (_entity == nullptr) {
		Console::LogError("ECSGroup::AddDoNotDestroyEntity: Null entity provided.");
		return;
	}

	entityCollection_->AddDoNotDestroyEntity(_entity);
}

/**
 * @brief 非破棄エンティティ登録を解除します。
 */
void ECSGroup::RemoveDoNotDestroyEntity(GameEntity* _entity) {
	if (_entity == nullptr) {
		Console::LogError("ECSGroup::RemoveDoNotDestroyEntity: Null entity provided.");
		return;
	}

	entityCollection_->RemoveDoNotDestroyEntity(_entity);
}

/**
 * @brief 名前を指定してエンティティのIDを取得します。
 */
uint32_t ECSGroup::GetEntityId(const std::string& _name) {
	return entityCollection_->GetEntityId(_name);
}

/**
 * @brief 指定した名前のエンティティが何体登録されているかをカウントします。
 */
uint32_t ECSGroup::CountEntity(const std::string& _name) {
	const auto& entities = entityCollection_->GetEntities();
	return static_cast<uint32_t>(std::count_if(entities.begin(), entities.end(),
		[&_name](const std::unique_ptr<GameEntity>& entity) {
			std::string name = entity->GetName();
			/// 後ろから"_"を検索、"_"を含む場合はその前までを比較する
			if (name.find_last_of('_') != std::string::npos) {
				return name.substr(0, name.find_last_of('_')) == _name;
			}

			return name == _name;
		}
	));
}

/**
 * @brief コンポーネント名を文字列で指定して、新規コンポーネントを追加します。
 */
IComponent* ECSGroup::AddComponent(const std::string& _compName) {
	return componentCollection_->AddComponent(_compName);
}

/**
 * @brief ハッシュとコンポーネントIDを指定して、コンポーネントを削除します。
 */
void ECSGroup::RemoveComponent(size_t _hash, uint32_t _compId) {
	componentCollection_->RemoveComponent(_hash, _compId);
}

/**
 * @brief 指定したゲームエンティティ（GameEntity）に属するすべてのコンポーネントを一括削除します。
 */
void ECSGroup::RemoveComponentAll(GameEntity* _entity) {
	if (_entity == nullptr) {
		return;
	}

	componentCollection_->RemoveComponentAll(_entity);
}

/**
 * @brief エンティティに関連付けられたコンポーネント（エディタデータ等）の読み込み・再構築を行います。
 */
void ECSGroup::LoadComponent(GameEntity* _entity) {
	componentInputCommand_.SetEntity(_entity);
	componentInputCommand_.Execute();
}

/**
 * @brief 実行時（ランタイム）以外のフェーズで動作するエディタ専用システム群の更新を実行します。
 */
void ECSGroup::OutsideOfRuntimeUpdateSystems() {
	systemCollection_->OutsideOfRuntimeUpdate(this);
}

/**
 * @brief ランタイム（実行中）に動作する全制御システム群の更新を実行します。
 */
void ECSGroup::RuntimeUpdateSystems() {
	systemCollection_->RuntimeUpdate(this);
}

/**
 * @brief このグループのメイン3Dカメラ（CameraComponent）を設定します。
 */
void ECSGroup::SetMainCamera(CameraComponent* _camera) {
	entityCollection_->SetMainCamera(_camera);
}

/**
 * @brief このグループのメイン2Dカメラを設定します。
 */
void ECSGroup::SetMainCamera2D(CameraComponent* _camera) {
	entityCollection_->SetMainCamera2D(_camera);
}

/**
 * @brief エンティティのコンテナ（EntityCollection）オブジェクトを取得します。
 */
EntityCollection* ECSGroup::GetEntityCollection() {
	return entityCollection_.get();
}

/**
 * @brief グループ内の全エンティティのリスト（ユニークポインタ配列）を取得します。
 */
const std::vector<std::unique_ptr<GameEntity>>& ECSGroup::GetEntities() const {
	return entityCollection_->GetEntities();
}

/**
 * @brief 配列のインデックスから特定のゲームエンティティを取得します。
 */
GameEntity* ECSGroup::GetEntity(int32_t _id) const {
	return entityCollection_->GetEntity(_id);
}

/**
 * @brief メイン3Dカメラコンポーネント（読み取り専用）を取得します。
 */
const CameraComponent* ECSGroup::GetMainCamera() const {
	return entityCollection_->GetMainCamera();
}

/**
 * @brief メイン3Dカメラコンポーネントを取得します。
 */
CameraComponent* ECSGroup::GetMainCamera() {
	return entityCollection_->GetMainCamera();
}

/**
 * @brief メイン2Dカメラコンポーネント（読み取り専用）を取得します。
 */
const CameraComponent* ECSGroup::GetMainCamera2D() const {
	return entityCollection_->GetMainCamera2D();
}

/**
 * @brief メイン2Dカメラコンポーネントを取得します。
 */
CameraComponent* ECSGroup::GetMainCamera2D() {
	return entityCollection_->GetMainCamera2D();
}

/**
 * @brief このグループに割り当てられているグループ名を取得します。
 */
const std::string& ECSGroup::GetGroupName() const {
	return groupName_;
}
