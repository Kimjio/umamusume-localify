#include "../../Cute.Cri.Assembly.hpp"
#include "../../../ScriptInternal.hpp"
#include "MoviePlayerBase.hpp"

namespace
{
	void* MoviePlayerBase_get_MovieInfo_addr = nullptr;
}

static void InitAddress()
{
	MoviePlayerBase_get_MovieInfo_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Cute.Cri", "MoviePlayerBase", "get_MovieInfo", 0);
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace Cute::Cri
{
	CriWare::CriMana::MovieInfo* MoviePlayerBase::MovieInfo()
	{
		return reinterpret_cast<CriWare::CriMana::MovieInfo * (*)(Il2CppObject*)>(MoviePlayerBase_get_MovieInfo_addr)(instance);
	}
}
