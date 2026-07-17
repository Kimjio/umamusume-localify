#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <sstream>

#include "il2cpp/il2cpp-api-types.h"

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
	string u8;
	for (const wchar_t wc : wstr) {
		const auto cp = static_cast<uint32_t>(wc);
		if (cp <= 0x7f) {
			u8 += static_cast<char>(cp);
		} else if (cp <= 0x7ff) {
			u8 += static_cast<char>(0xc0 | (cp >> 6));
			u8 += static_cast<char>(0x80 | (cp & 0x3f));
		} else if (cp <= 0xffff) {
			u8 += static_cast<char>(0xe0 | (cp >> 12));
			u8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3f));
			u8 += static_cast<char>(0x80 | (cp & 0x3f));
		} else {
			u8 += static_cast<char>(0xf0 | (cp >> 18));
			u8 += static_cast<char>(0x80 | ((cp >> 12) & 0x3f));
			u8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3f));
			u8 += static_cast<char>(0x80 | (cp & 0x3f));
		}
	}
	return u8;
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
	string u8;
	for (size_t i = 0; i < str.length(); ++i) {
		uint32_t cp = str[i];
		if (cp >= 0xd800 && cp <= 0xdbff && i + 1 < str.length()) {
			const uint32_t low = str[i + 1];
			if (low >= 0xdc00 && low <= 0xdfff) {
				cp = 0x10000 + ((cp - 0xd800) << 10) + (low - 0xdc00);
				++i;
			}
		}
		if (cp <= 0x7f) {
			u8 += static_cast<char>(cp);
		} else if (cp <= 0x7ff) {
			u8 += static_cast<char>(0xc0 | (cp >> 6));
			u8 += static_cast<char>(0x80 | (cp & 0x3f));
		} else if (cp <= 0xffff) {
			u8 += static_cast<char>(0xe0 | (cp >> 12));
			u8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3f));
			u8 += static_cast<char>(0x80 | (cp & 0x3f));
		} else {
			u8 += static_cast<char>(0xf0 | (cp >> 18));
			u8 += static_cast<char>(0x80 | ((cp >> 12) & 0x3f));
			u8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3f));
			u8 += static_cast<char>(0x80 | (cp & 0x3f));
		}
	}
	return u8;
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
	wstring w;
	for (size_t i = 0; i < str.length(); ) {
		uint32_t cp = 0;
		const uint8_t c = str[i++];
		if (c <= 0x7f) {
			cp = c;
		} else if (c <= 0xdf) {
			if (i >= str.length()) break;
			cp = (c & 0x1f) << 6 | (static_cast<uint8_t>(str[i++]) & 0x3f);
		} else if (c <= 0xef) {
			if (i + 1 >= str.length()) break;
			const uint32_t b1 = static_cast<uint8_t>(str[i++]) & 0x3f;
			const uint32_t b2 = static_cast<uint8_t>(str[i++]) & 0x3f;
			cp = (c & 0x0f) << 12 | b1 << 6 | b2;
		} else {
			if (i + 2 >= str.length()) break;
			const uint32_t b1 = static_cast<uint8_t>(str[i++]) & 0x3f;
			const uint32_t b2 = static_cast<uint8_t>(str[i++]) & 0x3f;
			const uint32_t b3 = static_cast<uint8_t>(str[i++]) & 0x3f;
			cp = (c & 0x07) << 18 | b1 << 12 | b2 << 6 | b3;
		}
		w += static_cast<wchar_t>(cp);
	}
	return w;
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
	u16string u16;
	for (size_t i = 0; i < str.length(); ) {
		uint32_t cp = 0;
		const uint8_t c = str[i++];
		if (c <= 0x7f) {
			cp = c;
		} else if (c <= 0xdf) {
			if (i >= str.length()) break;
			cp = (c & 0x1f) << 6 | (static_cast<uint8_t>(str[i++]) & 0x3f);
		} else if (c <= 0xef) {
			if (i + 1 >= str.length()) break;
			const uint32_t b1 = static_cast<uint8_t>(str[i++]) & 0x3f;
			const uint32_t b2 = static_cast<uint8_t>(str[i++]) & 0x3f;
			cp = (c & 0x0f) << 12 | b1 << 6 | b2;
		} else {
			if (i + 2 >= str.length()) break;
			const uint32_t b1 = static_cast<uint8_t>(str[i++]) & 0x3f;
			const uint32_t b2 = static_cast<uint8_t>(str[i++]) & 0x3f;
			const uint32_t b3 = static_cast<uint8_t>(str[i++]) & 0x3f;
			cp = (c & 0x07) << 18 | b1 << 12 | b2 << 6 | b3;
		}

		if (cp <= 0xffff) {
			u16 += static_cast<char16_t>(cp);
		} else {
			cp -= 0x10000;
			u16 += static_cast<char16_t>(0xd800 | (cp >> 10));
			u16 += static_cast<char16_t>(0xdc00 | (cp & 0x3ff));
		}
	}
	return u16;
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

inline il2cppstring u8_il2cpp(const string& str)
{
#ifdef _MSC_VER
	il2cppstring result;
	result.resize(str.length() * 4);

	int len = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), reinterpret_cast<Il2CppChar*>(result.data()), result.length());

	result.resize(len);

	return result;
#else
	u16string u16 = u8_u16(str);
	return {u16.begin(), u16.end()};
#endif
}

inline string il2cpp_u8(const il2cppstring& str)
{
#ifdef _MSC_VER
	string result;
	result.resize(str.length() * 4);

	int len = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const Il2CppChar*>(str.data()), str.length(), result.data(), result.length(), nullptr, nullptr);

	result.resize(len);

	return result;
#else
	const u16string u16(str.begin(), str.end());
	return u16_u8(u16);
#endif
}

inline il2cppstring GetFileName(const char* path)
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
