#include "GameEntity.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Collection/ComponentCollection.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"

GameEntity::GameEntity() {
	parent_ = nullptr;
}
GameEntity::~GameEntity() {}

void GameEntity::Awake() {
	name_ = typeid(*this).name();
	name_.erase(0, 6);
	prefabName_ = "";

	pECSGroup_->LoadComponent(this);

	transform_ = AddComponent<Transform>();
	variables_ = AddComponent<Variables>();
}

IComponent* GameEntity::AddComponent(const std::string& _name) {

	size_t hash = GetComponentHash(_name);
	auto it = components_.find(hash);
	if (it != components_.end()) { ///< すでに同じコンポーネントが存在している場合
		it->second->SetOwner(this);
		return it->second;
	}

	/// component の生成, 追加
	IComponent* component = pECSGroup_->AddComponent(_name);
	if (!component) {
		return nullptr;
	}

	component->SetOwner(this);
	components_[hash] = component;

	return component;
}

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

void GameEntity::RemoveComponent(const std::string& _compName) {
	size_t hash = GetComponentHash(_compName);
	auto it = components_.find(hash);
	if (it != components_.end()) {
		pECSGroup_->RemoveComponent(hash, it->second->id); ///< コンポーネントを削除
		components_.erase(it); ///< コンポーネントのマップから削除
	}

	if (_compName == "Transform") {
		transform_ = nullptr; ///< Transformコンポーネントを削除した場合はnullptrに設定
	} else if (_compName == "Variables") {
		variables_ = nullptr; ///< Variablesコンポーネントを削除した場合はnullptrに設定
	}
}

void GameEntity::RemoveComponentAll() {
	pECSGroup_->RemoveComponentAll(this); ///< 全てのコンポーネントを削除
	components_.clear();
}

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

void GameEntity::Destroy() {
	pECSGroup_->RemoveEntity(this);
}

void GameEntity::SetPosition(const Vector3& _v) {
	transform_->position = _v;
	UpdateTransform();
}

void GameEntity::SetRotate(const Vector3& _v) {
	transform_->rotate = Quaternion::FromEuler(_v);
}

void GameEntity::SetRotate(const Quaternion& _q) {
	transform_->rotate = _q;
}

void GameEntity::SetScale(const Vector3& _v) {
	transform_->scale = _v;
}

void GameEntity::SetParent(GameEntity* _parent) {
	/// 親子関係の解除
	if (!_parent) {
		RemoveParent();
		return;
	}
	_parent->children_.push_back(this);
	parent_ = _parent;
}

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

void GameEntity::SetName(const std::string& _name) {
	name_ = _name;
}

void GameEntity::SetPrefabName(const std::string& _name) {
	prefabName_ = _name;
}

const Vector3& GameEntity::GetLocalPosition() const {
	return transform_->position;
}

Vector3 GameEntity::GetLocalRotate() const {
	return Quaternion::ToEuler(transform_->rotate);
}

const Quaternion& GameEntity::GetLocalRotateQuaternion() const {
	return transform_->rotate;
}

const Vector3& GameEntity::GetLocalScale() const {
	return transform_->scale;
}

Vector3 GameEntity::GetPosition() {
	Vector3 position = {
		transform_->matWorld.m[3][0],
		transform_->matWorld.m[3][1],
		transform_->matWorld.m[3][2]
	};
	
	return position;
}

Vector3 GameEntity::GetRotate() {
	if (!parent_) {
		return Quaternion::ToEuler(transform_->rotate);
	}

	// 自身のローカル回転を加算  
	return Quaternion::ToEuler(parent_->GetRotateQuaternion() * transform_->rotate);
}

Quaternion GameEntity::GetRotateQuaternion() {
	if (!parent_) {
		return transform_->rotate;
	}

	return parent_->GetRotateQuaternion() * transform_->rotate;
}

Vector3 GameEntity::GetScale() {
	return transform_->scale;
}

Transform* GameEntity::GetTransform() const {
	return transform_;
}

const GameEntity* GameEntity::GetParent() const {
	return parent_;
}

GameEntity* GameEntity::GetParent() {
	return parent_;
}

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

const std::vector<GameEntity*>& GameEntity::GetChildren() const {
	return children_;
}

GameEntity* GameEntity::GetChild(size_t _index) {
	return children_[_index];
}

const std::unordered_map<size_t, IComponent*>& GameEntity::GetComponents() const {
	return components_;
}

std::unordered_map<size_t, IComponent*>& GameEntity::GetComponents() {
	return components_;
}

const std::string& GameEntity::GetName() const {
	return name_;
}

const std::string& GameEntity::GetPrefabName() const {
	return prefabName_;
}

bool GameEntity::ContainsPrefab() const {
	/// 空文字列でないかチェック
	return prefabName_ != "";
}

bool GameEntity::GetActive() const {
	return active_;
}

int32_t GameEntity::GetId() const {
	return id_;
}

const Guid& GameEntity::GetGuid() const {
	return guid_;
}

ECSGroup* GameEntity::GetECSGroup() const {
	return pECSGroup_;
}



void to_json(nlohmann::json& _j, const GameEntity& _entity) {
	/// ----- GameEntityからJsonを生成 ----- ///

	nlohmann::json entityJson = nlohmann::json::object();
	entityJson["prefabName"] = _entity.GetPrefabName();
	entityJson["name"] = _entity.GetName();
	entityJson["id"] = _entity.GetId();

	// コンポーネントの情報を追加
	auto& components = _entity.GetComponents();
	for (const auto& component : components) {
		entityJson["components"].push_back(ComponentJsonConverter::ToJson(component.second));
	}

	/// 親子関係の情報を追加
	if (_entity.GetParent()) {
		entityJson["parent"] = _entity.GetParent()->GetId();
	} else {
		entityJson["parent"] = nullptr;
	}

	_j = nlohmann::json{
		{ "name", _entity.GetName() },
		{ "prefabName", _entity.GetPrefabName() },
		{ "active", _entity.GetActive() },
		{ "components", nlohmann::json::array() }
	};
}

void from_json(const nlohmann::json& /*_j*/, GameEntity& /*_entity*/) {
	/// ----- JsonからGameEntityを生成 ----- ///

}
