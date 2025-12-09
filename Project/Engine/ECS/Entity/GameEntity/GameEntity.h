#pragma once

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/ECS/Component/Collection/ComponentHash.h"

template <typename T>
concept ComponentType = std::is_base_of_v<IComponent, T>;

/// ////////////////////////////////////////////////////
/// エンティティインターフェース
/// ////////////////////////////////////////////////////
class GameEntity {
	friend class EntityComponentSystem;
	friend class EntityCollection;
	friend class SceneIO;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GameEntity();
	~GameEntity();

	/// @brief GameEntityの生成時に行う処理
	void Awake();


	/// --------------------------------------------------
	/// component 関連
	/// --------------------------------------------------

	/// @brief component の追加
	/// @tparam T 追加する component の型
	/// @return 追加した component のポインタ
	template <ComponentType Comp>
	Comp* AddComponent();

	/// @brief stringから component を追加する
	/// @param _name componentの名前
	/// @return 追加した component のポインタ
	IComponent* AddComponent(const std::string& _name);

	/// @brief component の取得
	/// @tparam T ゲットする component の型
	/// @return component のポインタ
	template <ComponentType Comp>
	Comp* GetComponent() const;

	/// @brief Componentの名前から取得する
	/// @param _compName Componentの名前
	IComponent* GetComponent(const std::string& _compName) const;

	/// @brief Componentの削除
	/// @tparam Comp 削除するComponentの型
	template <ComponentType Comp>
	void RemoveComponent();

	/// @brief Componentの名前から削除する
	/// @param _compName Componentの名前
	void RemoveComponent(const std::string& _compName);

	/// @brief すべてのComponentの削除
	void RemoveComponentAll();

	/// @brief Componentのマップの取得
	/// @return componentのマップの参照
	const std::unordered_map<size_t, IComponent*>& GetComponents() const;

	/// @brief Componentのマップの参照を取得
	/// @return comonentのマップの参照
	std::unordered_map<size_t, IComponent*>& GetComponents();


	/// --------------------------------------------------
	/// transform 関連
	/// --------------------------------------------------

	/// @brief ワールド座標行列の更新
	void UpdateTransform();

	/// @brief local position の設定
	/// @param _v 座標
	void SetPosition(const Vector3& _v);

	/// @brief local rotation の設定
	/// @param _v Euler角度
	void SetRotate(const Vector3& _v);

	/// @brief local rotation の設定
	/// @param _q Quaternion回転
	void SetRotate(const Quaternion& _q);

	/// @brief local scale の設定
	/// @param _v 拡縮度
	void SetScale(const Vector3& _v);

	/// @brief local position の取得
	const Vector3& GetLocalPosition() const;

	/// @brief local rotation の取得
	/// @return Euler角度
	Vector3 GetLocalRotate() const;

	/// @brief local rotation の取得
	/// @return Quaternion回転
	const Quaternion& GetLocalRotateQuaternion() const;

	/// @brief local scale の取得
	/// @return 拡縮度
	const Vector3& GetLocalScale() const;


	/// @brief world position の取得
	/// @return world座標
	Vector3 GetPosition();

	/// @brief world rotation の取得
	/// @return Euler角度
	Vector3 GetRotate();

	/// @brief world rotation の取得
	/// @return Quaternion回転
	Quaternion GetRotateQuaternion();

	/// @brief world scale の取得
	/// @return world拡縮度
	Vector3 GetScale();

	/// @brief transform component の取得
	/// @return transform component へのポインタ
	Transform* GetTransform() const;


	/// --------------------------------------------------
	/// parent - children 関連
	/// --------------------------------------------------

	/// @brief 親エンティティの設定
	/// @param _parent 親エンティティのポインタ
	void SetParent(GameEntity* _parent);

	/// @brief 親エンティティの解除
	void RemoveParent();

	/// @brief 親エンティティの取得
	/// @return 親エンティティのポインタ
	const GameEntity* GetParent() const;

	/// @brief 親エンティティの取得
	/// @return 親エンティティのポインタ
	GameEntity* GetParent();


	/// @brief 子エンティティの削除
	/// @param _child 子エンティティのポインタ
	/// @return true: 削除成功, false: 削除失敗
	bool RemoveChild(GameEntity* _child);

	/// @brief 子エンティティの配列を取得する
	/// @return Entityへのポインタ配列
	const std::vector<GameEntity*>& GetChildren() const;

	/// @brief 子エンティティの取得
	/// @param _index 配列のインデックス
	/// @return 子エンティティのポインタ
	GameEntity* GetChild(size_t _index);


	/// --------------------------------------------------
	/// other parameters
	/// --------------------------------------------------

	/// @brief エンティティの名前の設定
	void SetName(const std::string& _name);

	/// @brief エンティティの名前の取得
	/// @return name のconst参照
	const std::string& GetName() const;


	/// @brief thisに対応するprefabの名前を設定する
	void SetPrefabName(const std::string& _name);

	/// @brief prefabの名前の取得
	const std::string& GetPrefabName() const;

	/// @brief prefabを持っているか(prefabを元に生成されたのか)を取得
	/// @return true: 持っている, false: 持っていない
	bool ContainsPrefab() const;


	/// @brief エンティティのidの取得
	/// @return id > 0 なら runtime外で生成された、 id < 0 なら runtime中に生成された
	int32_t GetId() const;

	/// @brief guidの取得
	/// @return guidのconst参照
	const Guid& GetGuid() const;

	/// @brief thisが所属するECSGroupの取得
	class ECSGroup* GetECSGroup() const;



	/// --------------------------------------------------
	/// this entity methods
	/// --------------------------------------------------

	/// @brief 自身の削除処理
	void Destroy();


public:
	/// ===================================================
	/// public : objects
	/// ===================================================

	bool active = true; ///< true のときは更新する


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Transform* transform_;
	class ECSGroup* pEcsGroup_;

	int32_t id_ = 0; ///< entityのID
	Guid guid_; ///< entityのGUID

	std::unordered_map<size_t, IComponent*> components_;
	std::vector<GameEntity*> children_;
	GameEntity* parent_;
	std::string name_;
	std::string prefabName_;

};

template<ComponentType Comp>
inline Comp* GameEntity::AddComponent() {
	std::string name = typeid(Comp).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}
	return static_cast<Comp*>(AddComponent(name));
}

template<ComponentType Comp>
inline Comp* GameEntity::GetComponent() const {
	auto it = components_.find(GetComponentHash<Comp>());
	if (it != components_.end()) {
		return dynamic_cast<Comp*>(it->second);
	}
	return nullptr;
}

template<ComponentType Comp>
inline void GameEntity::RemoveComponent() {
	std::string name = typeid(Comp).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}

	RemoveComponent(name);
}

/// json 変換
void to_json(nlohmann::json& _j, const GameEntity& _entity);
void from_json(const nlohmann::json& _j, GameEntity& _entity);