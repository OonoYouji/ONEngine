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
		Vector3 velocity = new Vector3();
		float speed = 0.1f;

		if (Input.PressKey(DIK.DIK_W)) {
			velocity.z += 0.1f;
		}

		if (Input.PressKey(DIK.DIK_S)) {
			velocity.z -= 0.1f;
		}

		if (Input.PressKey(DIK.DIK_A)) {
			velocity.x -= 0.1f;
		}

		if (Input.PressKey(DIK.DIK_D)) {
			velocity.x += 0.1f;
		}

		if(Input.PressKey(DIK.DIK_LSHIFT)) {
			speed = 0.2f;
		}

		velocity = velocity.Normalized() * speed;

		t.position += velocity;

		// 元のtransformを更新
		transform = t;
	}

}
