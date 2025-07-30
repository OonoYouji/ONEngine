using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Block : MonoBehavior {
	public PuzzleBlockData blockData;
	public PuzzlePlayer playerPtr;

	[SerializeField] private int value;
	[SerializeField] private int type;

	public override void Initialize() {
		// playerPtr = EntityCollection.FindEntity("Player");
	}

	public override void Update() {
		// MAPDATAから、ブロックは 10 or 11なので一桁目だけ見て色を判断
		if (blockData.mapValue != 0) {
			blockData.type = blockData.mapValue % 10;
		}

		transform.scale = Vector3.one * 0.1f;

		type = blockData.type;
		value = blockData.mapValue;
		
		Debug.Log("===========: type " + blockData.type + " value " + blockData.mapValue);

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