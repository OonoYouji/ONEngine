

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
		Debug.LogInfo("ECSGroup.AddECSGroup - added: " + group.groupName + "  GroupCount " + groups.Count);
		return group;
	}

	/// <summary>
	/// ECSGroupの取得
	/// </summary>
	static public ECSGroup GetECSGroup(string _name) {
		Debug.LogInfo("ECSGroup.GetECSGroup - Getting ECSGroup: " + _name + "  GroupCount " + groups.Count);

		if (groups.TryGetValue(_name, out ECSGroup group)) {
			return group;
		} else {
			Debug.LogError("ECSGroup.GetECSGroup - ECSGroup not found: " + _name + "  GroupCount " + groups.Count);
			foreach (var ecsGroup in groups) {
				Debug.LogError("Available ECSGroups: " + ecsGroup.Key);
			}
			return null;
		}
	}

	
	/// <summary>
	/// すべてのGroupのエンティティを削除する
	/// </summary>
	static public void DeleteEntityAll() {
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////////////////////");
		Debug.Log("EntityComponentSystem.DeleteEntityAll - Deleting all entities from all ECSGroups. GroupCount: " + groups.Count);
		Debug.Log("//////////////////////////////////////////////////////////////////////////////////////////////////////////////////");

		foreach (var group in groups.Values) {
			group.DeleteEntityAll();
		}

		groups.Clear();
		Debug.LogInfo("All entities deleted from all ECSGroups.");

	}

}