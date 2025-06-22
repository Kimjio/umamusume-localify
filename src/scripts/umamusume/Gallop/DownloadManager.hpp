#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"

namespace Gallop
{
	class DownloadManager : public MonoSingleton<DownloadManager>
	{
	public:
		class DownloadProgressUIGame {};
		class BackgroundDownloadProgressUI {};

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
