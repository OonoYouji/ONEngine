using System.Diagnostics;

public class PrefabTest : MonoBehavior {
	private float time = 0f;
	private MeshRenderer renderer;

	public override void Initialize() {
		renderer = entity.GetComponent<MeshRenderer>();
	}

	public override void Update() {
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