using System;
using ONEngine.Scripting;
using ONEngine.Scripting.Generated;

namespace Scripts {
	public class RotatingCube : GameScript {
		[SerializeField]
		private float Speed = 2.0f; // これがInspectorに出現します

		public int TestCount = 10;  // publicも自動で出現します

		public override void Update(float deltaTime) {
			transformRef.rotation.y += Speed * deltaTime;
		}
	}
}
