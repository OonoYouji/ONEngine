

public class Player : MonoBehavior {

	float height = 0.0f;
	float jumpPower = 5.0f;

	bool isDushing = false; // ダッシュ中かどうか
	float moveSpeed = 16f; // 移動速度
	float dushSpeed = 32f; // ダッシュ速度

	Vector3 cameraOffset = new Vector3(0.0f, 2.0f, -5.0f); // カメラのオフセット

	public override void Initialize() {

	}

	public override void Update() {
		Move();
		Jump();

		//CameraFollow();
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

		/// 入力
		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.RightThumb);

		/// 回転角 θ φ
		cameraOffset.x -= gamepadAxis.y * 0.1f * Time.deltaTime; // X軸の回転
		cameraOffset.y += gamepadAxis.x * 0.1f * Time.deltaTime; // Y軸の回転

		/// 距離 r
		float distance = cameraOffset.z; // カメラとプレイヤーの距離


		/// カメラの位置を計算
		Transform cT = entity.GetChild(0).transform;
		Vector3 cPos = cT.position;
		Vector3 cRot = cT.rotate;

		cPos.x = distance * Mathf.Sin(cameraOffset.y) * Mathf.Cos(cameraOffset.x);
		cPos.y = distance * Mathf.Sin(cameraOffset.x);
		cPos.z = distance * Mathf.Cos(cameraOffset.y) * Mathf.Cos(cameraOffset.x);

		//string log1 = "Camera Position" + Vector3.ToSimpleString(cPos);
		string log2 = "Player Position" + Vector3.ToSimpleString(transform.position);


		/// 
		cRot = Vector3.LookAt(cPos, transform.position); // プレイヤーを向くようにカメラの回転を設定

		cT.position = cPos; // プレイヤーの位置にオフセットを加える
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


}
