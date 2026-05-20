using System;

/// <summary>
/// オブジェクトを回転させるシンプルなスクリプト
/// </summary>
public class Rotator : MonoScript {
    [SerializeField] public Vector3 rotationSpeed = new Vector3(0, 100, 0);

    public override void Update() {
        // 回転角を更新 (度数法からラジアンに変換してクォータニオンを作成)
        Vector3 rotationDelta = rotationSpeed * Time.deltaTime;
        
        // 現在の回転に加算
        Quaternion deltaRotation = Quaternion.FromEuler(new Vector3(
            rotationDelta.x * Mathf.Deg2Rad, 
            rotationDelta.y * Mathf.Deg2Rad, 
            rotationDelta.z * Mathf.Deg2Rad
        ));
        
        transform.rotate = transform.rotate * deltaRotation;
    }
}
