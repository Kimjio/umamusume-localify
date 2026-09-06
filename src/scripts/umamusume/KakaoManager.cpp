#include "../ScriptInternal.hpp"
#include "KakaoManager.hpp"

namespace
{
	Il2CppMethodPointer KakaoManager_GetKakaoOptionValue_addr = nullptr;
	void* KakaoManager_GetKakaoOptionValue_orig = nullptr;
}

Il2CppString* KakaoManager_GetKakaoOptionValue_hook(Il2CppObject* self, Il2CppString* optionKey)
{
	if (optionKey->chars == IL2CPP_STRING("kakaoUmaNumberOfDlls"s))
	{
		return il2cpp_string_new("0");
	}

	return reinterpret_cast<Il2CppString * (*)(Il2CppObject*, Il2CppString*)>(KakaoManager_GetKakaoOptionValue_orig)(self, optionKey);
}

static void InitAddress()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		KakaoManager_GetKakaoOptionValue_addr = il2cpp_symbols::get_method_pointer("umamusume.dll", "", "KakaoManager", "GetKakaoOptionValue", 1);
	}
}

static void HookMethods()
{
#ifdef _MSC_VER
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(KakaoManager_GetKakaoOptionValue, "KakaoManager::GetKakaoOptionValue at %p");
	}
#endif
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace
{

}
