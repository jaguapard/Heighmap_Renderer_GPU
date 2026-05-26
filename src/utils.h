#pragma once
#include <string>
#include <Windows.h>
#include <stdexcept>

namespace utils
{
	static std::wstring getCurrModuleFullPath()
	{
		wchar_t path[8192] = { 0 };
		HRESULT hr = GetModuleFileNameW(nullptr, path, sizeof(path) / sizeof(path[0]));
		if (FAILED(hr)) throw std::runtime_error("GetModuleFileNameW failed with HRESULT " + std::to_string(hr));
		return std::wstring(path);
	}
}