#pragma once

#include <tuple>

namespace intgemm {

/*
 * Sequence of unsigned integers
 *
 * Examples:
 *   sequence<1, 2, 3>()
 *   sequence_pushback<4, sequence<1, 2, 3>>() = sequence<1, 2, 3, 4>()
 *   sequence_popfront<sequence<1, 2, 3>>() = sequence<2, 3>()
 *   make_sequence<3>() = sequence<0, 1, 2>()
 */
template <unsigned... Indices>
struct sequence { using type = sequence; };

template <unsigned I, typename Sequence>
struct sequence_pushback;

template <unsigned I, unsigned... Indices>
struct sequence_pushback<I, sequence<Indices...>> : sequence<Indices..., I> {};

template <typename Sequence>
struct sequence_popfront;

template <unsigned FirstIndex, unsigned... RestIndices>
struct sequence_popfront<sequence<FirstIndex, RestIndices...>> : sequence<RestIndices...> {};

namespace { // anonymous namespace
template <unsigned N>
struct make_sequence_impl : sequence_pushback<N - 1, typename make_sequence_impl<N - 1>::type> {};
template <>
struct make_sequence_impl<0> : sequence<> {};
} // anonymous namespace

template <unsigned N>
using make_sequence = typename make_sequence_impl<N>::type;

/*
 * Make a subtuple
 */
template <typename Tuple, unsigned... Indices>
using subtuple_t = typename std::tuple<typename std::tuple_element<Indices, Tuple>::type...>;

template <typename Tuple, unsigned... Indices>
constexpr subtuple_t<Tuple, Indices...> make_subtuple(const Tuple& tuple, sequence<Indices...>) {
  return std::make_tuple(std::get<Indices>(tuple)...);
}

/*
 * Factorial
 */
constexpr unsigned long long factorial(unsigned n) {
  return n <= 1 ? 1 : n * factorial(n - 1);
}

/*
 * e^n, where n is integer
 */
namespace { // anonymous namespace
constexpr double expi_nonnegative(unsigned n) {
  return n == 0 ? 1.0 : (n == 1 ? 2.718281828459045 : expi_nonnegative(n / 2) * expi_nonnegative((n + 1) / 2));
}
} // anonymous namespace

constexpr double expi(int n) {
  return (n >= 0 ? expi_nonnegative(n) : 1.0 / expi_nonnegative(-n));
}

/*
 * Compile-time loop over range 0-N (exclusive).
 *
 * To use it, you need to create a struct inheriting from CompileTimeLoop and
 * containing static procedure 'body'. To run loop, just call static function
 * 'run' of you struct with parameters that are expected by 'body' function.
 *
 * Example of creating loop:
 *
 *   template <Index N>
 *   struct TestCompileTimeLoop : CompileTimeLoop<N, TestCompileTimeLoop> {
 *     static void body(const char* text) {
 *       std::cout << text << " " << N << std::endl;
 *     }
 *   };
 *
 * Example of running loop:
 *
 *   TestCompileTimeLoop<5>::run("Test");
 *
 * Output of the example:
 *
 *  Test 0
 *  Test 1
 *  Test 2
 *  Test 3
 *  Test 4
 *
 */
template <Index N, template <Index> class Body>
struct CompileTimeLoop {
  template <typename... Args>
  static void run(Args&&... args) {
    CompileTimeLoop<N-1, Body>::run(std::forward<Args>(args)...);
    Body<N-1>::body(std::forward<Args>(args)...);
  }
};

template <template <Index> class Body>
struct CompileTimeLoop<1, Body> {
  template <typename... Args>
  static void run(Args&&... args) {
    Body<0>::body(std::forward<Args>(args)...);
  }
};

}
