#include "Core.h"
#include <cstdint>
#include <memory>
#include <new>
#include <string>
#include <utility>

using Core = ::Core;

extern "C" {
void cxxbridge1$getCoreInstance(::std::shared_ptr<::Core> *return$) noexcept {
  ::std::shared_ptr<::Core> (*getCoreInstance$)() = ::getCoreInstance;
  new (return$) ::std::shared_ptr<::Core>(getCoreInstance$());
}

void cxxbridge1$Core$helloWorld(::Core const &self) noexcept {
  void (::Core::*helloWorld$)() const = &::Core::helloWorld;
  (self.*helloWorld$)();
}

::std::int32_t cxxbridge1$Core$takeInt(::Core const &self, ::std::string const &msg) noexcept {
  ::std::int32_t (::Core::*takeInt$)(::std::string const &) const = &::Core::takeInt;
  return (self.*takeInt$)(msg);
}

::std::string *cxxbridge1$get_empty_string() noexcept {
  ::std::unique_ptr<::std::string> (*get_empty_string$)() = ::get_empty_string;
  return get_empty_string$().release();
}

static_assert(sizeof(::std::shared_ptr<::Core>) == 2 * sizeof(void *), "");
static_assert(alignof(::std::shared_ptr<::Core>) == alignof(void *), "");
void cxxbridge1$shared_ptr$Core$null(::std::shared_ptr<::Core> *ptr) noexcept {
  ::new (ptr) ::std::shared_ptr<::Core>();
}
void cxxbridge1$shared_ptr$Core$clone(::std::shared_ptr<::Core> const &self, ::std::shared_ptr<::Core> *ptr) noexcept {
  ::new (ptr) ::std::shared_ptr<::Core>(self);
}
::Core const *cxxbridge1$shared_ptr$Core$get(::std::shared_ptr<::Core> const &self) noexcept {
  return self.get();
}
void cxxbridge1$shared_ptr$Core$drop(::std::shared_ptr<::Core> *self) noexcept {
  self->~shared_ptr();
}
} // extern "C"
