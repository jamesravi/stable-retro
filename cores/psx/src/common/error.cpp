#include "error.h"
#include <cstdlib>
#include <cstring>
#include <type_traits>

// Platform-specific includes
#if defined(_WIN32)
#include "windows_headers.h"
#endif

namespace Common {

Error::Error() : m_type(Type::None)
{
  m_error.none = 0;
}

Error::Error(const Error& c)
{
  m_type = c.m_type;
  std::memcpy(&m_error, &c.m_error, sizeof(m_error));
  m_code_string.AppendString(c.m_code_string);
  m_message.AppendString(c.m_message);
}

Error::~Error() = default;

void Error::Clear()
{
  m_type = Type::None;
  m_error.none = 0;
  m_code_string.Clear();
  m_message.Clear();
}

void Error::SetMessage(const char* msg)
{
  m_type = Type::User;
  m_error.user = 0;
  m_code_string.Clear();
  m_message = msg;
}

void Error::SetFormattedMessage(const char* format, ...)
{
  std::va_list ap;
  va_start(ap, format);

  m_type = Type::User;
  m_error.user = 0;
  m_code_string.Clear();
  m_message.FormatVA(format, ap);
  va_end(ap);
}

Error& Error::operator=(const Error& e)
{
  m_type = e.m_type;
  std::memcpy(&m_error, &e.m_error, sizeof(m_error));
  m_code_string.Clear();
  m_code_string.AppendString(e.m_code_string);
  m_message.Clear();
  m_message.AppendString(e.m_message);
  return *this;
}

bool Error::operator==(const Error& e) const
{
  switch (m_type)
  {
    case Type::None:
      return true;
    case Type::User:
      return m_error.user == e.m_error.user;
  }

  return false;
}

bool Error::operator!=(const Error& e) const
{
  switch (m_type)
  {
    case Type::None:
      return false;
    case Type::User:
      return m_error.user != e.m_error.user;
  }

  return true;
}

SmallString Error::GetCodeAndMessage() const
{
  SmallString ret;
  GetCodeAndMessage(ret);
  return ret;
}

void Error::GetCodeAndMessage(String& dest) const
{
  if (m_code_string.IsEmpty())
    dest.Assign(m_message);
  else
    dest.Format("[%s]: %s", m_code_string.GetCharArray(), m_message.GetCharArray());
}

} // namespace Common
