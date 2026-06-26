#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Matrix4x4.hpp"

namespace UnityEngine
{
	[[maybe_unused]] Matrix4x4 Matrix4x4::zeroMatrix = Matrix4x4{ {.x=0,.y=0,.z=0,.w=0}, {.x=0,.y=0,.z=0,.w=0}, {.x=0,.y=0,.z=0,.w=0}, {.x=0,.y=0,.z=0,.w=0} };
	[[maybe_unused]] Matrix4x4 Matrix4x4::identityMatrix = Matrix4x4{ {.x=1,.y=0,.z=0,.w=0}, {.x=0,.y=1,.z=0,.w=0}, {.x=0,.y=0,.z=1,.w=0}, {.x=0,.y=0,.z=0,.w=1} };
}
