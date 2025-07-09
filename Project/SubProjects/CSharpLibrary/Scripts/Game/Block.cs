using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Block : MonoBehavior {

	int widthAddress, heightAddress;
	Entity playerPtr;

	public Block(int _widthAddress, int _heightAddress) {
		widthAddress = _widthAddress;
		heightAddress = _heightAddress;
	} 

	public override void Initialize() {
		playerPtr = EntityCollection.FindEntity("Player");
	}

	public override void Update() {

	}




}
