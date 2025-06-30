

public class Player : MonoBehavior {

	float height = 0.0f;
	float jumpPower = 5.0f;

	bool isDushing = false; // ダッシュ中かどうか
	float moveSpeed = 16f; // 移動速度
	float dushSpeed = 32f; // ダッシュ速度

	Vector3 sphericalCoord = new Vector3(0.0f, 0f, -8f); // カメラのオフセット
	Vector3 cameraOffset = new Vector3(0.0f, 2.0f, 0f); // カメラのオフセット（球面座標）


	public override void Initialize() {

	}

	public override void Update() {
		Move();
		//Jump();

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

		/// カメラの回転に合わせて移動する
		Transform cT = entity.GetChild(0).transform;
		if (cT != null) {
			Matrix4x4 matCameraRotate = Matrix4x4.RotateY(cT.rotate.y);
			velocity = Matrix4x4.Transform(matCameraRotate, velocity);
		}

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

		/// 入力
		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.RightThumb);

		/// 回転角 θ φ
		sphericalCoord.x += gamepadAxis.y * 0.75f * Time.deltaTime; // X軸の回転
		sphericalCoord.y += gamepadAxis.x * Time.deltaTime; // Y軸の回転

		/// 距離 r
		float distance = sphericalCoord.z; // カメラとプレイヤーの距離

		/// カメラの位置を計算
		Transform cT = entity.GetChild(0).transform;
		Vector3 cPos = cT.position;
		Vector3 cRot = cT.rotate;

		cPos.x = distance * Mathf.Sin(sphericalCoord.y) * Mathf.Cos(sphericalCoord.x);
		cPos.y = distance * Mathf.Sin(sphericalCoord.x);
		cPos.z = distance * Mathf.Cos(sphericalCoord.y) * Mathf.Cos(sphericalCoord.x);

		cRot = LookAt(-cPos); // カメラの向きをプレイヤーに向ける

		cT.position = cPos + cameraOffset; // プレイヤーの位置にオフセットを加える
		cT.rotate = cRot;
	}


	public override void OnCollisionEnter(Entity collision) {

		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr == null) {
			Log.WriteLine("Collision with non-mesh object: " + collision.name);
			return; // メッシュレンダラーがない場合は何もしない
		}

		mr.color = new Vector4(0f, 0f, 0f, 1f); // 衝突したオブジェクトの色を赤に変更

	}


	Vector3 LookAt(Vector3 dir) {
		dir = Vector3.Normalize(dir);

		float pitch = Mathf.Asin(-dir.y);
		float yaw = Mathf.Atan2(dir.x, dir.z);

		return new Vector3(pitch, yaw, 0f);
	}

}
