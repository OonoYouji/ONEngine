using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class GoalAlphaObject : MonoScript {

	[SerializeField] float minAlpha = 0.0f;
	[SerializeField] float maxAlpha = 1.0f;
	float time_;
	[SerializeField] float speed = 1.0f;

	MeshRenderer meshRenderer_;

	public override void Initialize() {
		meshRenderer_ = entity.GetComponent<MeshRenderer>();
		if (meshRenderer_) {
			Vector4 color = meshRenderer_.color;
			color.w = 0.0f;
			meshRenderer_.color = color;
		}

		enable = true;
	}

	public override void Update() {
		if (!meshRenderer_) {
			meshRenderer_ = entity.GetComponent<MeshRenderer>();
			return;
		}

		time_ += Time.deltaTime;

		/// 色の変更
		Vector4 color = meshRenderer_.color;
		color.w = minAlpha + (maxAlpha - minAlpha) * 0.5f * (1.0f + Mathf.Sin(time_ * speed));
		meshRenderer_.color = color;


		/// 上下運動
		Vector3 pos = transform.position;
		pos.y = 0.3f + 0.2f * Mathf.Sin(time_ * speed);
		transform.position = pos;

		/// Y軸回転
		Quaternion rot = transform.rotate;
		rot = Quaternion.MakeFromAxis(Vector3.up, time_ * speed);
		transform.rotate = rot;

	}

}
