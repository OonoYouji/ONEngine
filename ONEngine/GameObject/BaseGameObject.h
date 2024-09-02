#pragma once

#include <list>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>

#include <Transform.h>
//#include <BaseCollider.h>
#include <Model.h>


/*
#pragma once

#include <GameObjectManager.h>

class Player : public BaseGameObject {
public:

	/// コンストラクタ これは定形文なのでこれを基本使用する
	Player() : BaseGameObject(this) {
		/// 中身はすきに定義して ok
	}

	/// デストラクタ 中身はお好きに
	~Player(){}



	/// 初期化の関数 overrideしないとエラー吐くので注意
	void Initialize() override;

	/// 更新処理の関数 overrideしないとエラー吐くので注意
	void Update() override;


	/// すべての更新処理、当たり判定処理が終わったあとに呼び出される
	void LastUpdate() override;


	/// 描画系の override 
	void Draw() override;			 /// 3dモデルの描画
	void FrontSpriteDraw() override; /// 前景スプライト
	void BackSpriteDraw() override;	 /// 背景スプライト


	/// imguiのデバッグなどに使用して 
	/// Begin, Endは使用禁止
	void Debug() override {
		if(ImGui::TreeNode() {
			....
		}
	}

private:

};

*/



/// <summary>
/// すべてのゲームオブジェクトの基本となるクラス
/// </summary>
class BaseGameObject {
public:

	BaseGameObject(BaseGameObject* object);
	virtual ~BaseGameObject() = default;

public:

	virtual void Initialize() = 0;

	virtual void Update() = 0;
	virtual void LastUpdate() {}

	virtual void Draw() {};
	virtual void FrontSpriteDraw() {};
	virtual void BackSpriteDraw() {};

	virtual void Debug() {};

	void UpdateMatrix();


#pragma region Transform
	void SetPosX(float x) { transform_.position.x = x; }
	void SetPosY(float y) { transform_.position.y = y; }
	void SetPosZ(float z) { transform_.position.z = z; }
	void SetPos(const Vec3& v) { transform_.position = v; }

	void SetRotateX(float x) { transform_.rotate.x = x; }
	void SetRotateY(float y) { transform_.rotate.y = y; }
	void SetRotateZ(float z) { transform_.rotate.z = z; }
	void SetRotate(const Vec3& v) { transform_.rotate = v; }

	void SetScaleX(float x) { transform_.scale.x = x; }
	void SetScaleY(float y) { transform_.scale.y = y; }
	void SetScaleZ(float z) { transform_.scale.z = z; }
	void SetScale(const Vec3& v) { transform_.scale = v; }

	const Vec3 GetPosition() const;
	const Vec3 GetRotate() const { return transform_.rotate; }
	const Vec3 GetScale() const { return transform_.scale; }
	
	const Mat4& GetMatTransform() const { return transform_.matTransform; };
	const Transform& GetTransform() const { return transform_; }
#pragma endregion

#pragma region Parent Child
	void SetParent(BaseGameObject* parent);
	BaseGameObject* GetParent() const;
	void AddChild(BaseGameObject* child);
	const std::list<BaseGameObject*>& GetChilds() const;
#pragma endregion Parent Child

#pragma region Accesser Tag Name
	void SetTag(const std::string& tag);
	const std::string& GetTag() const;
	void SetName(const std::string& name);
	const std::string& GetName() const;
#pragma endregion Accesser Tag Name

#pragma region JSON 保存 読み込み
	/// ---------------------------------------------------
	/// 変数一個当たりの情報
	/// ---------------------------------------------------
	struct Item final {
		using Pointer = std::variant<int*, float*, Vector3*, bool*, std::string*>;	///- ポインタ
		using Value = std::variant<int, float, Vector3, bool, std::string>;		///- 実体
		std::pair<Pointer, Value> variable;
	};

	/// ---------------------------------------------------
	/// Itemの集合
	/// ---------------------------------------------------
	struct Group final {
		/// <summary>
		/// 値のsetter
		/// </summary>
		/// <param name="key">: itemsへのkey</param>
		/// <param name="value">: itemsへセットする値</param>
		template<typename T>
		void SetPtr(const std::string& key, T* ptr);

		template<typename T>
		void SetValue(const std::string& key, const T& value);

		/// <summary>
		/// 値のGetter
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		template<typename T>
		const T& GetItem(const std::string& key);

		/// <summary>
		/// デバッグ
		/// </summary>
		void ImGuiDebug();

		/// <summary>
		/// Itemの集合
		/// </summary>
		std::unordered_map<std::string, Item> items;
	};

	/// <summary>
	/// 
	/// </summary>
	/// <param name="groupName"></param>
	Group& CreateGroup(const std::string& groupName);

	/// <summary>
	/// srtのグループを作る
	/// </summary>
	void CreateWorldTransformGruop();
#pragma endregion JSON 保存 読み込み

#pragma region Collider 
	virtual void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision) {}
	virtual void OnCollisionStay([[maybe_unused]] BaseGameObject* const collision) {}
	virtual void OnCollisionExit([[maybe_unused]] BaseGameObject* const collision) {}

	void CreateBoxCollider(Model* model);

	//BaseCollider* GetCollider() const { return collider_.get(); }
#pragma endregion Collider


	void ImGuiDebug();

private:
	std::string tag_;	//- グループごとの名前 : Enemyなど
	std::string name_;	//- オブジェクトごとに違う名前 : Enemy1, Enemy2など
protected:

	Transform transform_;

	BaseGameObject* parent_ = nullptr;
	std::list<BaseGameObject*> childs_;

	std::unordered_map<std::string, Group> groups_;

	//std::unique_ptr<BaseCollider> collider_ = nullptr;

public:

	bool isActive = true;
	bool isDrawActive = true;

};