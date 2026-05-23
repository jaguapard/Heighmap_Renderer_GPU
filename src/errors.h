#pragma once
#include <string>
#include <sstream>

static void __raise_error_internal(const char* filePath, int line, std::string errorMsg)
{
    std::stringstream ss;
    ss << "Error in file: " << filePath << "\n" << "Line " << line << "\n" << errorMsg;
    throw std::runtime_error(ss.str());
}
#define RAISE_ERROR(msg) (__raise_error_internal(__FILE__, __LINE__, std::string("Error: ")+msg))
