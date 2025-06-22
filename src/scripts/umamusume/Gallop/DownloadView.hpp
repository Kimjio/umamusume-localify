#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"

namespace Gallop
{
	class DownloadView : public UnityEngine::MonoBehaviour
	{
	public:
		void UpdateView(float downloadSize, float allDownloadSize);

		using UnityEngine::MonoBehaviour::MonoBehaviour;
	};
}
