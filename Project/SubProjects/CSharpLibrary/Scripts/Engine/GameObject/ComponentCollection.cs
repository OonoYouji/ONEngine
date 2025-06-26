using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

public class ComponentCollection {
	
	List<Component> components = new List<Component>();

	//void Initialize() {
	//	components.Add(new Transform());
	//}


	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern Component GetComponent(int _entityId);

}
