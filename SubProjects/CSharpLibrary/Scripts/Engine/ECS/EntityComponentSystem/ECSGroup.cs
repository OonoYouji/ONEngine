using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
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
		/// 引数の_idがすでに存在する場合はエラー
		if (entities_.ContainsKey(_id)) {
			// Debug.LogError("ECSGroup.AddEntity - Entity with ID: " + _id + " already exists in group: " + groupName);
			return;
		}


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
	public void AddScript(int _entityId, MonoScript _behavior, bool _enable) {
		Entity entity;
		if (entities_.TryGetValue(_entityId, out entity)) {
			Debug.LogInfo("ECSGroup.AddScript - Adding script to Entity ID: " + _entityId + ", Script Name: "
						  + _behavior.GetType().Name);
			_behavior.CreateBehavior(_entityId, _behavior.GetType().Name, this);
			_behavior.enable = _enable;
			entity.AddScript(_behavior);
		} else {
			Debug.LogError("Entity.AddScript - Entity not found with ID: " + _entityId);
		}
	}

	/// <summary>
	/// c#側から呼び出すエンティティの生成関数
	/// </summary>
	public Entity CreateEntity(string _prefabName) {
		Debug.LogInfo("ECSGroup.CreateEntity - Creating entity with prefab: " + _prefabName + ", Group Name: "
					  + groupName);

		int id = 0;
		InternalCreateEntity(out id, _prefabName, groupName);
		Entity entity = new Entity(id, this);
		entities_.Add(id, entity);

		awakeList_.Add(entity); //!< 生成されたエンティティを生成リストに追加
		initList_.Add(entity); //!< 初期化リストにも追加
		Debug.Log("ECSGroup.CreateEntity - AwakeListCount: " + awakeList_.Count + ", InitListCount: "
				  + initList_.Count);

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

		var sw = Stopwatch.StartNew();


		/// 生成、初期化の呼び出しを行う
		CallAwake();
		CallInitialize();

		foreach (Entity entity in entities_.Values) {
			foreach (Component comp in entity.GetComponents()) {
				comp.Begin();
			}
		}

		foreach (Entity entity in entities_.Values) {
			if (!CheckEnable(entity)) {
				continue;
			}

			foreach (MonoScript script in entity.GetScripts()) {
				if (script.enable) {
					script.Update();
				}
			}
		}

		foreach (Entity entity in entities_.Values) {
			foreach (Component comp in entity.GetComponents()) {
				comp.End();
			}
		}



		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.Log("ECSGroup.UpdateEntities - Updating entities in group: " + groupName + ", EntityCount: " + entities_.Count);
		Debug.Log($"gen0:{GC.CollectionCount(0)} gen1:{GC.CollectionCount(1)} gen2:{GC.CollectionCount(2)}");
		sw.Stop();
		double ms = sw.ElapsedTicks * 1000.0 / Stopwatch.Frequency;
		Debug.Log("Update Time (ms): " + ms);
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////");
	}


	/// <summary>
	/// 生成されたエンティティの生成関数の呼び出しを行う
	/// </summary>
	private void CallAwake() {
		if (awakeList_.Count == 0) {
			return;
		}

#if DEBUG
		Debug.Log("");
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.Log("ECSGroup.CallAwake - Awakening entities in group: " + groupName + ", Count: " + awakeList_.Count);
#endif

		List<Entity> entitiesToAwake = new List<Entity>(awakeList_);
		awakeList_.Clear(); // 生成リストをクリア
		foreach (Entity entity in entitiesToAwake) {
			foreach (MonoScript script in entity.GetScripts()) {
				script.Awake();
			}
		}

#if DEBUG
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.Log("");
#endif
	}


	/// <summary>
	/// 生成されたエンティティの初期化関数の呼び出しを行う
	/// </summary>
	private void CallInitialize() {
		if (initList_.Count == 0) {
			return;
		}

#if DEBUG
		Debug.Log("");
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.Log("ECSGroup.CallInitialize - Initializing entities in group: " + groupName + ", Count: "
				  + initList_.Count);
#endif

		List<Entity> entitiesToInitialize = new List<Entity>(initList_);
		initList_.Clear();
		foreach (Entity entity in entitiesToInitialize) {
			foreach (MonoScript script in entity.GetScripts()) {
				script.Initialize();
			}
		}

#if DEBUG
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.Log("");
#endif
	}

	/// <summary>
	/// Entityの取得
	/// </summary>
	public Entity GetEntity(int _id) {
		if (entities_.TryGetValue(_id, out Entity entity)) {
#if DEBUG
			Debug.Log("ECSGroup.GetEntity - Entity found with ID: " + entity.Id + ", Entity Name: " + entity.name);
#endif
			return entity;
		}

#if DEBUG
		Debug.LogError("ECSGroup.GetEntity - Entity not found with ID: " + _id + ", Group Name: " + groupName);
#endif
		return null;
	}

	/// <summary>
	/// エンティティの削除
	/// </summary>
	public void DestroyEntity(int _id) {
		if (entities_.TryGetValue(_id, out Entity entity)) {
			entities_.Remove(_id);
			InternalDestroyEntity(groupName, _id);
#if DEBUG
			Debug.Log("Entity destroyed with ID: " + _id);
		} else {
			Debug.LogError("Entity not found with ID: " + _id);
#endif
		}
	}

	/// <summary>
	/// すべてのエンティティを削除
	/// </summary>
	public void DeleteEntityAll() {
#if DEBUG
		Debug.Log("ECSGroup.DeleteEntityAll - Deleting all entities in group: " + groupName + ", EntityCount: "
				  + entities_.Count);
#endif

		var entitiesToDestroy = new Dictionary<int, Entity>(entities_);
		foreach (var entity in entitiesToDestroy.Values) {
			entity.Destroy();
		}
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

#if DEBUG
		Debug.LogError("Entity not found with name: " + _name);
#endif
		return null;
	}


	bool CheckEnable(Entity _entity) {
		if (!_entity) {
			return false;
		}

		if (!_entity.enable) {
			return false;
		}

		Entity parent = _entity.parent;
		if (parent) {
			return CheckEnable(parent);
		}

		return true;
	}


	///////////////////////////////////////////////////////////////////////////////////////////
	// internal methods
	///////////////////////////////////////////////////////////////////////////////////////////

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalCreateEntity(out int _id, string _prefabName, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalDestroyEntity(string _ecsGroupName, int _id);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetEnable();

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalFindEntity(string _ecsGroupName, string _entityName);


	///////////////////////////////////////////////////////////////////////////////////////////
	// operators
	///////////////////////////////////////////////////////////////////////////////////////////

	public static implicit operator bool(ECSGroup _group) {
		return _group != null;
	}
}