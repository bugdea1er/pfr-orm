#pragma once

#include <podrm/detail/field.hpp>
#include <podrm/detail/pfr.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>

#include <boost/pfr.hpp>
#include <boost/pfr/config.hpp>

static_assert(BOOST_PFR_ENABLED, "Boost.PFR is not supported, cannot build");
static_assert(BOOST_PFR_CORE_NAME_ENABLED,
              "Boost.PFR does not support field name extraction, cannot build");

namespace podrm {

/// Identifier mode
enum class IdMode : std::uint8_t {
  Auto,   ///< Automatically generated by database
  Manual, ///< Provided when persisting
};

template <typename T> class FieldDescriptor {
public:
  [[nodiscard]] constexpr std::size_t get() const { return this->field; }

  template <const auto T::*MemberPtr>
  constexpr static FieldDescriptor fromMember() {
    return FieldDescriptor{detail::getFieldIndex<T, MemberPtr>()};
  }

private:
  std::size_t field;

  constexpr explicit FieldDescriptor(const std::size_t field) : field(field) {}
};

template <typename T, const auto MemberPtr>
  requires(std::is_member_pointer_v<decltype(MemberPtr)>)
constexpr FieldDescriptor<T> FieldOf =
    FieldDescriptor<T>::template fromMember<MemberPtr>();

template <const auto MemberPtr>
  requires(std::is_member_pointer_v<decltype(MemberPtr)>)
constexpr FieldDescriptor<detail::MemberPtrClass<MemberPtr>> Field =
    FieldDescriptor<detail::MemberPtrClass<MemberPtr>>::template fromMember<
        MemberPtr>();

template <typename T> struct EntityRegistrationData {
  FieldDescriptor<T> id;
  IdMode idMode;
};

/// ORM entity registration
/// @tparam T registered type
template <typename T>
constexpr std::optional<EntityRegistrationData<T>> EntityRegistration =
    std::nullopt;

template <typename T> struct CompositeRegistrationData {};

/// ORM composite value registration
/// @tparam T registered type
template <typename T>
constexpr std::optional<CompositeRegistrationData<T>> CompositeRegistration =
    std::nullopt;

enum class NativeType : std::uint8_t {
  BigInt,
  String,
};

template <typename T> struct ValueRegistrationData {
  NativeType nativeType;
};

/// Basic value type mapping
/// @tparam T registered value type
template <typename T>
constexpr std::optional<ValueRegistrationData<T>> ValueRegistration =
    std::nullopt;

template <>
inline constexpr auto ValueRegistration<uint64_t> =
    ValueRegistrationData<uint64_t>{
        .nativeType = NativeType::BigInt,
    };

template <>
inline constexpr auto ValueRegistration<std::string> =
    ValueRegistrationData<std::string>{
        .nativeType = NativeType::String,
    };

/// Tag to be used with boost::pfr::is_reflectable*
struct ReflectionTag;

} // namespace podrm
