#pragma once
#include "MoviePlayerBase.hpp"
#include "MoviePlayerHandle.hpp"
#include "scripts/UnityEngine.CoreModule/UnityEngine/MonoBehaviour.hpp"
#include "scripts/CriMw.CriWare.RUntime/CriWare/CriMana/MovieInfo.hpp"

namespace Cute::Cri
{
	class MovieManager : public UnityEngine::MonoBehaviour
	{
	public:
		CriWare::CriMana::MovieInfo* GetMovieInfo(Cute::Cri::MoviePlayerHandle playerHandle);

		using MonoBehaviour::MonoBehaviour;
	};
}
