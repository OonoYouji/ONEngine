using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

static public class EntityCollection {

	/// entityのポインタを保持する変数
	static private Dictionary<int, Entity> entities_ = new Dictionary<int, Entity>();
	static private List<Entity> hierarchy_ = new List<Entity>();

	static private void EntityCallUpdateMethods() {
		/// リスト内のentitiesを更新する
		foreach (Entity entity in hierarchy_) {
			var scripts = entity.GetScripts();
			for (int i = 0; i < scripts.Count; ++i) {
				scripts[i].Update();
			}
		}	
	}


	static private void CalculationEntityHierarchy() {

		for (int i = 0; i < entities_.Count; ++i) {
			var entity = entities_[i];
			if (entity.parent) {
				continue;
			}
			
		}
		
	}

	static private void HierarchyPushBackChildren(Entity _entity) {
		// for(_entity.GetChild())
	}
	
	
	static public Entity GetEntity(int _id) {
		/// 既にコンテナないにあるかチェック
		if (entities_.ContainsKey(_id)) {
			return entities_[_id];
		}

		/// なかった場合一度c++側に確認、あったら新しくコンテナに加える
		if (InternalContainsEntity(_id)) {
			Debug.LogInfo("Entity found in C++ for ID: " + _id + ", adding to collection.");

			Entity entity = new Entity(_id);
			entities_.Add(_id, entity);
			return entity;
		}

		/// 最終チェック、デバッグ用のPrefabEntityではないかチェック
		if (InternalContainsPrefabEntity(_id)) {
			Debug.LogInfo("PrefabEntity found in C++ for ID: " + _id + ", adding to collection.");

			Entity entity = new Entity(_id);
			entities_.Add(_id, entity);
			return entity;
		}

		Debug.LogError("Entity not found for deletion: " + _id);

		/// それでも無かったらnullを返す
		return null;
	}

	static public Entity FindEntity(string _name) {
		/// entityの名前からEntityを探索する
		return GetEntity(InternalGetEntityId(_name));
	}

	static public Entity CreateEntity(string _prefabName) {
		int id = InternalCreateEntity(_prefabName);
		Debug.Log("Creating Entity: " + _prefabName + " with ID: " + id);
		if (id == 0) {
			Debug.LogError("Failed to create entity with prefab name: " + _prefabName);
			return null;
		}

		Entity entity = GetEntity(id);
		if (entity == null) {
			Debug.LogError("Failed to retrieve entity after creation: " + _prefabName + " with ID: " + id);
			return null;
		}

		return entity;
	}

	static public void DeleteEntityAll() {
		/// 全てのEntityを削除
		foreach (var entity in entities_.Values) {
			entity.Destroy();
		}
		entities_.Clear();
	}

	static public void DestroyEntity(int _entityId) {
		/// Entityを削除
		if (entities_.ContainsKey(_entityId)) {
			Debug.LogInfo("Destroying Entity ID: " + _entityId + " Name: " + entities_[_entityId].name);
			entities_[_entityId].Destroy();
			entities_.Remove(_entityId);
		} else {
			Debug.LogError("Entity not found for deletion: " + _entityId);
		}
	}

	static public void DestroyEntity(Entity _entity) {
		if (_entity == null) {
			Debug.LogError("Cannot destroy null entity.");
			return;
		}
		DestroyEntity(_entity.Id);
	}

	static public void DestroyEntity(string _name) {
		/// 名前からEntityを削除
		Entity entity = FindEntity(_name);
		if (entity != null) {
			DestroyEntity(entity.Id);
		} else {
			Debug.LogError("Entity not found for deletion by name: " + _name);
		}
	}


	static public int EntityCount() {
		return entities_.Count;
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalContainsEntity(int _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetEntityId(string _name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalCreateEntity(string _prefabName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalContainsPrefabEntity(int _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalDestroyEntity(int _entityId);

}
