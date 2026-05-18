using System;
using ONEngine.Scripting;
using ONEngine.Scripting.Generated;
using ONEngine.Scripting.Math; // Math namespace を追加

namespace Scripts {
	public class RotatingCube : GameScript {
		[SerializeField]
		private float Speed = 2.0f; // これがInspectorに出現します

		public int TestCount = 10;  // publicも自動で出現します

		public override void Update(float deltaTime) {
            // 現在のクォータニオンをコピーして変更を加える
            Quaternion currentRotation = transformRef.rotation;

            // Y軸周りの回転クォータニオンを作成
            // オイラー角 (Y軸のみ) -> クォータニオン
            // deltaTime * Speed はラジアン値として扱う
            float angle = deltaTime * Speed; // ラジアン
            
            // Y軸回転のクォータニオンを計算
            Quaternion deltaRotation = new Quaternion(0, (float)System.Math.Sin(angle / 2.0f), 0, (float)System.Math.Cos(angle / 2.0f));

            // 現在の回転に新しい回転を適用 (乗算順序に注意: newRotation * oldRotation)      
            transformRef.rotation = deltaRotation * currentRotation;

            // 正規化 (TransformSystemでも行われるが、念のため)
            transformRef.rotation = Quaternion.Normalize(transformRef.rotation);
		}
	}
}


