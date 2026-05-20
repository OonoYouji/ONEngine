using System;

public static class ObjectUtils {
    /// <summary>
    /// プレハブからエンティティを生成します。
    /// </summary>
    public static Entity Instantiate(string prefabName, string groupName = "Main") {
        ECSGroup group = EntityComponentSystem.GetECSGroup(groupName);
        if (group == null) {
            Debug.LogError($"ECSGroup '{groupName}' not found.");
            return null;
        }
        return group.CreateEntity(prefabName);
    }

    /// <summary>
    /// プレハブからエンティティを指定した座標・回転で生成します。
    /// </summary>
    public static Entity Instantiate(string prefabName, Vector3 position, Quaternion rotation, string groupName = "Main") {
        Entity entity = Instantiate(prefabName, groupName);
        if (entity != null) {
            entity.transform.position = position;
            entity.transform.rotate = rotation;
        }
        return entity;
    }

    /// <summary>
    /// 指定した名前のエンティティを検索します。
    /// </summary>
    public static Entity Find(string name, string groupName = "Main") {
        ECSGroup group = EntityComponentSystem.GetECSGroup(groupName);
        if (group == null) return null;
        return group.FindEntity(name);
    }
}
