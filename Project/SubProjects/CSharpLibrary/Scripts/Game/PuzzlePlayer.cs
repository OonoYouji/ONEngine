using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzlePlayer : MonoBehavior {

	public bool isIdle = true;

	/// 左上から
	Vector2 address = Vector2.zero;

	public override void Initialize() {
		// 初期化処理

		Transform t = transform;
		t.position = new Vector3(
			address.x * 1.0f,
			0.0f,
			address.y * 1.0f
		);

	}

	public override void Update() {


	}


}

