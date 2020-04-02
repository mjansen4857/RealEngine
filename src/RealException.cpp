#include "RealException.h"
#include <sstream>

RealException::RealException(int line, const char* file) noexcept :
        line(line), file(file) {}

const char* RealException::what() const noexcept {
    std::ostringstream oss;
    oss << GetType() << std::endl << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* RealException::GetType() const noexcept {
    return "RealEngine Exception";
}

int RealException::GetLine() const noexcept {
    return line;
}

const std::string& RealException::GetFile() const noexcept {
    return file;
}

std::string RealException::GetOriginString() const noexcept {
    std::ostringstream oss;
    oss << "[File] " << file << std::endl
        << "[Line] " << line;
    return oss.str();
}