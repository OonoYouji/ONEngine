using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;



public class Player : MonoBehavior {

	float time = 0.0f;
	float height = 0.0f;
	float jumpPower = 5.0f;

	float moveSpeed = 64f; // 移動速度
	float dushSpeed = 120f; // ダッシュ速度

	public override void Initialize() {
		// entityId = Find("Player");
		//entityId = 5;
		time = 1f / 60f;

		//Update();
	}

	public override void Update() {
		Move();
		Jump();
	}


	public void Move() {
		Transform t = transform;

		/// 位置を更新
		Vector3 velocity = new Vector3();

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

		float speed = moveSpeed;
		if (Input.PressKey(DIK.DIK_LSHIFT)) {
			speed = dushSpeed; // ダッシュ
		}

		velocity = velocity.Normalized() * (speed * time);

		t.position += velocity;

		// 元のtransformを更新
		transform = t;
	}


	void Jump() {
		if (Input.TriggerKey(DIK.DIK_SPACE)) {
			height = jumpPower;
		}


		if (height > 0.0f) {
			height -= 9.8f * time; // 重力
								   //if (height < 0.0f) {
								   //	height = 0.0f;
								   //}
		}

		Transform t = transform;
		Vector3 position = t.position;
		position.y = height;
		t.position = position;

		transform = t;

	}


}
