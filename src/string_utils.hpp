#pragma once
#include <codecvt>
#include <string>

#ifdef _MSC_VER
#include <Windows.h>
#endif

using namespace std;

// copy-pasted from https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
inline void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

inline void replaceAll(std::u16string& str, const std::u16string& from, const std::u16string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::u16string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

inline void replaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::wstring::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

inline string wide_u8(const wstring& wstr)
{
#ifdef _MSC_VER
	string result;
	result.resize(wstr.length() * 4);

	int len = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr.length(), result.data(), result.length(), nullptr, nullptr);

	result.resize(len);

	return result;
#else
	wstring_convert<codecvt_utf8_utf16<wchar_t>, wchar_t> utf8conv;
	return utf8conv.to_bytes(wstr);
#endif
}

inline string u16_u8(const u16string& str)
{
#ifdef _MSC_VER
	string result;
	result.resize(str.length() * 4);

	int len = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(str.data()), str.length(), result.data(), result.length(), nullptr, nullptr);

	result.resize(len);

	return result;
#else
	wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
	return utf16conv.to_bytes(str);
#endif
}

inline wstring u8_wide(const string& str)
{
#ifdef _MSC_VER
	wstring result;
	result.resize(str.length() * 4);

	int len = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), result.data(), result.length());

	result.resize(len);

	return result;
#else
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> wconv;
	return wconv.from_bytes(str);
#endif
}

inline u16string u8_u16(const string& str)
{
#ifdef _MSC_VER
	u16string result;
	result.resize(str.length() * 4);

	int len = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), reinterpret_cast<wchar_t*>(result.data()), result.length());

	result.resize(len);

	return result;
#else
	wstring_convert<codecvt_utf8<char16_t>, char16_t> wconv;
	return wconv.from_bytes(str);
#endif
}

inline wstring u16_wide(const u16string& str)
{
#ifdef _MSC_VER
	return wstring(str.begin(), str.end());
#else
	return u8_wide(u16_u8(str));
#endif
}

inline u16string wide_u16(const wstring& str)
{
#ifdef _MSC_VER
	return u16string(str.begin(), str.end());
#else
	return u8_u16(wide_u8(str));
#endif
}

#ifdef _MSC_VER
inline string wide_acp(const wstring& str)
{
	string result;
	result.resize(str.length() * 4);

	int len = WideCharToMultiByte(CP_ACP, 0, str.data(), str.length(), result.data(), result.length(), nullptr, nullptr);

	result.resize(len);

	return result;
}

inline wstring acp_wide(const string& str)
{
	wstring result;
	result.resize(str.length() * 4);

	int len = MultiByteToWideChar(CP_ACP, 0, str.data(), str.length(), result.data(), result.length());

	result.resize(len);

	return result;
}
#endif
