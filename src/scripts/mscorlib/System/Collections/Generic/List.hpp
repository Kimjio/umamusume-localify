#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

namespace System
{
	namespace Collections
	{
		namespace Generic
		{
			template<typename T>
			class List
			{
			public:
				struct Enumerator
				{
				public:
					Il2CppObject* list;
					int index;
					int version;
					T current;
				};
			};
		}
	}
}
