#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "../umamusume.hpp"
#include "../../UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

#include <string>

using namespace std;

namespace Gallop
{
	class TitleViewController : public UnityEngine::MonoBehaviour
	{
	public:
		static string viewerId;
		static string onetimeToken;

		using UnityEngine::MonoBehaviour::MonoBehaviour;
	};
}
