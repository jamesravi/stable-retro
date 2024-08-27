#pragma once
#include "string.h"
#include "types.h"

namespace Common {

// this class provides enough storage room for all of these types
class Error
{
public:
  Error();
  Error(const Error& e);
  ~Error();

  enum class Type
  {
    None = 0,   // Set by default constructor, returns 'No Error'.
    User = 1   // When translated, will return 'User Error %u' if no message is specified.
  };

  // setter functions
  void Clear();
  void SetMessage(const char* msg);
  void SetFormattedMessage(const char* format, ...) printflike(2, 3);

  // get code and description, e.g. "[0x00000002]: File not Found"
  SmallString GetCodeAndMessage() const;
  void GetCodeAndMessage(String& dest) const;

  // operators
  Error& operator=(const Error& e);
  bool operator==(const Error& e) const;
  bool operator!=(const Error& e) const;

private:
  Type m_type = Type::None;
  union
  {
    int none;
    int user;
  } m_error{};
  StackString<16> m_code_string;
  TinyString m_message;
};

} // namespace Common
