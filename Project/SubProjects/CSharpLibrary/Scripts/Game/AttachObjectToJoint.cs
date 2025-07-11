public class AttachObjectToJoint : MonoBehavior {

	public string jointName = "JointName"; // Attach to this joint
	public Entity attachedEntity; // Entity to attach

	public override void Initialize() {
		
	}

	public override void Update() {
		if(attachedEntity == null) {
			Debug.LogError("Attached entity is not set.");
			return;
		}

		SkinMeshRenderer smr = entity.GetComponent<SkinMeshRenderer>();
		if(smr == null) {
			Debug.LogError("SkinMeshRenderer not found on entity.");
			return;
		}

		TransformData jointTransform = smr.GetJointTransform(jointName);
		if (jointTransform == null) {
			Debug.LogError("Joint not found: " + jointName);
			return;
		}

		Transform t = attachedEntity.transform;
		t.position = jointTransform.position;
		t.rotate = jointTransform.rotate;
		//t.scale = jointTransform.scale;

	}


}