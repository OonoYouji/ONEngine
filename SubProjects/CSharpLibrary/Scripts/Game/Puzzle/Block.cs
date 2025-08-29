using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

public class Block : MonoBehavior {
	public PuzzleBlockData blockData;

	/* ----- color ----- */
	private Vector4 currentColor_;

	/* ----- clear vars ----- */
	private bool isStartClearAnimation_;
	private float clearAnimationTime_;
	private int clearEffectMode_;
	private bool isEndClearAnimation_;
	[SerializeField] private float sinValue_;


	private enum Mode : int {
		Up,
		Down
	}

	public override void Initialize() {
		transform.scale = Vector3.one * 0.1f;
	}

	public override void Update() {
		transform.scale = Vector3.one * 0.1f;
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
			currentColor_ = Vector4.one;
			if (blockData.type == (int)BlockType.Black) {
				float value = 0.2f;
				currentColor_ = new Vector4(value, value, value, 1);
			}

			mr.color = currentColor_;
		} else {
			Debug.LogWarning("-----: block color not setting");
		}
	}

	public void UpdatePosition(int _playerType) {
		float height = 0f;
		if (_playerType != this.blockData.type) {
			height -= 0.05f;
		}

		Vector3 newPos = ObjectPool.vector3.Get();
		newPos.x = blockData.address.x * blockData.blockSpace;
		newPos.y = height;
		newPos.z = blockData.address.y * blockData.blockSpace;
		transform.position = newPos;
		
		ObjectPool.vector3.Release(newPos);
	}

	public void StartClearEffect() {
		isStartClearAnimation_ = true;
		clearEffectMode_ = (int)Mode.Up;
	}

	private void UpdateClearEffect() {
		clearAnimationTime_ += Time.deltaTime;
		float clamp = Mathf.Clamp01(clearAnimationTime_);
		float ease = Ease.InOut.Expo(clamp);
		float value = ease * Mathf.PI;
		float sin = Mathf.Sin(value);

		if (value >= Mathf.PI) {
			isEndClearAnimation_ = true;
		}


		if (!isEndClearAnimation_) {
			/// 色を金色にする
			MeshRenderer mr = entity.GetComponent<MeshRenderer>();
			Vector4 color = Vector4.Lerp(currentColor_, Mathf.FromColorCode(0xffd608ff), ease);
			mr.color = color;

			/// 上下にアニメーションさせる(一回キリ)
			Vector3 position = transform.position;
			if (clearEffectMode_ == (int)Mode.Down) {
				position.y = -sin * 0.2f;
			} else {
				position.y = sin * 0.2f;
			}

			transform.position = position;
		}
	}
}