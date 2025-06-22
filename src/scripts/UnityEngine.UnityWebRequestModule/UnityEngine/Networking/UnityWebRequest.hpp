#pragma once
#include "il2cpp/il2cpp_symbols.hpp"

#include "../../../UnityEngine.CoreModule/UnityEngine/Object.hpp"

namespace UnityEngine
{
	namespace Networking
	{
		class UnityWebRequest : public Object
		{
		public:
			enum UnityWebRequestError
			{
				OK,
				Unknown,
				SDKError,
				UnsupportedProtocol,
				MalformattedUrl,
				CannotResolveProxy,
				CannotResolveHost,
				CannotConnectToHost,
				AccessDenied,
				GenericHttpError,
				WriteError,
				ReadError,
				OutOfMemory,
				Timeout,
				HTTPPostError,
				SSLCannotConnect,
				Aborted,
				TooManyRedirects,
				ReceivedNoData,
				SSLNotSupported,
				FailedToSendData,
				FailedToReceiveData,
				SSLCertificateError,
				SSLCipherNotAvailable,
				SSLCACertError,
				UnrecognizedContentEncoding,
				LoginFailed,
				SSLShutdownFailed,
				NoInternetConnection
			};

			using Object::Object;
		};
	}
}
