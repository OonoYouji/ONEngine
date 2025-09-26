using Newtonsoft.Json.Linq;
using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class MonoBehavior {
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// objects
    ///////////////////////////////////////////////////////////////////////////////////////////

    /// Behaviorの生成
    public void CreateBehavior(int _entityId, string _name, ECSGroup _ecsGroup) {
        if (!_ecsGroup) {
            Debug.LogError("MonoBehavior.CreateBehavior - ECSGroup is null. Cannot create MonoBehavior for Entity ID: " + _entityId);
            return;
        }

        name_ = _name;
        ecsGroup = _ecsGroup;
        entity = ecsGroup.GetEntity(_entityId);

        Debug.Log("MonoBehavior created for Entity ID: " + _entityId + ", Script Name: " + _name + ", Group Name: " + _ecsGroup.groupName);
    }


    /// この behavior が所属するECSGroup
    public ECSGroup ecsGroup {
        get; internal set;
    }

    private string name_;
    public bool enable;

    public Entity entity {
        get; internal set;
    }

    public Transform transform {
        get {
            if (entity == null) {
                Debug.LogError("MonoBehavior.transform - Entity is not initialized. Please call InternalInitialize first.");
                return null;
            }

            if (entity.transform == null) {
                Debug.LogError("MonoBehavior.transform - Transform component is not initialized for Entity ID: " + entity.Id);
                return null;
            }

            return entity.transform;
        }
    }



    ///////////////////////////////////////////////////////////////////////////////////////////
    /// methods
    ///////////////////////////////////////////////////////////////////////////////////////////

    public virtual void Awake() { }
    public virtual void Initialize() { }
    public virtual void Update() { }

    public virtual void OnCollisionEnter(Entity collision) { }
    public virtual void OnCollisionExit(Entity collision) { }
    public virtual void OnCollisionStay(Entity collision) { }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// operators
    ///////////////////////////////////////////////////////////////////////////////////////////
    public static implicit operator bool(MonoBehavior _monoBehavior) {
        return _monoBehavior != null;
    }

}
