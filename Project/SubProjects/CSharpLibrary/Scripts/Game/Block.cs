using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Block : MonoBehavior {
	public PuzzleBlockData blockData;
	Entity playerPtr;

	public override void Initialize() {
		playerPtr = EntityCollection.FindEntity("Player");
	}

	public override void Update() {
		Debug.Log("============: type " + blockData.type);
		UpdateColor();
	}

	/// <summary>
	/// 色の更新処理
	/// </summary>
	public void UpdateColor() {
		MeshRenderer mr = entity.GetComponent<MeshRenderer>();
		if (mr) {
			Vector4 color = Vector4.one;
			if (blockData.type == (int)BlockType.Black) {
				float value = 0.2f;
				color = new Vector4(value, value, value, 1);
			}
			
			mr.color = color;
		} else {
			Debug.LogWarning("-----: block color not setting");
		}
		
	}
}