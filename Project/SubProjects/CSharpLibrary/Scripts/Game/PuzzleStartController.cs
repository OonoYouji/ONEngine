using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class PuzzleStartController : MonoBehavior {
	
	private List<MonoBehavior> thisScripts_; 

	public override void Initialize() {
		thisScripts_ = entity.GetScripts();

		int thisIndex = 0;
		for (int i = 0; i < thisScripts_.Count; i++) {
			if (thisScripts_[i] == this) {
				thisIndex = i;
			}
		}

		/// このスクリプトは管理しないので削除しておく
		thisScripts_.Remove(thisScripts_[thisIndex]);
	}

	public override void Update() {
		Debug.Log("====================================================");
		Debug.Log("this script count = " + thisScripts_.Count);
		Debug.Log("====================================================");
	}

}