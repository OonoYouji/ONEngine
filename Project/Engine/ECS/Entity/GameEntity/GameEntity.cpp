#include "GameEntity.h"

using namespace ONEngine;

/// engine
#include <algorithm>
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Collection/ComponentCollection.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

GameEntity::GameEntity() {
	parent_ = nullptr;
}
GameEntity::~GameEntity() {}

/**
 * @brief エンティティの初期生成時（Awake）にコールされるコールバック。
 */
void GameEntity::Awake() {
	name_ = typeid(*this).name();
	name_ = name_.substr(strlen("class ONEngine::"));
	prefabName_ = "";

	transform_ = AddComponent<Transform>();
	AddComponent<Variables>();
}

/**
 * @brief コンポーネント名の文字列を指定して、新規コンポーネントを追加します。
 */
IComponent* GameEntity::AddComponent(const std::string& _name) {

	size_t hash = GetComponentHash(_name);
	auto it = components_.find(hash);
	if (it != components_.end()) { ///< すでに同じコンポーネントが存在している場合
		it->second->SetOwner(this);
		return it->second;
	}

	/// component の生成, 追加
	IComponent* component = pEcsGroup_->AddComponent(_name);
	if (!component) {
		return nullptr;
	}

	component->SetOwner(this);
	components_[hash] = component;

	return component;
}

/**
 * @brief コンポーネント名文字列を指定して、コンポーネントを取得します。
 */
IComponent* GameEntity::GetComponent(const std::string& _compName) const {

	/// stringをhashに変換
	size_t hash = GetComponentHash(_compName);

	/// hashからコンポーネントを取得
	auto itr = components_.find(hash);
	if (itr != components_.end()) {
		return itr->second;
	}

	/// コンポーネントが見つからない場合はnullptrを返す
	return nullptr;
}

/**
 * @brief コンポーネント名文字列を指定して、コンポーネントを削除します。
 */
void GameEntity::RemoveComponent(const std::string& _compName) {
	size_t hash = GetComponentHash(_compName);
	auto it = components_.find(hash);
	if (it != components_.end()) {
		pEcsGroup_->RemoveComponent(hash, it->second->id); ///< コンポーネントを削除
		components_.erase(it); ///< コンポーネントのマップから削除
	}

	if (_compName == "Transform") {
		transform_ = nullptr; ///< Transformコンポーネントを削除した場合はnullptrに設定
	}
}

/**
 * @brief このエンティティにアタッチされているすべてのコンポーネントを削除します。
 */
void GameEntity::RemoveComponentAll() {
	pEcsGroup_->RemoveComponentAll(this); ///< 全てのコンポーネントを削除
	components_.clear();
}

/**
 * @brief トランスフォームのワールド行列・親子関係を再計算・更新します。
 */
void GameEntity::UpdateTransform() {
	/// ----- 行列の更新(親があるならその行列をかけるのか判断して更新する) ----- ///

	transform_->Update();

	if (parent_) {

		if ((transform_->matrixCalcFlags & Transform::kAll) == Transform::kAll) {
			transform_->matWorld *= parent_->transform_->GetMatWorld();
			return;
		}

		Matrix4x4 matCancel = Matrix4x4::kIdentity;
		if (transform_->matrixCalcFlags & Transform::kScale) {
			matCancel = Matrix4x4::MakeScale(parent_->transform_->scale);
		}

		if (transform_->matrixCalcFlags & Transform::kRotate) {
			matCancel *= Matrix4x4::MakeRotate(parent_->transform_->rotate);
		}

		if (transform_->matrixCalcFlags & Transform::kPosition) {
			matCancel *= Matrix4x4::MakeTranslate(parent_->transform_->position);
		}

		transform_->matWorld *= matCancel;
	}
}

/**
 * @brief このエンティティおよびアタッチされている全コンポーネントを破棄予約し、コレクションから削除します。
 */
void GameEntity::Destroy() {
	pEcsGroup_->RemoveEntity(this);
}

/**
 * @brief ローカル位置座標を設定します。
 */
void GameEntity::SetPosition(const Vector3& _v) {
	transform_->position = _v;
	UpdateTransform();
}

/**
 * @brief オイラー角を指定してローカル回転を設定します。
 */
void GameEntity::SetRotate(const Vector3& _v) {
	transform_->rotate = Quaternion::FromEuler(_v);
}

/**
 * @brief クォータニオンを指定してローカル回転を設定します。
 */
void GameEntity::SetRotate(const Quaternion& _q) {
	transform_->rotate = _q;
}

/**
 * @brief ローカルスケール（拡大縮小）を設定します。
 */
void GameEntity::SetScale(const Vector3& _v) {
	transform_->scale = _v;
}

/**
 * @brief 指定されたエンティティをこのエンティティの親（Parent）として設定し、トランスフォームの階層構造を構築します。
 */
void GameEntity::SetParent(GameEntity* _parent) {
	/// 親子関係の解除
	if (!_parent) {
		RemoveParent();
		return;
	}

	if (parent_ == _parent) {
		return;
	}

	RemoveParent();

	_parent->children_.push_back(this);
	parent_ = _parent;
}

/**
 * @brief 現在アタッチされている親エンティティとの親子関係を解除します。
 */
void GameEntity::RemoveParent() {
	if (parent_) {
		auto itr = std::remove_if(parent_->children_.begin(), parent_->children_.end(),
			[this](GameEntity* child) {
				return child == this;
			}
		);
		parent_->children_.erase(itr, parent_->children_.end());
		parent_ = nullptr;
	}
}

/**
 * @brief 子エンティティリスト内での順番を並び替えます（ヒエラルキーの順序等）。
 */
void GameEntity::MoveChild(GameEntity* _child, size_t _newIndex) {
	if (!_child || _child->parent_ != this) {
		return;
	}

	auto it = std::find(children_.begin(), children_.end(), _child);
	if (it != children_.end()) {
		children_.erase(it);
		if (_newIndex > children_.size()) {
			_newIndex = children_.size();
		}
		children_.insert(children_.begin() + _newIndex, _child);
	}
}

/**
 * @brief エンティティの識別名を設定します。
 */
void GameEntity::SetName(const std::string& _name) {
	name_ = _name;
}

/**
 * @brief このエンティティがアタッチしているプレハブのアセットファイル名を設定します。
 */
void GameEntity::SetPrefabName(const std::string& _name) {
	prefabName_ = _name;
}

/**
 * @brief ローカル位置座標を取得します。
 */
const Vector3& GameEntity::GetLocalPosition() const {
	return transform_->position;
}

/**
 * @brief オイラー角によるローカル回転値を取得します。
 */
Vector3 GameEntity::GetLocalRotate() const {
	return Quaternion::ToEuler(transform_->rotate);
}

/**
 * @brief クォータニオンによるローカル回転値を取得します。
 */
const Quaternion& GameEntity::GetLocalRotateQuaternion() const {
	return transform_->rotate;
}

/**
 * @brief ローカルスケール値を取得します。
 */
const Vector3& GameEntity::GetLocalScale() const {
	return transform_->scale;
}

/**
 * @brief ワールド位置座標を取得します（親の変換行列を再帰的に考慮）。
 */
Vector3 GameEntity::GetPosition() {
	Vector3 position = {
		transform_->matWorld.m[3][0],
		transform_->matWorld.m[3][1],
		transform_->matWorld.m[3][2]
	};

	return position;
}

/**
 * @brief オイラー角によるワールド回転値を取得します。
 */
Vector3 GameEntity::GetRotate() {
	if (!parent_) {
		return Quaternion::ToEuler(transform_->rotate);
	}

	// 自身のローカル回転を加算  
	return Quaternion::ToEuler(parent_->GetRotateQuaternion() * transform_->rotate);
}

/**
 * @brief クォータニオンによるワールド回転値を取得します。
 */
Quaternion GameEntity::GetRotateQuaternion() {
	if (!parent_) {
		return transform_->rotate;
	}

	return parent_->GetRotateQuaternion() * transform_->rotate;
}

/**
 * @brief ワールドスケール値を取得します。
 */
Vector3 GameEntity::GetScale() {
	return transform_->scale;
}

/**
 * @brief このエンティティの基本変形情報（Transformコンポーネント）を取得します。
 */
Transform* GameEntity::GetTransform() const {
	return transform_;
}

/**
 * @brief 現在の親エンティティ（読み取り専用）を取得します。
 */
const GameEntity* GameEntity::GetParent() const {
	return parent_;
}

/**
 * @brief 現在の親エンティティを取得します。
 */
GameEntity* GameEntity::GetParent() {
	return parent_;
}

/**
 * @brief 指定された子エンティティをリストから切り離し（親子関係を解除）、成功したかどうかを返します。
 */
bool GameEntity::RemoveChild(GameEntity* _child) {
	/// ----- 子エンティティの削除 ----- ///

	if (!_child) {
		return false;
	}

	/// 子エンティティが存在するか確認して削除
	auto it = std::remove(children_.begin(), children_.end(), _child);
	if (it != children_.end()) {
		children_.erase(it, children_.end());
		_child->RemoveParent();
		return true;
	}

	return false;
}

/**
 * @brief 直属の子エンティティ群のリスト（読み取り専用）を取得します。
 */
const std::vector<GameEntity*>& GameEntity::GetChildren() const {
	return children_;
}

/**
 * @brief インデックスを指定して直属の子エンティティを1つ取得します。
 */
GameEntity* GameEntity::GetChild(size_t _index) {
	return children_[_index];
}

/**
 * @brief アタッチされている全コンポーネントのハッシュマップ（読み取り専用）を取得します。
 */
const std::unordered_map<size_t, IComponent*>& GameEntity::GetComponents() const {
	return components_;
}

/**
 * @brief アタッチされている全コンポーネントのハッシュマップを取得します。
 */
std::unordered_map<size_t, IComponent*>& GameEntity::GetComponents() {
	return components_;
}

/**
 * @brief エンティティの識別名を取得します。
 */
const std::string& GameEntity::GetName() const {
	return name_;
}

/**
 * @brief 関連付けられているプレハブのアセット名を取得します。
 */
const std::string& GameEntity::GetPrefabName() const {
	return prefabName_;
}

/**
 * @brief このエンティティがプレハブを元にクローン生成されたものであるかを判定します。
 */
bool GameEntity::ContainsPrefab() const {
	/// 空文字列でないかチェック
	return prefabName_ != "";
}

/**
 * @brief ランタイム/非ランタイムを区別する一意なエンティティIDを取得します。
 */
int32_t GameEntity::GetId() const {
	return id_;
}

/**
 * @brief エンティティの一意なGUIDを取得します。
 */
const Guid& GameEntity::GetGuid() const {
	return guid_;
}

/**
 * @brief このエンティティが所属するECSGroup（シーン管理ハブ）を取得します。
 */
ECSGroup* GameEntity::GetECSGroup() const {
	return pEcsGroup_;
}



/**
 * @brief JSON へのシリアライズ変換
 */
void ONEngine::to_json(nlohmann::json& _j, const GameEntity& _entity) {
	_j = EntityJsonConverter::ToJson(&_entity);
}

/**
 * @brief JSON からのデシリアライズ変換
 */
void ONEngine::from_json(const nlohmann::json& _j, GameEntity& _entity) {
	// GameEntity should already be instantiated and have its ID/Guid set by the collection
	EntityJsonConverter::FromJson(_j, &_entity, _entity.GetECSGroup()->GetGroupName());
}
