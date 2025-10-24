#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/Object.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/GameObject.hpp"
#include "DialogCommonBase.hpp"

namespace Gallop
{
	class DialogCommonBaseData : public UnityEngine::Object
	{
	public:
		DialogCommonBase::FormType FormType();

		void FormType(DialogCommonBase::FormType value);

		Il2CppString* Title();

		void Title(Il2CppString* value);

		UnityEngine::GameObject ContentsObject();

		void ContentsObject(UnityEngine::GameObject value);

		using Object::Object;
	};
}
