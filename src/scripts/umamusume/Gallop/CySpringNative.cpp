#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "CySpringNative.hpp"

#include "FrameRateController.hpp"

namespace
{
	Il2CppClass* CySpringNative_klass = nullptr;

	void* CySpringNative_cctor_addr = nullptr;
	void* CySpringNative_cctor_orig = nullptr;

	void* CySpringNative_UpdateForce_addr = nullptr;
	void* CySpringNative_UpdateForce_orig = nullptr;

	void* CySpringNative_NativeClothUpdate_addr = nullptr;
	
	void* CySpringNative_NativeClothSkirtUpdate_addr = nullptr;
	
	void* CySpringNative_NativeSkirtUpdate_addr = nullptr;

	FieldInfo* CySpringNative_isNativeField = nullptr;
}

static void CySpringNative_cctor_hook()
{
	reinterpret_cast<decltype(CySpringNative_cctor_hook)*>(CySpringNative_cctor_orig)();
	bool isNative = !config::cyspring_disable_native;
	il2cpp_field_static_set_value(CySpringNative_isNativeField, &isNative);
}

static void CySpringNative_UpdateForce_hook(Gallop::NativeClothWorking* clothWorking, float stiffnessForceRate, float dragForceRate, float gravityRate, Vector3 windPower, float windStrength, Vector3 positionDiff, float frameScale)
{
	if (config::cyspring_update_mode == 1)
	{
		frameScale = config::cyspring_mono_uncap_frame_scale ? min(60.0f, Gallop::FrameRateController::Instance().GetLayerFrameRate(Gallop::FrameRateController::FrameRateOverrideLayer::SystemValue) / 2.0f) : 60.0f;
	}
	reinterpret_cast<decltype(CySpringNative_UpdateForce_hook)*>(CySpringNative_UpdateForce_orig)(clothWorking, stiffnessForceRate, dragForceRate, gravityRate, windPower, windStrength, positionDiff, frameScale);
}

static void InitAddress()
{
	CySpringNative_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "CySpringNative");
	CySpringNative_cctor_addr = il2cpp_symbols::get_method_pointer(CySpringNative_klass, ".cctor", 0);
	CySpringNative_UpdateForce_addr = il2cpp_symbols::get_method_pointer(CySpringNative_klass, "UpdateForce", 8);
	CySpringNative_isNativeField = il2cpp_class_get_field_from_name(CySpringNative_klass, "isNative");
#ifdef _MSC_VER
	wstring module_name;
	module_name.resize(MAX_PATH);
	module_name.resize(GetModuleFileNameW(nullptr, module_name.data(), MAX_PATH));

	filesystem::path module_path(module_name);

	wstring name = module_path.filename().replace_extension();

	SetDllDirectoryW((name + L"_Data\\Plugins\\x86_64\\"s).data());

	auto CySpringPlugin = LoadLibraryExW(L"CySpringPlugin.dll", nullptr, 0);

	SetDllDirectoryW(nullptr);

	CySpringNative_NativeClothUpdate_addr = GetProcAddress(CySpringPlugin, "NativeClothUpdate");
	CySpringNative_NativeClothSkirtUpdate_addr = GetProcAddress(CySpringPlugin, "NativeClothSkirtUpdate");
	CySpringNative_NativeSkirtUpdate_addr = GetProcAddress(CySpringPlugin, "NativeSkirtUpdate");
#else
	auto CySpringPlugin = dlopen("libCySpringPlugin.so", RTLD_NOW);

	CySpringNative_NativeClothUpdate_addr = dlsym(CySpringPlugin, "NativeClothUpdate");
	CySpringNative_NativeClothSkirtUpdate_addr = dlsym(CySpringPlugin, "NativeClothSkirtUpdate");
	CySpringNative_NativeSkirtUpdate_addr = dlsym(CySpringPlugin, "NativeSkirtUpdate");
#endif
}

static void HookMethods()
{
	ADD_HOOK(CySpringNative_cctor, "Gallop.CySpringNative::.cctor at %p\n");
	ADD_HOOK(CySpringNative_UpdateForce, "Gallop.CySpringNative::UpdateForce at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	bool CySpringNative::isNative()
	{
		bool value;
		il2cpp_field_static_get_value(CySpringNative_isNativeField, &value);
		return value;
	}

	void CySpringNative::isNative(bool value)
	{
		il2cpp_field_static_set_value(CySpringNative_isNativeField, &value);
	}

	void CySpringNative::NativeClothUpdate(NativeClothWorking* cond, int nCond, NativeClothCollision* collisions, NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate)
	{
		reinterpret_cast<decltype(NativeClothUpdate)*>(CySpringNative_NativeClothUpdate_addr)(cond, nCond, collisions, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, true, moveRate, addMoveRate, springRate);
	}

	void CySpringNative::NativeClothSkirtUpdate(NativeClothWorking* cond, int nCond, NativeClothCollision* collisions, NativeSkirtWorking* pWorking, NativeSkirtArg* pArg, NativeRootParentWork* pRootParentWork, float stiffnessForceRate, float dragForceRate, float gravityRate, float windX, float windY, float windZ, float windStrength, bool bCollisionSwitch, float timescale, bool is60FPS, float moveRate, float addMoveRate, float springRate)
	{
		reinterpret_cast<decltype(NativeClothSkirtUpdate)*>(CySpringNative_NativeClothSkirtUpdate_addr)(cond, nCond, collisions, pWorking, pArg, pRootParentWork, stiffnessForceRate, dragForceRate, gravityRate, windX, windY, windZ, windStrength, bCollisionSwitch, timescale, true, moveRate, addMoveRate, springRate);
	}

	void CySpringNative::NativeSkirtUpdate(NativeSkirtWorking* pWorking, NativeSkirtArg* pArg)
	{
		reinterpret_cast<decltype(NativeSkirtUpdate)*>(CySpringNative_NativeSkirtUpdate_addr)(pWorking, pArg);
	}
}
