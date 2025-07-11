using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

static public class EntityCollection {

	static Dictionary<int, Entity> entities = new Dictionary<int, Entity>();

	static public Entity GetEntity(int _id) {
		/// 既にコンテナないにあるかチェック
		if (entities.ContainsKey(_id)) {
			return entities[_id];
		}

		/// なかった場合一度c++側に確認、あったら新しくコンテナに加える
		if (InternalContainsEntity(_id)) {
			Entity entity = new Entity(_id);
			entities.Add(_id, entity);
			return entity;
		}

		/// 最終チェック、デバッグ用のPrefabEntityではないかチェック
		if (InternalContainsPrefabEntity(_id)) {
			Entity entity = new Entity(_id);
			entities.Add(_id, entity);
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
		return GetEntity(id);
	}

	static public void DeleteEntityAll() {
		/// 全てのEntityを削除
		foreach (var entity in entities.Values) {
			entity.Destroy();
		}
		entities.Clear();
	}

	static public void DestroyEntity(int _entityId) {
		/// Entityを削除
		if (entities.ContainsKey(_entityId)) {
			Debug.LogInfo("Destroying Entity ID: " + _entityId + " Name: " + entities[_entityId].name);
			entities[_entityId].Destroy();
			entities.Remove(_entityId);
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
		return entities.Count;
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
