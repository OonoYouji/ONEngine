
using System.Collections.Generic;
using System.Runtime.CompilerServices;

public class ECSGroup {
	///////////////////////////////////////////////////////////////////////////////////////////
	// objects
	///////////////////////////////////////////////////////////////////////////////////////////

	public string groupName;
	private bool enable_; //!< このGroupの有効/無効フラグ
	private Dictionary<int, Entity> entities_ = new Dictionary<int, Entity>();

	/// 生成処理、初期化処理の呼び出し用リスト
	private List<Entity> awakeList_ = new List<Entity>();
	private List<Entity> initList_ = new List<Entity>();


	///////////////////////////////////////////////////////////////////////////////////////////
	// methods
	///////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_groupName"></param>
	public ECSGroup(string _groupName) {
		groupName = _groupName;
		enable_ = true;
	}

	// ==============================================
	// エンティティの生成
	// ==============================================

	/// <summary>
	/// c/c++側から呼び出すエンティティの追加関数
	/// </summary>
	public void AddEntity(int _id) {
		Debug.LogInfo("ECSGroup.AddEntity - Adding entity with ID: " + _id + ", Group Name: " + groupName);

		Entity entity = new Entity(_id, this);
		entities_.Add(_id, entity);

		/// 生成、初期化の呼び出し用リストに追加
		awakeList_.Add(entity);
		initList_.Add(entity);
	}

	/// <summary>
	/// C/C++側から呼び出すコンポーネントの追加関数
	/// </summary>
	public void AddScript(int _entityId, MonoBehavior _behavior) {
		Entity entity;
		if (entities_.TryGetValue(_entityId, out entity)) {
			Debug.LogInfo("ECSGroup.AddScript - Adding script to Entity ID: " + _entityId + ", Script Name: " + _behavior.GetType().Name);
			entity.AddScript(_behavior);
			_behavior.CreateBehavior(_entityId, _behavior.GetType().Name, this);
			Debug.Log("Script added to Entity ID: " + _entityId + ", Script Name: " + _behavior.GetType().Name);
		} else {
			Debug.LogError("Entity.AddScript - Entity not found with ID: " + _entityId);
		}
	}

	/// <summary>
	/// c#側から呼び出すエンティティの生成関数
	/// </summary>
	public Entity CreateEntity(string _prefabName) {
		Debug.LogInfo("ECSGroup.CreateEntity - Creating entity with prefab: " + _prefabName + ", Group Name: " + groupName);

		int id = 0;
		InternalCreateEntity(out id, _prefabName, groupName);
		Entity entity = new Entity(id, this);
		entities_.Add(id, entity);

		/// 生成、初期化の呼び出しを行う
		CallAwakeAndInitialize(entity);
		return entity;
	}

	// ==============================================
	// 更新
	// ==============================================

	/// <summary>
	/// リストないのエンティティの更新処理を呼ぶ
	/// </summary>
	public void UpdateEntities() {
		//!< 無効なら処理しない
		if (!enable_) {
			return;
		}

		/// 生成、初期化の呼び出しを行う
		CallAwake();
		CallInitialize();

		foreach (Entity entity in entities_.Values) {
			foreach (MonoBehavior script in entity.GetScripts()) {
				script.Update();
			}
			Debug.Log("EntityUpdate ScriptCount: " + entity.GetScripts().Count + ", Entity ID: " + entity.Id);
		}

		Debug.Log("ECSGroup: " + groupName + " updated. Entity count: " + entities_.Count);

	}


	/// <summary>
	/// 生成されたエンティティの生成関数の呼び出しを行う
	/// </summary>
	private void CallAwake() {
		foreach (Entity entity in awakeList_) {
			foreach (MonoBehavior script in entity.GetScripts()) {
				script.Awake();
				Debug.Log("Awake called for script: " + script.GetType().Name + " on Entity ID: " + entity.Id);
			}
		}
		Debug.Log("All entities awakened in group: " + groupName + "  count=" + awakeList_.Count);
		awakeList_.Clear();
	}


	/// <summary>
	/// 生成されたエンティティの初期化関数の呼び出しを行う
	/// </summary>
	private void CallInitialize() {
		foreach (Entity entity in initList_) {
			foreach (MonoBehavior script in entity.GetScripts()) {
				script.Initialize();
				Debug.Log("Initialize called for script: " + script.GetType().Name + " on Entity ID: " + entity.Id);
			}
		}
		Debug.Log("All entities initialized in group: " + groupName + "  count=" + initList_.Count);
		initList_.Clear();
	}


	/// <summary>
	/// 引数のエンティティのスクリプトのAwakeとInitializeを呼び出す
	/// </summary>
	private void CallAwakeAndInitialize(Entity _entity) {
		foreach (MonoBehavior script in _entity.GetScripts()) {
			script.Awake();
			script.Initialize();
		}
	}


	/// <summary>
	/// Entityの取得
	/// </summary>
	public Entity GetEntity(int _id) {
		if (entities_.TryGetValue(_id, out Entity entity)) {
			return entity;
		}
		Debug.LogError("Entity not found with ID: " + _id);
		return null;
	}


	/// <summary>
	/// エンティティの削除
	/// </summary>
	public void DestroyEntity(int _id) {
		if (entities_.TryGetValue(_id, out Entity entity)) {
			entity.Destroy();
			entities_.Remove(_id);
			Debug.Log("Entity destroyed with ID: " + _id);
		} else {
			Debug.LogError("Entity not found with ID: " + _id);
		}
	}

	/// <summary>
	/// すべてのエンティティを削除
	/// </summary>
	public void DeleteEntityAll() {
		foreach (var entity in entities_.Values) {
			entity.Destroy();
		}

		entities_.Clear();
	}

	/// <summary>
	/// エンティティの探索
	/// </summary>
	public Entity FindEntity(string _name) {
		foreach (var entity in entities_.Values) {
			if (entity.name == _name) {
				return entity;
			}
		}
		Debug.LogError("Entity not found with name: " + _name);
		return null;
	}



	///////////////////////////////////////////////////////////////////////////////////////////
	// internal methods
	///////////////////////////////////////////////////////////////////////////////////////////

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalCreateEntity(out int _id, string _prefabName, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalDestroyEntity(int _id);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetEnable();

	///////////////////////////////////////////////////////////////////////////////////////////
	// operators
	///////////////////////////////////////////////////////////////////////////////////////////

	public static implicit operator bool(ECSGroup _group) {
		return _group != null;
	}

}