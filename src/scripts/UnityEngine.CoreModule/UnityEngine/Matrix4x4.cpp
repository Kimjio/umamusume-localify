#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Matrix4x4.hpp"

// void* GetNodeStates_Internal_addr = nullptr;

static void InitAddress()
{
	// GetNodeStates_Internal_addr = il2cpp_resolve_icall("UnityEngine.XR.InputTracking::GetNodeStates_Internal()");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Matrix4x4 Matrix4x4::zeroMatrix = Matrix4x4{ {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} };
	Matrix4x4 Matrix4x4::identityMatrix = Matrix4x4{ {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
}
