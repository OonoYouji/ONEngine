
public class Breathe : MonoBehavior {

	public Vector3 defaultScale = Vector3.one; // デフォルトのスケール
	Vector3 scale = Vector3.one;

	public override void Awake() {

	}

	public override void Initialize() {
		Log.WriteLine("Breathe initialized.");
		defaultScale = transform.scale; // 初期スケールを保存
	}

	public override void Update() {
		Log.WriteLine("Breathe Update called.");
		//if (transform == null) {
		//	Log.WriteLine("Transform is null in Breathe.");
		//	return;
		//}

		//Transform t = transform;
		scale = defaultScale * Mathf.Clamp(Mathf.Sin(Time.time) * 0.5f + 0.5f, 0.2f, 1f);

	}

}
