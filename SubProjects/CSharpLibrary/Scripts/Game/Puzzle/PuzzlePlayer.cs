using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzlePlayer : MonoBehavior {
	public PuzzleBlockData blockData; // ブロックデータ
	public Flag isGoaled;
	private AudioSource audioSource_;
	public bool isActive = false;

	public override void Initialize() {
		// 初期化処理
		isGoaled.Set(false);
		audioSource_ = entity.GetComponent<AudioSource>();
	}

	public override void Update() {
		isGoaled.Update();

		if (isGoaled.Trigger()) {
			ecsGroup.CreateEntity("Block");
		}

		/// MAPDATAより、Playerは 100 or 101なので
		blockData.type = blockData.mapValue % 100 % 10;
		UpdateColor();
	}

	public void Move(Vector2Int _moveDir) {
		blockData.address += _moveDir;
	}

	public void PlayMoveSE() {
		//audioSource_.OneShotPlay(1f, 1f, "./Assets/Sounds/Game/se/blackMove.mp3");
	}

	public void UpdatePosition() {
		/// 座標更新
		Vector3 newPos = new Vector3(blockData.address.x * blockData.blockSpace, 0f,
			blockData.address.y * blockData.blockSpace);
		transform.position = newPos;
	}

	public void UpdateColor() {
		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr) {
			Vector4 color = Vector4.one;
			if (blockData.type == (int)BlockType.Black) {
				float value = 0.2f;
				color = new Vector4(value, value, value, 1);
			}

			/// アクティブであるなら赤色にする
			if (isActive) {
				color = Vector4.Lerp(color, Vector4.red, 0.5f);
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