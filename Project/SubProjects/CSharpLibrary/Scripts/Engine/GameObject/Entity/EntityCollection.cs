using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

static public class EntityCollection {

	static Dictionary<uint, Entity> entities = new Dictionary<uint, Entity>();

	static public Entity GetEntity(uint _id) {
		/// 既にコンテナないにあるかチェック
		if(entities.ContainsKey(_id)) {
			return entities[_id];
		}

		/// なかった場合一度c++側に確認、あったら新しくコンテナに加える
		if(InternalContainsEntity(_id)) {
			Entity entity = new Entity(_id);
			entities.Add(_id, entity);
			return entity;
		}

		/// それでも無かったらnullを返す
		return null;
	}

	static public Entity FindEntity(string _name) {
		/// entityの名前からEntityを探索する
		return GetEntity(InternalGetEntityId(_name));
	}

	static public Entity CreateEntity(string _prefabName) {
		uint id = InternalCreateEntity(_prefabName);
		return GetEntity(id);
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalContainsEntity(uint _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern uint InternalGetEntityId(string _name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern uint InternalCreateEntity(string _prefabName);


}
