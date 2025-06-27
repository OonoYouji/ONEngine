using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using System.Xml;



public class Player : MonoBehavior {

	float height = 0.0f;
	float jumpPower = 5.0f;

	float moveSpeed = 64f; // 移動速度
	float dushSpeed = 120f; // ダッシュ速度

	public override void Initialize() {

	}

	public override void Update() {
		Move();
		Jump();
	}


	public void Move() {
		Transform t = transform;

		/// 位置を更新
		Vector3 velocity = new Vector3();

		if (Input.PressKey(KeyCode.W)) { velocity.z += 0.1f; }
		if (Input.PressKey(KeyCode.S)) { velocity.z -= 0.1f; }
		if (Input.PressKey(KeyCode.A)) { velocity.x -= 0.1f; }
		if (Input.PressKey(KeyCode.D)) { velocity.x += 0.1f; }

		float speed = moveSpeed;
		if (Input.PressKey(KeyCode.LeftShift)) {
			speed = dushSpeed; // ダッシュ
		}

		velocity = velocity.Normalized() * (speed * Time.deltaTime);

		t.position += velocity;

		// 元のtransformを更新
		transform = t;
	}


	void Jump() {
		if (Input.TriggerKey(KeyCode.Space)) {
			height = jumpPower;
		}


		if (height > 0.0f) {
			height -= 9.8f * Time.deltaTime; // 重力
		}

		Transform t = transform;
		Vector3 position = t.position;
		position.y = height;
		t.position = position;

		transform = t;
	}


	public override void OnCollisionEnter(Entity collision) {

		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr == null) {
			Log.WriteLine("Collision with non-mesh object: " + collision.name);
			return; // メッシュレンダラーがない場合は何もしない
		}

		mr.color = new Vector4(0f, 0f, 0f, 1f); // 衝突したオブジェクトの色を赤に変更

	}


}
