using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class DebugCamera : MonoBehavior {
	/* ----- 移動に用いる変数群 ----- */
	[SerializeField] private bool isActive_;
	[SerializeField] private bool isMoving_;
	[SerializeField] private float moveSpeed_;
	[SerializeField] private Vector3 eulerAngles_;
	[SerializeField] private Vector3 position_;
	[SerializeField] private Vector3 velocity_;

	public override void Initialize() {
		transform.position = new Vector3(0f, 0f, -10f);
		eulerAngles_ = Vector3.zero;

		isActive_ = true;
		isMoving_ = false;
		moveSpeed_ = 0.05f;
	}

	public override void Update() {
		if (Input.PressKey(KeyCode.LeftControl) && Input.TriggerKey(KeyCode.C)) {
			isActive_ = !isActive_;
		}

		/// 非アクティブの時は以降の処理をしない
		if (!isActive_) {
			return;
		}

		/// マウスがimgui scene window内にあるか判断する or C/C++側で判断してからこの関数を呼び出すようにするか

		/// 移動速度の切り替え
		if (Input.PressKey(KeyCode.LeftControl)) {
			/// 下げる処理
			if (Input.TriggerKey(KeyCode.Minus)) {
				moveSpeed_ *= 0.5f;
				if (moveSpeed_ < 0.001f) {
					/// 移動の最低値
					moveSpeed_ = 0.001f;
				}
			}

			/// 上げる処理
			if (Input.TriggerKey(KeyCode.Equals)) {
				moveSpeed_ *= 2.0f;
				if (moveSpeed_ > 1.0f) {
					/// 移動の最大値を設定
					moveSpeed_ = 1.0f;
				}
			}
		}


		/// 実際の移動処理
		isMoving_ = false;

		if (transform) {
			position_ = transform.position;
		}

		if (Input.PressMouse(Mouse.Right)) {
			isMoving_ = true;

			float speed = moveSpeed_;
			if (Input.PressKey(KeyCode.LeftShift)) {
				speed *= 2.0f;
			}

			velocity_ = new Vector3(0f, 0f, 0f);
			if (Input.PressKey(KeyCode.W)) {
				velocity_.z += speed;
			}

			if (Input.PressKey(KeyCode.S)) {
				velocity_.z -= speed;
			}

			if (Input.PressKey(KeyCode.A)) {
				velocity_.x -= speed;
			}

			if (Input.PressKey(KeyCode.D)) {
				velocity_.x += speed;
			}

			if (Input.PressKey(KeyCode.E)) {
				velocity_.y += speed;
			}

			if (Input.PressKey(KeyCode.Q)) {
				velocity_.y -= speed;
			}

			velocity_ = Matrix4x4.Transform(velocity_, Matrix4x4.Rotate(transform.rotate));
			position_ += velocity_ * 10f;
			transform.position = position_;

			Vector2 mouseMove = Input.MouseVelocity();
			eulerAngles_.x += mouseMove.y * 0.01f;
			eulerAngles_.y += mouseMove.x * 0.01f;

			transform.rotate = Quaternion.FromEuler(eulerAngles_);
		}

	}
}