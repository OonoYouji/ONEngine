using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class DebugCamera : MonoScript {
	/* ----- 移動に用いる変数群 ----- */
	[SerializeField] private bool isActive_;
	[SerializeField] private float moveSpeed_;
	[SerializeField] private Vector3 eulerAngles_;
	[SerializeField] private Vector3 position_;
	[SerializeField] private Vector3 velocity_;

	public override void Initialize() {
		eulerAngles_ = Vector3.zero;
		isActive_ = true;
		moveSpeed_ = 0.05f;
	}

	public override void Update() {
		if (Input.PressKey(KeyCode.LeftControl) && Input.TriggerKey(KeyCode.C)) {
			isActive_ = !isActive_;
		}

		if (!isActive_) {
			return;
		}

		/// 移動速度の切り替え
		float mouseWheel = Input.MouseWheel();

		if (Input.PressKey(KeyCode.LeftControl)) {
			/// 下げる処理
			if (Input.TriggerKey(KeyCode.Minus)) {
				moveSpeed_ *= 0.5f;
				if (moveSpeed_ < 0.001f) moveSpeed_ = 0.001f;
			}
			if (mouseWheel < 0f) {
				moveSpeed_ *= 0.9f;
				if (moveSpeed_ < 0.001f) moveSpeed_ = 0.001f;
			}

			/// 上げる処理
			if (Input.TriggerKey(KeyCode.Equals)) {
				moveSpeed_ *= 2.0f;
				if (moveSpeed_ > 1.0f) moveSpeed_ = 1.0f;
			}
			if (mouseWheel > 0f) {
				moveSpeed_ *= 1.1f;
				if (moveSpeed_ > 1.0f) moveSpeed_ = 1.0f;
			}
		}

		/// C++側のTransform情報の取得と同期
		if (transform) {
			position_ = transform.position;

			// ★ここが重要: C++側の最新のQuaternion(rotate)からForwardベクトルを作り、PitchとYawを計算し直す
			// （もし transform.rotate.eulerAngles などの変換プロパティがエンジン側にある場合は、それを使っても構いません）
			Vector3 forward = Matrix4x4.Transform(new Vector3(0f, 0f, 1f), Matrix4x4.Rotate(transform.rotate));

			// 誤差で Asin に -1.0～1.0 以外の値が入るのを防ぐため Math.Max/Min でクランプ
			float clampedY = Math.Max(-1.0f, Math.Min(1.0f, forward.y));

			float yaw = (float)Math.Atan2(forward.x, forward.z);
			float pitch = -(float)Math.Asin(clampedY);

			// C++の回転状態をC#の変数に同期
			eulerAngles_ = new Vector3(pitch, yaw, 0f);
		}

		/// 実際の移動・回転処理
		if (Input.PressMouse(Mouse.Right)) {
			float speed = moveSpeed_;
			if (Input.PressKey(KeyCode.LeftShift)) {
				speed *= 2.0f;
			}

			velocity_ = new Vector3(0f, 0f, 0f);
			if (Input.PressKey(KeyCode.W)) velocity_.z += speed;
			if (Input.PressKey(KeyCode.S)) velocity_.z -= speed;
			if (Input.PressKey(KeyCode.A)) velocity_.x -= speed;
			if (Input.PressKey(KeyCode.D)) velocity_.x += speed;
			if (Input.PressKey(KeyCode.E)) velocity_.y += speed;
			if (Input.PressKey(KeyCode.Q)) velocity_.y -= speed;

			velocity_ = Matrix4x4.Transform(velocity_, Matrix4x4.Rotate(transform.rotate));
			position_ += velocity_ * 10f;
			transform.position = position_;

			Vector2 mouseMove = Input.MouseVelocity();

			// 先ほどC++から同期したeulerAngles_に対して、マウスの差分を加算する
			eulerAngles_.x += mouseMove.y * 0.01f;
			eulerAngles_.y += mouseMove.x * 0.01f;

			transform.rotate = Quaternion.FromEuler(eulerAngles_);
		}
	}
}