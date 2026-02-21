#include "LocalifyGlobal.hpp"
#include "../../string_utils.hpp"
#include <vector>
#include <sstream>

using namespace std;

namespace Localify
{
	il2cppstring GetFileName(const char* path)
	{
		string pathString = path;
		replaceAll(pathString, "\\", "/");
		stringstream pathStream(pathString);
		string segment;
		vector<string> splited;
		while (getline(pathStream, segment, '/'))
		{
			splited.emplace_back(segment);
		}
		return u8_il2cpp(splited.back());
	}
}
