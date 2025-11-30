using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleCameraController : MonoBehavior {

	Entity cameraEntity;
	[SerializeField] Vector3 cameraOffset = new Vector3(0f, 5f, -10f);
	[SerializeField] Vector3 lookAtOffset = new Vector3(0f, 2f, 0f);

	public override void Initialize() {

		cameraEntity = ecsGroup.FindEntity("Camera");
		if (!cameraEntity) {
			Debug.LogError("PuzzleCameraController.Initialize - Camera entity not found.");
		}
	}

	public override void Update() {

		if (!cameraEntity) {
			return;
		}

		/// カメラの位置を更新
		Vector3 targetPosition = entity.transform.position + cameraOffset;
		cameraEntity.transform.position = Vector3.Lerp(cameraEntity.transform.position, targetPosition, 0.1f);

		/// カメラの注視点を更新
		Vector3 lookAtPosition = entity.transform.position + lookAtOffset;
		Quaternion targetRotation = Quaternion.LookAt(cameraEntity.transform.position, lookAtPosition, Vector3.up);
		cameraEntity.transform.rotate = targetRotation;

	}

}
