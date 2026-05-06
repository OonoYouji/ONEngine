using System;
using ONEngine.Scripting;
using ONEngine.Scripting.Generated;

namespace Scripts
{
    public class RotatingCube : GameScript
    {
        public override void Update(float deltaTime)
        {
            ref var t = ref transformRef;
            t.rotation.y += 1.0f * deltaTime;
        }
    }
}
