using System.Diagnostics;

public class PrefabTest : MonoBehavior {
	private float time = 0f;
	private MeshRenderer renderer;

	public override void Initialize() {
		renderer = entity.GetComponent<MeshRenderer>();
	}

	public override void Update() {
		Vector3 pos = new Vector3();
		for (int i = 0; i < 1000000; i++) {
			pos += Vector3.up;
		}

		time += Time.deltaTime;
		transform.scale = Vector3.one * Mathf.Sin(time);
	}

	public override void OnCollisionEnter(Entity collision) {
		if (renderer) {
			renderer.color = Vector4.green;
		}
	}

	public override void OnCollisionStay(Entity collision) {
		if (renderer) {
			renderer.color = Vector4.red;
		}
	}

	public override void OnCollisionExit(Entity collision) {
		if (renderer) {
			renderer.color = Vector4.blue;
		}
	}
}