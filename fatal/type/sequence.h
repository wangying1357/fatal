/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_sequence_h
#define FATAL_INCLUDE_fatal_type_sequence_h

#include <fatal/container/constant_array.h>
#include <fatal/type/list.h>

#include <type_traits>

namespace fatal {

/**
 * A compile-time sequence of values for template metaprogramming.
 *
 * Most operations, unless noted otherwise, are compile-time evaluated.
 *
 * Compile-time operations have no side-effects. I.e.: Operations that would
 * mutate the sequence upon which they are performed actually create a new
 * sequence.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T... Values>
struct constant_sequence {
  /**
   * The type of this sequence's values.
   */
  using type = T;

  /**
   * This sequence as a type_list of std::integral_constant.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `type_list<
   *  //   std::integral_constant<int, 1>,
   *  //   std::integral_constant<int, 2>,
   *  //   std::integral_constant<int, 3>
   *  // >`
   *  using result = seq::list;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using list = type_list<std::integral_constant<type, Values>...>;

  /**
   * Tells how many values this sequence has.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr std::size_t size = sizeof...(Values);

  /**
   * Tells whether this sequence is empty or not.
   * This is the same as `size == 0`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static constexpr bool empty = size == 0;

  /**
   * Adds values to the beginning of this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `constant_sequence<int, 9, 1, 2, 3>`
   *  using result = seq::push_front<9>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... UValues>
  using push_front = constant_sequence<type, UValues..., Values...>;

  /**
   * Adds values to the end of this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `constant_sequence<int, 1, 2, 3, 9>`
   *  using result = seq::push_back<9>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... UValues>
  using push_back = constant_sequence<type, Values..., UValues...>;

  /**
   * Applies the values of this sequence to the given template.
   *
   * Example:
   *
   *  template <int... V> struct foo {};
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `foo<1, 2, 3>`
   *  using result = seq::apply<foo>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <type...> class U>
  using apply = U<Values...>;

  /**
   * Applies the type and values of this sequence to the given template.
   *
   * Example:
   *
   *  template <typename T, int... V> struct foo {};
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `foo<int, 1, 2, 3>`
   *  using result = seq::typed_apply<foo>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename, type...> class U>
  using typed_apply = U<type, Values...>;

  /**
   * Gets a constexpr array with the values from this sequence.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `std::array<int, 3>` with values `{1, 2, 3}`
   *  auto result = seq::array;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... Suffix>
  using array = constant_array<type, Values..., Suffix...>;

  // TODO: DOCUMENT AND TEST
  template <type... Suffix>
  static constexpr type const *data() { return array<>::data(); }

  /**
   * Gets a constexpr array with the values from this sequence,
   * followed by a 0 terminator.
   *
   * Example:
   *
   *  using seq = constant_sequence<int, 1, 2, 3>;
   *
   *  // yields `std::array<int, 4>` with values `{1, 2, 3, 0}`
   *  auto result1 = seq::z_array;
   *
   *  // yields `std::array<int, 3>` with values `"hi"`
   *  auto hi = constant_sequence<char, 'h', 'i'>::z_array;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <type... Suffix>
  using z_array = constant_array<
    type, Values..., Suffix..., static_cast<type>(0)
  >;

  // TODO: DOCUMENT AND TEST
  template <type... Suffix>
  static constexpr type const *z_data() { return z_array<>::data(); }

  // TODO: DOCUMENT AND TEST (std::vector)
  template <typename U, typename... UArgs>
  static constexpr U init(UArgs &&...args) {
    return U{Values..., std::forward<UArgs>(args)...};
  }
};
////////////////////////////////////////
// IMPLEMENTATION DETAILS DECLARATION //
////////////////////////////////////////

namespace detail {
namespace range_builder_impl {

template <bool, bool, typename T, T, T> struct build;

} // namespace constant_sequence_impl {
} // namespace detail {

/////////////////////
// SUPPORT LIBRARY //
/////////////////////

/**
 * TODO: DOCUMENT AND TEST OpenEnd
 *
 * Builds a constant_sequence with elements in the range `[Begin, End)`.
 *
 * Example:
 *
 * // yields `constant_sequence<int, 1, 2, 3, 4>`
 * typedef constant_range<int, 1, 5> result1;
 *
 * // yields `constant_sequence<int, 1, 2>`
 * typedef constant_range<int, 1, 3> result2;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T Begin, T End, bool OpenEnd = true>
using constant_range = typename detail::range_builder_impl::build<
  false, OpenEnd, T, Begin, End
>::type;

///////////////////////////////
// STATIC MEMBERS DEFINITION //
///////////////////////////////

template <typename T, T... Values>
constexpr std::size_t constant_sequence<T, Values...>::size;

template <typename T, T... Values>
constexpr bool constant_sequence<T, Values...>::empty;

///////////////////////////////////////
// IMPLEMENTATION DETAILS DEFINITION //
///////////////////////////////////////

namespace detail {
namespace range_builder_impl {

template <typename T, T End>
struct build<false, true, T, End, End> {
  using type = constant_sequence<T>;
};

template <typename T, T End>
struct build<false, false, T, End, End> {
  using type = constant_sequence<T, End>;
};

template <bool OpenEnd, typename T, T Current, T End>
struct build<false, OpenEnd, T, Current, End> {
  static_assert(Current < End, "begin must not be past end");

  using type = typename build<false, OpenEnd, T, Current + 1, End>::type
    ::template push_front<Current>;
};

} // namespace constant_sequence_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_sequence_h
