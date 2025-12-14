

using System.Collections.Generic;

static public class EntityComponentSystem {
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// objects
	///////////////////////////////////////////////////////////////////////////////////////////
	
	static private Dictionary<string, ECSGroup> groups = new Dictionary<string, ECSGroup>();
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// methods
	///////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 新規グループの追加
	/// </summary>
	static public ECSGroup AddECSGroup(string _name) {
		ECSGroup group = new ECSGroup(_name);
		groups.Add(_name, group);
		Debug.LogInfo("EntityComponentSystem.AddECSGroup - added: " + group.groupName + "  GroupCount " + groups.Count);
		return group;
	}

	/// <summary>
	/// ECSGroupの取得
	/// </summary>
	static public ECSGroup GetECSGroup(string _name) {
#if DEBUG
		Debug.LogInfo("EntityComponentSystem.GetECSGroup - Getting ECSGroup: " + _name + "  GroupCount " + groups.Count);
#endif

		if (groups.TryGetValue(_name, out ECSGroup group)) {
			return group;
		} else {
#if DEBUG
			Debug.LogError("EntityComponentSystem.GetECSGroup - ECSGroup not found: " + _name + "  GroupCount " + groups.Count);
			foreach (var ecsGroup in groups) {
				Debug.LogError("Available ECSGroups: " + ecsGroup.Key);
			}
#endif
			return null;
		}
	}

	
	/// <summary>
	/// すべてのGroupのエンティティを削除する
	/// </summary>
	static public void DeleteEntityAll() {
#if DEBUG
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.Log("EntityComponentSystem.DeleteEntityAll - Deleting all entities from all ECSGroups. GroupCount: " + groups.Count);
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.LogInfo("All entities deleted from all ECSGroups.");
#endif

		foreach (var group in groups.Values) {
			group.DeleteEntityAll();
		}
	}
	

	static public Entity GetEntity(string _groupName, int _id) {
#if DEBUG
		Debug.LogInfo("EntityComponentSystem.GetEntity - Getting Entity from group: " + _groupName + ", ID: " + _id);
		Debug.LogInfo("EntityComponentSystem.GetEntity - GroupCount: " + groups.Count);
		foreach (var g in groups) {
			Debug.LogInfo("EntityComponentSystem.GetEntity - Available ECSGroup: " + g.Key);
		}
#endif


		if (groups.TryGetValue(_groupName, out ECSGroup group)) {
#if DEBUG
			Debug.LogInfo("EntityComponentSystem.GetEntity - ECSGroup found: " + group.groupName);
#endif
			return group.GetEntity(_id);
		} else {
#if DEBUG
			Debug.LogError("EntityComponentSystem.GetEntity - ECSGroup not found: " + _groupName);
#endif
			return null;
		}
	}

	static public MonoScript GetMonoBehavior(string _groupName, int _entityId, string _scriptName) {
#if DEBUG
		Debug.LogInfo("EntityComponentSystem.GetMonoBehavior - Getting MonoBehavior from group: " + _groupName + ", Entity ID: " + _entityId + ", Script Name: " + _scriptName);
		Debug.LogInfo("EntityComponentSystem.GetEntity - GroupCount: " + groups.Count);
#endif
		
		if (groups.TryGetValue(_groupName, out ECSGroup group)) {
			Entity entity = group.GetEntity(_entityId);
			if (entity != null) {
				return entity.GetScript(_scriptName);
			} else {
#if DEBUG
				Debug.LogError("EntityComponentSystem.GetMonoBehavior - Entity not found with ID: " + _entityId);
#endif
				return null;
			}
		} else {
#if DEBUG
			Debug.LogError("EntityComponentSystem.GetMonoBehavior - ECSGroup not found: " + _groupName);
#endif
			return null;
		}
	}

}