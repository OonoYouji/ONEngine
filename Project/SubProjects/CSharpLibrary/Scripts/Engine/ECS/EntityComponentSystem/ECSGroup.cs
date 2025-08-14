
using System.Collections.Generic;
using System.Runtime.CompilerServices;

public class ECSGroup {
	///////////////////////////////////////////////////////////////////////////////////////////
	// objects
	///////////////////////////////////////////////////////////////////////////////////////////
	
	private string groupName_;
	private bool enable_; //!< このGroupの有効/無効フラグ
	private Dictionary<int, Entity> entities_;
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// methods
	///////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_groupName"></param>
	public ECSGroup(string _groupName) {
		groupName_ = _groupName;
		enable_ = true;
	}

	// ==============================================
	// エンティティの生成
	// ==============================================

	/// <summary>
	/// c/c++側から呼び出すエンティティの追加関数
	/// </summary>
	private void AddEntity(int _id) {
		Entity entity = new Entity(_id, groupName_);
		entities_.Add(_id, entity);
	}

	/// <summary>
	/// c#側から呼び出すエンティティの生成関数
	/// </summary>
	public Entity CreateEntity(string _prefabName) {
		int id = 0;
		InternalCreateEntity(out id, _prefabName);
		Entity entity = new Entity(id, groupName_);
		entities_.Add(id, entity);
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
		
		foreach (Entity entity in entities_.Values) {
			foreach (MonoBehavior script in entity.GetScripts()) {
				script.Update();
			}
		}
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// internal methods
	///////////////////////////////////////////////////////////////////////////////////////////

	[MethodImpl(MethodImplOptions.Synchronized)]
	static extern void InternalCreateEntity(out int _id, string _prefabName);
}