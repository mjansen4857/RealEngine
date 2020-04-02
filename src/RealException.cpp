#include "RealException.h"
#include <sstream>

RealException::RealException(int line, const char* file) noexcept :
        line(line), file(file) {}

const char* RealException::what() const noexcept {
    std::ostringstream oss;
    oss << getType() << std::endl << getOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* RealException::getType() const noexcept {
    return "RealEngine Exception";
}

int RealException::getLine() const noexcept {
    return line;
}

const std::string& RealException::getFile() const noexcept {
    return file;
}

std::string RealException::getOriginString() const noexcept {
    std::ostringstream oss;
    oss << "[File] " << file << std::endl
        << "[Line] " << line;
    return oss.str();
}