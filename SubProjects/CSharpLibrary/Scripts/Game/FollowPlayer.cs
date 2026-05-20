using System;

/// <summary>
/// ターゲット（主にプレイヤー）を追従するスクリプト
/// </summary>
public class FollowPlayer : MonoScript {
    [SerializeField] public string targetName = "Player";
    [SerializeField] public Vector3 offset = new Vector3(0, 5, -10);
    [SerializeField] public float smoothSpeed = 0.125f;

    private Entity target;

    public override void Initialize() {
        target = ecsGroup.FindEntity(targetName);
        if (target == null) {
            Debug.LogError($"FollowPlayer: Target entity '{targetName}' not found.");
        }
    }

    public override void Update() {
        if (target == null) return;

        Vector3 desiredPosition = target.transform.position + offset;
        // シンプルな線形補間（Lerp）で滑らかに移動
        Vector3 smoothedPosition = Vector3.Lerp(transform.position, desiredPosition, smoothSpeed);
        transform.position = smoothedPosition;

        // ターゲットの方を向く（オプション）
        // transform.rotate = Quaternion.LookAt(transform.position, target.transform.position, Vector3.up);
    }
}
