using System.Runtime.CompilerServices;
using System;
using System.Runtime.InteropServices;
using System.IO;

public class Player : MonoBehavior {

	float height = 0.0f;
	float jumpPower = 5.0f;

	bool isDushing = false; // ダッシュ中かどうか
	[SerializeField] float moveSpeed = 16f; // 移動速度
	[SerializeField] float dushSpeed = 32f; // ダッシュ速度

	[SerializeField] Vector3 sphericalCoord = new Vector3(0.0f, 0f, -8f); // カメラのオフセット
	[SerializeField] Vector3 cameraOffset = new Vector3(0.0f, 4.0f, -11f); // カメラのオフセット（球面座標）
	Entity camera;

	public override void Awake() {
	}

	public override void Initialize() {
		camera = ecsGroup.FindEntity("Camera"); // カメラエンティティを取得
		if (camera == null) {
			Debug.LogError("Camera entity not found. Please ensure the camera is initialized before the player.");
			return;
		}

		//camera.parent = this.entity;
	}

	public override void Update() {

		Move();
		//Jump();

		CameraFollow();

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
				Matrix4x4 matCameraRotate = Matrix4x4.RotateY(cT.rotate.y);
				velocity = Matrix4x4.Transform(velocity, matCameraRotate);
			}
		}

		t.position += velocity;
		RotateFromMoveDirection(velocity.Normalized());


		/// animationさせるかどうか
		SkinMeshRenderer smr = entity.GetComponent<SkinMeshRenderer>();
		if (smr != null) {
			if (velocity.Length() > 0.01f) {
				smr.isPlaying = true; // 動いているときはアニメーションを再生
			} else {
				smr.isPlaying = false; // 動いていないときはアニメーションを停止
			}
		} else {
			Debug.Log("SkinMeshRenderer not found on entity: " + entity.name);
		}
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


	[SerializeField] Vector3 lastPlayerPosition = new Vector3();
	void CameraFollow() {
		if (camera == null) {
			return; // 子エンティティがない場合は何もしない
		}

		/// 入力
		Vector2 gamepadAxis = Input.GamepadThumb(GamepadAxis.RightThumb);

		/// 回転角 θ φ
		sphericalCoord.x += gamepadAxis.y * 0.75f * Time.deltaTime; // X軸の回転
		sphericalCoord.y += gamepadAxis.x * Time.deltaTime; // Y軸の回転

		/// 距離 r
		float distance = sphericalCoord.z; // カメラとプレイヤーの距離

		/// カメラの位置を計算
		Transform cT = camera.transform;
		Vector3 cPos = cT.position;
		//Vector3 cRot = cT.rotate.ToEuler();

		cPos.x = distance * Mathf.Sin(sphericalCoord.y) * Mathf.Cos(sphericalCoord.x);
		cPos.y = distance * Mathf.Sin(sphericalCoord.x);
		cPos.z = distance * Mathf.Cos(sphericalCoord.y) * Mathf.Cos(sphericalCoord.x);

		//cRot = LookAt(transform.position - cPos); // カメラの向きをプレイヤーに向ける

		cT.position = this.transform.position + cameraOffset; // プレイヤーの位置にオフセットを加える
		lastPlayerPosition = this.transform.position;
		//cT.rotate = Quaternion.FromEuler(cRot);
	}


	public override void OnCollisionEnter(Entity collision) {

		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr == null) {
			Debug.Log("Collision with non-mesh object: " + collision.name);
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


	/// 進行方向に回転する
	private void RotateFromMoveDirection(Vector3 _dir) {
		float rotateY = Mathf.Atan2(_dir.z, _dir.x);
		Vector3 euler = transform.rotate.ToEuler();
		euler.y = -rotateY + Mathf.PI / 2.0f; // Z軸が前方向なので90度ずらす
		transform.rotate = Quaternion.FromEuler(euler);
	}

}
