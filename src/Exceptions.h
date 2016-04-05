#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stdexcept>
#include <string>

namespace color {

class Exception : public std::exception {
public:
    Exception() = default;
    Exception(std::string what) : m_what(std::move(what)) {}

    virtual ~Exception() {}

    Exception(const Exception& other) = default;
    Exception(Exception&& other) = default;
    Exception& operator=(const Exception& other) = default;
    Exception& operator=(Exception&& other) = default;

    virtual const char* what() const noexcept override {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

class InvalidPackingFormatError : public Exception {
public:
    InvalidPackingFormatError(std::string what) : Exception(std::move(what)) {}
};
}

#endif
