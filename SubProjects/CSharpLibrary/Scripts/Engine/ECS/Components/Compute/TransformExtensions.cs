using System;

public static class TransformExtensions {
    /// <summary>
    /// 対象との距離を計算します。
    /// </summary>
    public static float GetDistanceTo(this Transform transform, Transform target) {
        if (target == null) return 0f;
        return Vector3.Distance(transform.position, target.position);
    }

    /// <summary>
    /// 対象との距離を計算します。
    /// </summary>
    public static float GetDistanceTo(this Transform transform, Vector3 targetPosition) {
        return Vector3.Distance(transform.position, targetPosition);
    }

    /// <summary>
    /// X座標のみを設定します。
    /// </summary>
    public static void SetPositionX(this Transform transform, float x) {
        Vector3 pos = transform.position;
        pos.x = x;
        transform.position = pos;
    }

    /// <summary>
    /// Y座標のみを設定します。
    /// </summary>
    public static void SetPositionY(this Transform transform, float y) {
        Vector3 pos = transform.position;
        pos.y = y;
        transform.position = pos;
    }

    /// <summary>
    /// Z座標のみを設定します。
    /// </summary>
    public static void SetPositionZ(this Transform transform, float z) {
        Vector3 pos = transform.position;
        pos.z = z;
        transform.position = pos;
    }

    /// <summary>
    /// Xスケールのみを設定します。
    /// </summary>
    public static void SetScaleX(this Transform transform, float x) {
        Vector3 scale = transform.scale;
        scale.x = x;
        transform.scale = scale;
    }

    /// <summary>
    /// Yスケールのみを設定します。
    /// </summary>
    public static void SetScaleY(this Transform transform, float y) {
        Vector3 scale = transform.scale;
        scale.y = y;
        transform.scale = scale;
    }

    /// <summary>
    /// Zスケールのみを設定します。
    /// </summary>
    public static void SetScaleZ(this Transform transform, float z) {
        Vector3 scale = transform.scale;
        scale.z = z;
        transform.scale = scale;
    }
}
