using System.Diagnostics;

public class Player : MonoBehavior {

	float time = 0.0f;
	float height = 0.0f;
	float jumpPower = 5.0f;

	public override void Initialize() {
		entityId = 5;
		time = 1f / 60f;
	}

	public override void Update() {
		Move();
		Jump();
	}


	void Move() {
		Transform t = transform;

		/// 位置を更新
		Vector3 velocity = new Vector3();
		float speed = 5.0f;

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

		if (Input.PressKey(DIK.DIK_LSHIFT)) {
			speed = 15.0f;
		}

		velocity = velocity.Normalized() * (speed * time);

		t.position += velocity;

		// 元のtransformを更新
		transform = t;
	}


	void Jump() {
		if (Input.TriggerKey(DIK.DIK_SPACE)) {
			Transform t = transform;
			if (t.position.y <= 0.0f) {
				height = jumpPower;
			}
		}


		if(height > 0.0f) {
			Transform t = transform;
			t.position.y += height * time;
			height -= 9.8f * time; // 重力
			if (t.position.y < 0.0f) {
				t.position.y = 0.0f;
				height = 0.0f;
			}
			transform = t;
		}

	}


}
