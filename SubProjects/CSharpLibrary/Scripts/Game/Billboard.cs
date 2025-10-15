
public class Billboard : MonoBehavior {

	Entity camera_;
	public Entity target; /// 注視対象

	/// billboard axis
	[SerializeField] public bool isBillboardAxisX = true;
	[SerializeField] public bool isBillboardAxisY = true;

	public override void Initialize() {
		camera_ = ecsGroup.FindEntity("Camera");
	}

	public override void Update() {
		/// 注視対象が設定されていなければ、処理しない
		if (!target || !camera_) {
			return;
		}

		Vector3 dir = transform.position - camera_.transform.position;
		//Vector3 dir = camera_.transform.position - target.transform.position;
		Vector3 euler = Vector3.zero;


		/// Y軸回転（ヨー）
		if (isBillboardAxisY) {
			float yaw = Mathf.Atan2(dir.x, dir.z);
			euler.y = yaw * Mathf.Rad2Deg;
		}

		/// X軸回転（ピッチ）
		if (isBillboardAxisX) {
			float pitch = Mathf.Atan2(-dir.y, Mathf.Sqrt(dir.x * dir.x + dir.z * dir.z));
			euler.x = pitch * Mathf.Rad2Deg;
		}

		transform.rotate = Quaternion.FromEuler(euler);

	}

}
