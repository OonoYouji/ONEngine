using System.Diagnostics;

public class Player : MonoBehavior {

	float time = 0.0f;


	public override void Initialize() {
		entityId = 5;
	}

	public override void Update() {
		Move();
	}


	void Move() {

		time += 1f / 60f;


		Transform t = transform;

		/// 位置を更新
		Vector3 pos = t.position;
		pos.x = Mathf.Sin(time) * 10f;
		pos.z = Mathf.Cos(time) * 10f;


		//t.position += new Vector3(0, 0, 1f) * 2f;
		t.position = pos;

		// 元のtransformを更新
		transform = t;
	}

}
