
public class Breathe : MonoBehavior {

	public Vector3 defaultScale = Vector3.one; // デフォルトのスケール
	Vector3 scale = Vector3.one;

	public override void Awake() {

	}

	public override void Initialize() {
		Debug.Log("Breathe initialized.");
		defaultScale = transform.scale; // 初期スケールを保存
	}

	public override void Update() {
		Debug.Log("Breathe Update called.  EntityId:" + entity.Id);

		Transform t = transform;
		scale = defaultScale * Mathf.Clamp(Mathf.Sin(Time.time) * 0.5f + 0.5f, 0.2f, 1f);
		t.scale = scale;
		//t.rotate = Quaternion.FromEuler(
		//	new Vector3(0, Mathf.Sin(Time.time), 0)
		//	);
	}

}
