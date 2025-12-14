public class Billboard : MonoScript {

	Entity camera_;
	public Entity target; /// 注視対象

						  /// billboard axis
	[SerializeField] public bool isBillboardAxisX = true;
	[SerializeField] public bool isBillboardAxisY = true;
	[SerializeField] public Vector3 euler;
	[SerializeField] public Vector3 startRotate_ = Vector3.zero;
	[SerializeField] public Vector3 cameraWPos = Vector3.zero;
	[SerializeField] public Vector3 thisWPos = Vector3.zero;

	public override void Initialize() {
		camera_ = ecsGroup.FindEntity("Camera");
		startRotate_.x = Mathf.PI / 2;
	}

	public override void Update() {
		/// 注視対象が設定されていなければ、処理しない
		if (!target || !camera_) {
			return;
		}

		// カメラの位置からオブジェクトの位置への方向ベクトルを計算
		cameraWPos = camera_.transform.worldPosition;
		thisWPos = transform.worldPosition;

		Vector3 dir = camera_.transform.worldPosition - transform.worldPosition;
		dir.y = 0f;
		dir = Vector3.Normalize(dir);

		euler = Vector3.zero;

		/// Y軸回転（ヨー）
		if (isBillboardAxisY) {
			float yaw = Mathf.Atan2(dir.x, dir.z);
			euler.y = yaw;
		}

		/// X軸回転（ピッチ）
		if (isBillboardAxisX) {
			float pitch = Mathf.Atan2(-dir.y, Mathf.Sqrt(dir.x * dir.x + dir.z * dir.z));
			euler.x = pitch;
		}

		transform.rotate = CreateFromYawPitchRoll(
			euler.y + startRotate_.y,
			euler.x + startRotate_.x, 0.0f);

	}


	Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll) {
		float halfRoll = roll * 0.5f;
		float halfPitch = pitch * 0.5f;
		float halfYaw = yaw * 0.5f;

		float sinRoll = Mathf.Sin(halfRoll);
		float cosRoll = Mathf.Cos(halfRoll);
		float sinPitch = Mathf.Sin(halfPitch);
		float cosPitch = Mathf.Cos(halfPitch);
		float sinYaw = Mathf.Sin(halfYaw);
		float cosYaw = Mathf.Cos(halfYaw);

		Quaternion result;

		result.x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
		result.y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
		result.z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
		result.w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);

		return result;
	}

}
