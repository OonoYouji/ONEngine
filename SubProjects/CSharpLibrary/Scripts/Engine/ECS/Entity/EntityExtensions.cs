using System;

public static class EntityExtensions {
    /// <summary>
    /// コンポーネントを取得し、存在しない場合は新しく追加して返します。
    /// </summary>
    public static T GetOrAddComponent<T>(this Entity entity) where T : Component {
        T comp = entity.GetComponent<T>();
        if (comp == null) {
            comp = entity.AddComponent<T>();
        }
        return comp;
    }

    /// <summary>
    /// 指定したコンポーネントを保持しているか確認します。
    /// </summary>
    public static bool HasComponent<T>(this Entity entity) where T : Component {
        return entity.GetComponent<T>() != null;
    }

    /// <summary>
    /// 子要素を再帰的に検索し、指定した名前のエンティティを返します。
    /// </summary>
    public static Entity FindChildRecursive(this Entity entity, string name) {
        uint childCount = entity.GetChildCount();
        for (uint i = 0; i < childCount; i++) {
            Entity child = entity.GetChild(i);
            if (child != null) {
                if (child.name == name) {
                    return child;
                }
                Entity found = child.FindChildRecursive(name);
                if (found != null) {
                    return found;
                }
            }
        }
        return null;
    }

    /// <summary>
    /// 全ての子要素を一括で削除します。
    /// </summary>
    public static void DestroyAllChildren(this Entity entity) {
        uint childCount = entity.GetChildCount();
        // 削除すると個数が変わる可能性があるため、後ろから回すか、リストにコピーして回す
        // 現状のGetChild(index)の実装に依存するが、一般的に安全なのは後ろから
        for (int i = (int)childCount - 1; i >= 0; i--) {
            Entity child = entity.GetChild((uint)i);
            if (child != null) {
                child.Destroy();
            }
        }
    }
}
