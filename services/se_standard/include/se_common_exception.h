#ifndef SE_COMMON_EXCEPTION_H_
#define SE_COMMON_EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace OHOS {
namespace se {
enum SeErrorCode {
    SE_SUCCESS = 0,
    // SDK ERROR CODE

    // SERVICE ERROR CODE
    SE_SER_ERR_IO = 0x80000100,
    SE_SER_ERR_ILLEGAL_PARAM,
    SE_SER_ERR_ILLEGAL_STATE,
    SE_SER_ERR_NULL_POINTER,
    SE_SER_ERR_ACCESS_CONTROL,  // 0x80000105
    SE_SER_ERR_NO_SUCH_ELEMENT,
    SE_SER_ERR_SECURITY,
    SE_SER_ERR_UNSUPPORTED_OPERATION,
    SE_SER_ERR_SECURE_ELEMENT
};

class IOError : public std::runtime_error {
public:
    IOError(const std::string& what) : runtime_error(what) {}
};

class RemoteError : public std::runtime_error {
public:
    RemoteError(const std::string& what) : runtime_error(what) {}
};

class NameNotFoundError : public std::runtime_error {
public:
    NameNotFoundError(const std::string& what) : runtime_error(what) {}
};

class AccessControlError : public std::runtime_error {
public:
    AccessControlError(const std::string& what) : std::runtime_error(what) {}
};

class IllegalArgumentError : public std::runtime_error {
public:
    IllegalArgumentError(const std::string& what) : std::runtime_error(what) {}
};

class NoSuchElementError : public std::runtime_error {
public:
    NoSuchElementError(const std::string& what) : runtime_error(what) {}
};

class IllegalStateError : public std::runtime_error {
public:
    IllegalStateError(const std::string& what) : runtime_error(what) {}
};

class NullPointerError : public std::runtime_error {
public:
    NullPointerError(const std::string& what) : runtime_error(what) {}
};

class SecurityError : public std::runtime_error {
public:
    SecurityError(const std::string& what) : runtime_error(what) {}
};

class UnsupportedOperationError : public std::runtime_error {
public:
    UnsupportedOperationError(const std::string& what) : runtime_error(what) {}
};

class ServiceSpecificError : public std::runtime_error {
public:
    ServiceSpecificError(int errorCode, const std::string& what) : runtime_error(what), mErrorCode_(errorCode) {}

    ServiceSpecificError(int errorCode) : runtime_error(""), mErrorCode_(errorCode) {}

public:
    int mErrorCode_;
};

class MissingResourceError : public std::runtime_error {
public:
    MissingResourceError(const std::string& what) : runtime_error(what) {}
};

class SecureElementError : public std::runtime_error {
public:
    SecureElementError(const std::string& what) : runtime_error(what) {}
};
}  // namespace se
}  // namespace OHOS
#endif  // SE_COMMON_EXCEPTION_H_
