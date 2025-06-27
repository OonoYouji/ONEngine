using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using System.Xml;



public class Player : MonoBehavior {

	float height = 0.0f;
	float jumpPower = 5.0f;

	bool isDushing = false; // ダッシュ中かどうか
	float moveSpeed = 16f; // 移動速度
	float dushSpeed = 32f; // ダッシュ速度

	public override void Initialize() {

	}

	public override void Update() {
		Move();
		Jump();

		CameraFollow();
	}


	public void Move() {
		Transform t = transform;

		/// 位置を更新
		Vector3 velocity = new Vector3();
		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.LeftThumb);
		velocity.x = gamepadAxis.x;
		velocity.z = gamepadAxis.y;

		if (Input.TriggerGamepad(Gamepad.LeftThumb)) {
			isDushing = !isDushing; // ダッシュのトグル
		}

		/// 移動速度
		float speed = isDushing ? dushSpeed : moveSpeed;

		velocity = velocity.Normalized() * (speed * Time.deltaTime);

		t.position += velocity;

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

	}


	void CameraFollow() {
		/// カメラの位置をプレイヤーの位置に合わせる
		Transform cT = entity.GetChild(0).transform;
		//cT.position = new Vector3(transform.position.x, transform.position.y + 5.0f, transform.position.z - 10.0f);

		/// 入力
		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.RightThumb);
		Vector3 cameraRotate = cT.rotate;
		cameraRotate.y += gamepadAxis.x * 0.1f; // X軸の回転
		cameraRotate.x += gamepadAxis.y * 0.1f; // Y軸の回転
		cT.rotate = cameraRotate;

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
