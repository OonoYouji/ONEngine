using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

static public class EntityCollection {

	static Dictionary<uint, Entity> entities = new Dictionary<uint, Entity>();

	static public Entity GetEntity(uint id) {
		/// 既にコンテナないにあるかチェック
		if(entities.ContainsKey(id)) {
			return entities[id];
		}

		/// なかった場合一度c++側に確認、あったら新しくコンテナに加える
		if(InternalContainsEntity(id)) {
			Entity entity = new Entity(id);
			entities.Add(id, entity);
			return entity;
		}

		/// それでも無かったらnullを返す
		return null;
	}



	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalContainsEntity(uint _entityId);

}
