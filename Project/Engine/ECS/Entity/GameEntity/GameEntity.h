#pragma once

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/ECS/Component/Collection/ComponentHash.h"

namespace ONEngine {

/// ////////////////////////////////////////////////////
/// エンティティインターフェース
/// ////////////////////////////////////////////////////
/**
 * @class GameEntity
 * @brief ゲーム世界内のあらゆる実体（Entity）を表すクラス。親子関係ツリーの構築、コンポーネントのアタッチ、ワールド変換行列（Transform）へのアクセスなどを仲介します。
 */
class GameEntity {
	friend class EntityComponentSystem;
	friend class EntityCollection;
	friend class SceneIO;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	GameEntity();

	/**
	 * @brief デストラクタ
	 */
	~GameEntity();

	/**
	 * @brief エンティティの初期生成時（Awake）にコールされるコールバック。
	 */
	void Awake();


	/// --------------------------------------------------
	/// component 関連
	/// --------------------------------------------------

	/**
	 * @brief テンプレート引数で指定されたコンポーネント（Comp）を新規生成して追加します。
	 * @tparam Comp 追加するコンポーネントの型
	 * @return 追加されたコンポーネントのポインタ
	 */
	template <IsComponent Comp>
	Comp* AddComponent();

	/**
	 * @brief コンポーネント名の文字列を指定して、新規コンポーネントを追加します。
	 * @param _name コンポーネントのクラス名
	 * @return 追加されたコンポーネントの基底（IComponent）ポインタ
	 */
	IComponent* AddComponent(const std::string& _name);

	/**
	 * @brief 指定された型のコンポーネントを取得します。
	 * @tparam Comp 取得したいコンポーネントの型
	 * @return コンポーネントのポインタ。存在しない場合は nullptr
	 */
	template <IsComponent Comp>
	Comp* GetComponent() const;

	/**
	 * @brief コンポーネント名文字列を指定して、コンポーネントを取得します。
	 * @param _compName コンポーネント名
	 * @return コンポーネントの基底（IComponent）ポインタ
	 */
	IComponent* GetComponent(const std::string& _compName) const;

	/**
	 * @brief 指定された型のコンポーネントを削除します。
	 * @tparam Comp 削除対象のコンポーネント型
	 */
	template <IsComponent Comp>
	void RemoveComponent();

	/**
	 * @brief コンポーネント名文字列を指定して、コンポーネントを削除します。
	 * @param _compName コンポーネント名
	 */
	void RemoveComponent(const std::string& _compName);

	/**
	 * @brief このエンティティにアタッチされているすべてのコンポーネントを削除します。
	 */
	void RemoveComponentAll();

	/**
	 * @brief アタッチされている全コンポーネントのハッシュマップ（読み取り専用）を取得します。
	 * @return ハッシュとコンポーネントポインタ的マップ参照
	 */
	const std::unordered_map<size_t, IComponent*>& GetComponents() const;

	/**
	 * @brief アタッチされている全コンポーネントのハッシュマップを取得します。
	 * @return ハッシュとコンポーネントポインタのマップ参照
	 */
	std::unordered_map<size_t, IComponent*>& GetComponents();


	/// --------------------------------------------------
	/// transform 関連
	/// --------------------------------------------------

	/**
	 * @brief トランスフォームのワールド行列・親子関係を再計算・更新します。
	 */
	void UpdateTransform();

	/**
	 * @brief ローカル位置座標を設定します。
	 * @param _v 3次元座標（Vector3）
	 */
	void SetPosition(const Vector3& _v);

	/**
	 * @brief オイラー角を指定してローカル回転を設定します。
	 * @param _v オイラー角（度数法）
	 */
	void SetRotate(const Vector3& _v);

	/**
	 * @brief クォータニオンを指定してローカル回転を設定します。
	 * @param _q クォータニオン回転値
	 */
	void SetRotate(const Quaternion& _q);

	/**
	 * @brief ローカルスケール（拡大縮小）を設定します。
	 * @param _v 拡大縮小値
	 */
	void SetScale(const Vector3& _v);

	/**
	 * @brief ローカル位置座標を取得します。
	 * @return ローカル位置座標
	 */
	const Vector3& GetLocalPosition() const;

	/**
	 * @brief オイラー角によるローカル回転値を取得します。
	 * @return オイラー角
	 */
	Vector3 GetLocalRotate() const;

	/**
	 * @brief クォータニオンによるローカル回転値を取得します。
	 * @return クォータニオン回転値の参照
	 */
	const Quaternion& GetLocalRotateQuaternion() const;

	/**
	 * @brief ローカルスケール値を取得します。
	 * @return 拡大縮小値の参照
	 */
	const Vector3& GetLocalScale() const;


	/**
	 * @brief ワールド位置座標を取得します（親の変換行列を再帰的に考慮）。
	 * @return ワールド位置座標
	 */
	Vector3 GetPosition();

	/**
	 * @brief オイラー角によるワールド回転値を取得します。
	 * @return オイラー角
	 */
	Vector3 GetRotate();

	/**
	 * @brief クォータニオンによるワールド回転値を取得します。
	 * @return クォータニオン
	 */
	Quaternion GetRotateQuaternion();

	/**
	 * @brief ワールドスケール値を取得します。
	 * @return ワールドスケール
	 */
	Vector3 GetScale();

	/**
	 * @brief このエンティティの基本変形情報（Transformコンポーネント）を取得します。
	 * @return Transformコンポーネントポインタ
	 */
	Transform* GetTransform() const;


	/// --------------------------------------------------
	/// parent - children 関連
	/// --------------------------------------------------

	/**
	 * @brief 指定されたエンティティをこのエンティティの親（Parent）として設定し、トランスフォームの階層構造を構築します。
	 * @param _parent 親とするエンティティ
	 */
	void SetParent(GameEntity* _parent);

	/**
	 * @brief 現在アタッチされている親エンティティとの親子関係を解除します。
	 */
	void RemoveParent();

	/**
	 * @brief 現在の親エンティティ（読み取り専用）を取得します。
	 * @return 親エンティティへのポインタ。親がいない場合は nullptr
	 */
	const GameEntity* GetParent() const;

	/**
	 * @brief 現在の親エンティティを取得します。
	 * @return 親エンティティへのポインタ
	 */
	GameEntity* GetParent();


	/**
	 * @brief 指定された子エンティティをリストから切り離し（親子関係を解除）、成功したかどうかを返します。
	 * @param _child 切り離したい子エンティティ
	 * @return 成功なら true、失敗なら false
	 */
	bool RemoveChild(GameEntity* _child);

	/**
	 * @brief 直属の子エンティティ群のリスト（読み取り専用）を取得します。
	 * @return 子エンティティへの生ポインタ配列の参照
	 */
	const std::vector<GameEntity*>& GetChildren() const;

	/**
	 * @brief インデックスを指定して直属の子エンティティを1つ取得します。
	 * @param _index 配列内インデックス
	 * @return 子エンティティポインタ
	 */
	GameEntity* GetChild(size_t _index);

	/**
	 * @brief 子エンティティリスト内での順番を並び替えます（ヒエラルキーの順序等）。
	 * @param _child 対象の子エンティティポインタ
	 * @param _newIndex 新しい並び順インデックス
	 */
	void MoveChild(GameEntity* _child, size_t _newIndex);


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


	/**
	 * @brief ランタイム/非ランタイムを区別する一意なエンティティIDを取得します。
	 * @return エンティティID（id > 0 の場合は非ランタイム/エディタ生成、id < 0 の場合はランタイム時生成）
	 */
	int32_t GetId() const;

	/**
	 * @brief エンティティの一意なGUIDを取得します。
	 * @return GUIDオブジェクトの参照
	 */
	const Guid& GetGuid() const;

	/**
	 * @brief このエンティティが所属するECSGroup（シーン管理ハブ）を取得します。
	 * @return ECSGroupポインタ
	 */
	class ECSGroup* GetECSGroup() const;



	/// --------------------------------------------------
	/// this entity methods
	/// --------------------------------------------------

	/**
	 * @brief このエンティティおよびアタッチされている全コンポーネントを破棄予約し、コレクションから削除します。
	 */
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

template<IsComponent Comp>
inline Comp* GameEntity::AddComponent() {
	const std::string name = GetComponentTypeName<Comp>();
	return static_cast<Comp*>(AddComponent(name));
}

template<IsComponent Comp>
inline Comp* GameEntity::GetComponent() const {
	auto it = components_.find(GetComponentHash<Comp>());
	if (it != components_.end()) {
		return dynamic_cast<Comp*>(it->second);
	}
	return nullptr;
}

template<IsComponent Comp>
inline void GameEntity::RemoveComponent() {
	const std::string name = GetComponentTypeName<Comp>();
	RemoveComponent(name);
}

/// json 変換
void to_json(nlohmann::json& _j, const GameEntity& _entity);
void from_json(const nlohmann::json& _j, GameEntity& _entity);

} /// ONEngine
