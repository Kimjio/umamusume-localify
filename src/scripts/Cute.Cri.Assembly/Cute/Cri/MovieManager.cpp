#include "../../Cute.Cri.Assembly.hpp"
#include "../../../ScriptInternal.hpp"
#include "MovieManager.hpp"

namespace
{
	void* MovieManager_GetMovieInfo_addr = nullptr;
}

static void InitAddress()
{
	MovieManager_GetMovieInfo_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Cute.Cri", "MovieManager", "GetMovieInfo", 1);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Cute::Cri
{
	CriWare::CriMana::MovieInfo* MovieManager::GetMovieInfo(Cute::Cri::MoviePlayerHandle playerHandle)
	{
		return reinterpret_cast<CriWare::CriMana::MovieInfo * (*)(Il2CppObject*, Cute::Cri::MoviePlayerHandle)>(MovieManager_GetMovieInfo_addr)(instance, playerHandle);
	}
}
