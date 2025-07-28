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
		UpdateColor();
	}

	public void Move(Vector2Int _moveDir) {
		blockData.address += _moveDir;
	}

	public void UpdatePosition() {
		/// 座標更新
		Transform t = transform;
		if (t != null) {
			t.position = new Vector3(
				blockData.address.x * blockData.blockSpace,
				blockData.height,
				blockData.address.y * blockData.blockSpace
			);
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

	
}