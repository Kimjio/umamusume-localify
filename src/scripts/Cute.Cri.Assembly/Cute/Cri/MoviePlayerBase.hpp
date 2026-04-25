#pragma once
#include "MoviePlayerBase.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"
#include "scripts/CriMw.CriWare.RUntime/CriWare/CriMana/MovieInfo.hpp"

namespace Cute::Cri
{
	class MoviePlayerBase : public UnityEngine::MonoBehaviour
	{
	public:
		CriWare::CriMana::MovieInfo* MovieInfo();

		using MonoBehaviour::MonoBehaviour;
	};
}
