using System.Runtime.CompilerServices;
using System;
using System.Runtime.InteropServices;
using System.IO;

/// <summary>
/// ワールドを探索するプレイヤーのクラス
/// </summary>
public class Player : MonoScript {

	float height = 0.0f;
	float jumpPower = 5.0f;

	/// ---------------------------------------------------
	/// プレイヤーの移動の変数
	/// ---------------------------------------------------
	bool isDushing = false; // ダッシュ中かどうか
	[SerializeField] float moveSpeed = 16f; // 移動速度
	[SerializeField] float dushSpeed = 32f; // ダッシュ速度
	float lastRotationY_ = 0.0f;
	bool prevIsOperating_ = false;
	bool isOperating_ = false;


	/// ---------------------------------------------------
	/// カメラの変数
	/// ---------------------------------------------------

	[SerializeField] Vector3 sphericalCoord = new Vector3(0.0f, 0f, -8f);
	[SerializeField] Vector3 cameraOffset = new Vector3(0.0f, 4.0f, -11f);
	[SerializeField] Vector3 lookAtOffset = new Vector3(0.0f, 4.0f, 0f);
	Entity camera;

	/// カメラの角度制限(度数法)
	[SerializeField] float maxCameraRotationAngleX = 45f; // カメラのX軸回転の最大角度
	[SerializeField] float minCameraRotationAngleX = 0f;  // カメラのY軸回転の最大角度

	Matrix4x4 matCameraRotateY;

	/// 新しいカメラの動き方のための変数
	Vector3 cameraDirection = Vector3.forward;

	public override void Initialize() {
		camera = ecsGroup.FindEntity("Camera"); // カメラエンティティを取得
		if (camera == null) {
			Debug.LogError("Camera entity not found. Please ensure the camera is initialized before the player.");
			return;
		}
	}

	public override void Update() {
		/// ----- プレイヤーの移動 ----- ///

		Move();
		//Jump();

		CameraFollow();
		//NewCameraUpdate();

		float fallSpeed = 1.0f;
		Vector3 pos = transform.position;
		pos.y -= fallSpeed;
		transform.position = pos;
	}


	void Move() {
		Transform t = transform;

		/// 位置を更新
		Vector3 velocity = new Vector3();
		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.LeftThumb);
		Vector2 keyboardAxis = Input.KeyboardAxis(KeyboardAxis.WASD);

		/// 後で正規化するので大丈夫
		velocity.x = gamepadAxis.x + keyboardAxis.x;
		velocity.z = gamepadAxis.y + keyboardAxis.y;

		if (Input.TriggerGamepad(Gamepad.LeftThumb)) {
			isDushing = !isDushing; // ダッシュのトグル
		}



		/// 移動速度
		float speed = isDushing ? dushSpeed : moveSpeed;

		velocity = velocity.Normalized() * (speed * Time.deltaTime);

		/// カメラの回転に合わせて移動する
		if (camera != null) {
			Transform cT = camera.transform;
			if (cT != null) {
				/// velocityをカメラの向きに合わせる
				velocity = Matrix4x4.Transform(velocity, matCameraRotateY);
			}
		}

		t.position += velocity;
		RotateFromMoveDirection(velocity.Normalized());

	}


	void Jump() {
		if (Input.TriggerKey(KeyCode.Space)) {
			height = jumpPower;
		}

		height -= 9.8f * Time.deltaTime; // 重力

		Transform t = transform;
		Vector3 position = t.position;
		position.y = height;
		t.position = position;
	}



	void CameraFollow() {
		if (camera == null) {
			return;
		}

		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.RightThumb);
		/// 軸反転を行うことでカメラの上下の回転を自分の好みに合わせる
		gamepadAxis.y *= -1f;

		/// 回転角 θ φ
		sphericalCoord.x -= gamepadAxis.y * 0.75f * Time.deltaTime; // X軸の回転
		/// x軸の制限
		sphericalCoord.x = Mathf.Clamp(sphericalCoord.x, Mathf.Deg2Rad * minCameraRotationAngleX, Mathf.Deg2Rad * maxCameraRotationAngleX);
		sphericalCoord.y += gamepadAxis.x * Time.deltaTime; // Y軸の回転

		/// カメラの位置を計算
		Transform cT = camera.transform;
		Vector3 cPos = cT.position;

		/// カメラの位置を計算する
		matCameraRotateY = Matrix4x4.RotateY(sphericalCoord.y);
		Matrix4x4 cameraRotation = Matrix4x4.RotateX(sphericalCoord.x) * matCameraRotateY;
		Vector3 offset = Matrix4x4.Transform(cameraOffset, cameraRotation);
		cPos = transform.position + lookAtOffset + offset;

		/// カメラの向きをプレイヤーに向ける
		Vector3 direction = transform.position + lookAtOffset - cPos; // プレイヤーの位置からカメラの位置へのベクトル
		Vector3 cRot = LookAt(direction);

		/// 制限(カメラが地面の中に埋まらないようにする)
		cRot.x = Mathf.Clamp(cRot.x, minCameraRotationAngleX, maxCameraRotationAngleX);

		// カメラの位置と回転を設定
		cT.position = cPos;
		cT.rotate = Quaternion.FromEuler(cRot);
	}


	void NewCameraUpdate() {
		if (!camera) {
			return;
		}

		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.RightThumb);
		/// 軸反転を行うことでカメラの上下の回転を自分の好みに合わせる
		gamepadAxis.y *= -1f;

		/// カメラの向きを更新
		//cameraDirection = new Vector3(
		//	gamepadAxis.y * 0.75f * Time.deltaTime,
		//	gamepadAxis.x * Time.deltaTime,
		//	0f
		//);
		cameraDirection = new Vector3(gamepadAxis.y, gamepadAxis.x, 0.0f);
		cameraDirection = cameraDirection.Normalized();

		Transform cT = camera.transform;
		cT.position = transform.position;
		cT.rotate = Quaternion.LookAt(Vector3.zero, cameraDirection, Vector3.up);
	}

	public override void OnCollisionEnter(Entity collision) {

	}


	Vector3 LookAt(Vector3 dir) {
		dir = Vector3.Normalize(dir);

		float pitch = Mathf.Asin(-dir.y);
		float yaw = Mathf.Atan2(dir.x, dir.z);

		return new Vector3(pitch, yaw, 0f);
	}


	/// 進行方向に回転する
	private void RotateFromMoveDirection(Vector3 _dir) {
		/// _dirを参照して入力をしているのかどうかを確認する
		prevIsOperating_ = isOperating_;
		if (_dir.Length() > 0.1f) {
			isOperating_ = true;
		} else {
			isOperating_ = false;
		}

		/// 移動していないのであれば最後の回転を維持する
		float rotateY = Mathf.Atan2(_dir.z, _dir.x);
		if (!isOperating_) {
			rotateY = lastRotationY_;
		}

		Vector3 euler = Vector3.zero;
		euler.y = -rotateY + Mathf.PI / 2.0f; // Z軸が前方向なので90度ずらす
		transform.rotate = Quaternion.FromEuler(euler);


		/// 入力をやめたときに最後の回転量を保存しておく
		if (prevIsOperating_ && !isOperating_) {
			lastRotationY_ = euler.y;
		}

	}

}
