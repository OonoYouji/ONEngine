using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzlePlayer : MonoBehavior {
	public PuzzleBlockData blockData; // ブロックデータ
	public Flag isGoaled;


	public override void Initialize() {
		// 初期化処理
		isGoaled.Set(false);
	}

	public override void Update() {
		isGoaled.Update();

		if (isGoaled.Trigger()) {
			ecsGroup.CreateEntity("Block");
		}

		/// MAPDATAより、Playerは 100 or 101なので
		blockData.type = blockData.mapValue % 100 % 10;
		Debug.Log("//////////////////////////////////////////////////////");
		Debug.Log("type: " + blockData.type);
		Debug.Log("//////////////////////////////////////////////////////");

		UpdateColor();
	}

	public void Move(Vector2Int _moveDir) {
		blockData.address += _moveDir;
	}

	public void UpdatePosition() {
		/// 座標更新
		Vector3 newPos = new Vector3(blockData.address.x * blockData.blockSpace, blockData.height,
			blockData.address.y * blockData.blockSpace);
		transform.position = newPos;

		Debug.Log("/////////////////////////////////////////////////////////");
		Debug.Log("id=" + entity.Id + "; name=" + entity.name);
		Debug.Log("pos: .x+" + newPos.x + "; .y=" + newPos.y + "; .z=" + newPos.z);
		Debug.Log("block data: space=" + blockData.blockSpace + "; address.x=" + blockData.address.x + "; address.y="
		          + blockData.address.y + "; height=" + blockData.height);
		Debug.Log("/////////////////////////////////////////////////////////");
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