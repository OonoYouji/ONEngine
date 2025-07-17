using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzlePlayer : MonoBehavior {

	public PuzzleBlockData blockData; // ブロックデータ

	public bool isIdle = true;

	public override void Initialize() {
		// 初期化処理
	}

	public override void Update() {
			Move();
		if (!isIdle) {
		}


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

	void Move() {

		Vector2Int gamepadAxis = new Vector2Int();
		if (Input.TriggerGamepad(Gamepad.DPadUp)) { gamepadAxis.y -= 1; }
		if (Input.TriggerGamepad(Gamepad.DPadDown)) { gamepadAxis.y += 1; }
		if (Input.TriggerGamepad(Gamepad.DPadLeft)) { gamepadAxis.x -= 1; }
		if (Input.TriggerGamepad(Gamepad.DPadRight)) { gamepadAxis.x += 1; }

		blockData.address += gamepadAxis;

		Debug.Log("player address.x" + blockData.address.x);
		Debug.Log("player address.y" + blockData.address.y);
		Debug.Log("player blockSpace" + blockData.blockSpace);
	}



}

