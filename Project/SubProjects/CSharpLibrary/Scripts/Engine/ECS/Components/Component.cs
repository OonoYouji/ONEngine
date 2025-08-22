using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Component {
	public int enable;
	/// 0 = disabled, 1 = enabled
	public ulong nativeHandle;
	public Entity entity;
	
	public virtual void Begin() {}
	public virtual void End() {}

	public static implicit operator bool(Component _component) {
		return _component != null;
	}
}