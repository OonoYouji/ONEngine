using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzlePlayer : MonoBehavior {
	public PuzzleBlockData blockData; // ブロックデータ

	public override void Initialize() {
		// 初期化処理
	}

	public override void Update() {
		if (Input.TriggerGamepad(Gamepad.A)) {
			if (blockData.type == (int)BlockType.Black) {
				blockData.type = (int)BlockType.White;
			} else {
				blockData.type = (int)BlockType.Black;
			}
		}
		
		UpdateColor();
	}

	public void Move(Vector2Int _moveDir) {
		blockData.address += _moveDir;
	}

	public void UpdatePosition() {
		/// 座標更新
		Transform t = transform;
		if (t != null) {
			t.position = new Vector3(blockData.address.x * blockData.blockSpace, blockData.height,
				blockData.address.y * blockData.blockSpace);
		}
	}

	public void UpdateColor() {
		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr) {
			Vector4 color = Vector4.one;
			if (blockData.type == (int)BlockType.Black) {
				float value = 0.2f;
				color = new Vector4(value, value, value, 1);
			}

			mr.color = color;
		}
	}

	public void UpdateRotateY(Vector2Int _moveDir) {
		/// 移動方向に合わせて向きを変更する、モデルの正面は z+ 方向

		float rotateY = 0f;
		if (_moveDir.y < 0) {
			/// 上
			rotateY = Mathf.PI;
		} else if (_moveDir.y > 0) {
			/// 下
			rotateY = 0f;
		} else if (_moveDir.x < 0f) {
			/// 左
			rotateY = Mathf.PI * 1.5f;
		} else if (_moveDir.x > 0f) {
			/// 右
			rotateY = Mathf.PI * 0.5f;
		}

		Quaternion rotate = transform.rotate;
		rotate = Quaternion.MakeFromAxis(Vector3.up, rotateY);
		transform.rotate = rotate;

	}
}