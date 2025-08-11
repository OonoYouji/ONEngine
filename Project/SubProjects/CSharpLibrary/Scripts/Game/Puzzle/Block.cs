using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

public class Block : MonoBehavior {
	public PuzzleBlockData blockData;

	/* ----- clear vars ----- */
	private bool isStartClearAnimation_;
	private float clearAnimationTime_;

	public override void Initialize() {
		transform.scale = Vector3.one * 0.1f;
	}

	public override void Update() {
		// MAPDATAから、ブロックは 10 or 11なので一桁目だけ見て色を判断
		if (blockData.mapValue != 0) {
			blockData.type = blockData.mapValue % 10;
		}


		/// クリア演出
		if (isStartClearAnimation_) {
			UpdateClearEffect();
		} else {
			UpdateColor();
		}
	}

	/// <summary>
	/// 色の更新処理
	/// </summary>
	public void UpdateColor() {
		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr) {
			Vector4 color = Vector4.one;
			if (blockData.type == (int)BlockType.Black) {
				float value = 0.2f;
				color = new Vector4(value, value, value, 1);
			}

			mr.color = color;
		} else {
			Debug.LogWarning("-----: block color not setting");
		}
	}

	public void UpdatePosition(Vector3 _offset) {
		Vector3 newPos = new Vector3(blockData.address.x * blockData.blockSpace, blockData.height,
			blockData.address.y * blockData.blockSpace);
		newPos -= _offset;

		transform.position = newPos;
	}

	public void StartClearEffect(PuzzlePlayer _player) {
		isStartClearAnimation_ = true;
	}

	private void UpdateClearEffect() {
		clearAnimationTime_ += Time.deltaTime;
		/// _playerの色と自身の色を比較、色次第で別々の演出をする
		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		mr.color = new Vector4(1, 0, 0, 1);
	}
}