#pragma once
#include <string>
#include <sstream>
#include <optional>

static void __raise_error_internal(const char* filePath, int line, std::string errorMsg, std::optional<HRESULT> code = {})
{
    std::stringstream ss;
    ss << "Error in file: " << filePath << "\n" << "Line " << line << "\n" << errorMsg;
    if (code) ss << "\nError code: 0x" << std::hex << *code;
    throw std::runtime_error(ss.str());
}
#define RAISE_ERROR(msg) (__raise_error_internal(__FILE__, __LINE__, std::string("Error: ")+msg))

#define DX_RAISE_ON_FAIL(op, msg) do { \
HRESULT hr = (op);\
if (FAILED(hr)) __raise_error_internal(__FILE__, __LINE__, std::string("Message: ") + msg, hr);\
} while (0);