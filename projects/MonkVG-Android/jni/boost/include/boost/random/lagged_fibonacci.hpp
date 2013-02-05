/* boost random/lagged_fibonacci.hpp header file
 *
 * Copyright Jens Maurer 2000-2001
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id: lagged_fibonacci.hpp 60755 2010-03-22 00:45:06Z steven_watanabe $
 *
 * Revision history
 *  2001-02-18  moved to individual header files
 */

#ifndef BOOST_RANDOM_LAGGED_FIBONACCI_HPP
#define BOOST_RANDOM_LAGGED_FIBONACCI_HPP

#include <boost/config/no_tr1/cmath.hpp>
#include <iostream>
#include <algorithm>     // std::max
#include <iterator>
#include <boost/config/no_tr1/cmath.hpp>         // std::pow
#include <boost/config.hpp>
#include <boost/limits.hpp>
#include <boost/cstdint.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/detail/config.hpp>
#include <boost/random/detail/seed.hpp>
#include <boost/random/detail/pass_through_engine.hpp>

namespace boost {
namespace random {

#if BOOST_WORKAROUND(_MSC_FULL_VER, BOOST_TESTED_AT(13102292)) && BOOST_MSVC > 1300
#  define BOOST_RANDOM_EXTRACT_LF
#endif

#if defined(__APPLE_CC__) && defined(__GNUC__) && (__GNUC__ == 3) && (__GNUC_MINOR__ <= 3)
#  define BOOST_RANDOM_EXTRACT_LF
#endif

#  ifdef BOOST_RANDOM_EXTRACT_LF
namespace detail
{
  template<class IStream, class F, class RealType>
  IStream&
  extract_lagged_fibonacci_01(
      IStream& is
      , F const& f
      , unsigned int& i
      , RealType* x
      , RealType modulus)
  {
        is >> i >> std::ws;
        for(unsigned int i = 0; i < f.long_lag; ++i)
        {
            RealType value;
            is >> value >> std::ws;
            x[i] = value / modulus;
        }
        return is;
  }

  template<class IStream, class F, class UIntType>
  IStream&
  extract_lagged_fibonacci(
      IStream& is
      , F const& f
      , unsigned int& i
      , UIntType* x)
  {
      is >> i >> std::ws;
      for(unsigned int i = 0; i < f.long_lag; ++i)
          is >> x[i] >> std::ws;
      return is;
  }
}
#  endif

/** 
 * Instantiations of class template \lagged_fibonacci model a
 * \pseudo_random_number_generator. It uses a lagged Fibonacci
 * algorithm with two lags @c p and @c q:
 * x(i) = x(i-p) + x(i-q) (mod 2<sup>w</sup>) with p > q.
 */
template<class UIntType, int w, unsigned int p, unsigned int q,
         UIntType val = 0>
class lagged_fibonacci
{
public:
  typedef UIntType result_type;
  BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);
  BOOST_STATIC_CONSTANT(int, word_size = w);
  BOOST_STATIC_CONSTANT(unsigned int, long_lag = p);
  BOOST_STATIC_CONSTANT(unsigned int, short_lag = q);

  /**
   * Returns: the smallest value that the generator can produce
   */
  result_type min BOOST_PREVENT_MACRO_SUBSTITUTION () const { return 0; }
  /**
   * Returns: the largest value that the generator can produce
   */
  result_type max BOOST_PREVENT_MACRO_SUBSTITUTION () const { return wordmask; }

  /**
   * Creates a new @c lagged_fibonacci generator and calls @c seed()
   */
  lagged_fibonacci() { init_wordmask(); seed(); }
  /**
   * Creates a new @c lagged_fibonacci generator and calls @c seed(value)
   */
  explicit lagged_fibonacci(uint32_t value) { init_wordmask(); seed(value); }
  /**
   * Creates a new @c lagged_fibonacci generator and calls @c seed(first, last)
   */
  template<class It> lagged_fibonacci(It& first, It last)
  { init_wordmask(); seed(first, last); }
  // compiler-generated copy ctor and assignment operator are fine

private:
  /// \cond hide_private_members
  void init_wordmask()
  {
    wordmask = 0;
    for(int j = 0; j < w; ++j)
      wordmask |= (1u << j);
  }
  /// \endcond

public:
  /**
   * Sets the state of the generator to values produced by
   * a \minstd_rand generator.
   */
  void seed(uint32_t value = 331u)
  {
    minstd_rand0 gen(value);
    for(unsigned int j = 0; j < long_lag; ++j)
      x[j] = gen() & wordmask;
    i = long_lag;
  }

  /**
   * Sets the state of the generator to values from the iterator
   * range [first, last).  If there are not enough elements in the
   * range [first, last) throws @c std::invalid_argument.
   */
  template<class It>
  void seed(It& first, It last)
  {
    // word size could be smaller than the seed values
    unsigned int j;
    for(j = 0; j < long_lag && first != last; ++j, ++first)
      x[j] = *first & wordmask;
    i = long_lag;
    if(first == last && j < long_lag)
      throw std::invalid_argument("lagged_fibonacci::seed");
  }

  /**
   * Returns: the next value of the generator
   */
  result_type operator()()
  {
    if(i >= long_lag)
      fill();
    return x[i++];
  }

  static bool validation(result_type x)
  {
    return x == val;
  }
  
#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE

#ifndef BOOST_RANDOM_NO_STREAM_OPERATORS
  template<class CharT, class Traits>
  friend std::basic_ostream<CharT,Traits>&
  operator<<(std::basic_ostream<CharT,Traits>& os, const lagged_fibonacci& f)
  {
    os << f.i << " ";
    for(unsigned int i = 0; i < f.long_lag; ++i)
      os << f.x[i] << " ";
    return os;
  }

  template<class CharT, class Traits>
  friend std::basic_istream<CharT, Traits>&
  operator>>(std::basic_istream<CharT, Traits>& is, lagged_fibonacci& f)
  {
# ifdef BOOST_RANDOM_EXTRACT_LF
      return detail::extract_lagged_fibonacci(is, f, f.i, f.x);
# else
      is >> f.i >> std::ws;
      for(unsigned int i = 0; i < f.long_lag; ++i)
          is >> f.x[i] >> std::ws;
      return is;
# endif 
  }
#endif

  friend bool operator==(const lagged_fibonacci& x, const lagged_fibonacci& y)
  { return x.i == y.i && std::equal(x.x, x.x+long_lag, y.x); }
  friend bool operator!=(const lagged_fibonacci& x,
                         const lagged_fibonacci& y)
  { return !(x == y); }
#else
  // Use a member function; Streamable concept not supported.
  bool operator==(const lagged_fibonacci& rhs) const
  { return i == rhs.i && std::equal(x, x+long_lag, rhs.x); }
  bool operator!=(const lagged_fibonacci& rhs) const
  { return !(*this == rhs); }
#endif

private:
  /// \cond hide_private_members
  void fill();
  /// \endcond

  UIntType wordmask;
  unsigned int i;
  UIntType x[long_lag];
};

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION
//  A definition is required even for integral static constants
template<class UIntType, int w, unsigned int p, unsigned int q, UIntType val>
const bool lagged_fibonacci<UIntType, w, p, q, val>::has_fixed_range;
template<class UIntType, int w, unsigned int p, unsigned int q, UIntType val>
const unsigned int lagged_fibonacci<UIntType, w, p, q, val>::long_lag;
template<class UIntType, int w, unsigned int p, unsigned int q, UIntType val>
const unsigned int lagged_fibonacci<UIntType, w, p, q, val>::short_lag;
#endif

/// \cond hide_private_members

template<class UIntType, int w, unsigned int p, unsigned int q, UIntType val>
void lagged_fibonacci<UIntType, w, p, q, val>::fill()
{
  // two loops to avoid costly modulo operations
  {  // extra scope for MSVC brokenness w.r.t. for scope
  for(unsigned int j = 0; j < short_lag; ++j)
    x[j] = (x[j] + x[j+(long_lag-short_lag)]) & wordmask;
  }
  for(unsigned int j = short_lag; j < long_lag; ++j)
    x[j] = (x[j] + x[j-short_lag]) & wordmask;
  i = 0;
}



// lagged Fibonacci generator for the range [0..1)
// contributed by Matthias Troyer
// for p=55, q=24 originally by G. J. Mitchell and D. P. Moore 1958

template<class T, unsigned int p, unsigned int q>
struct fibonacci_validation
{
  BOOST_STATIC_CONSTANT(bool, is_specialized = false);
  static T value() { return 0; }
  static T tolerance() { return 0; }
};

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION
//  A definition is required even for integral static constants
template<class T, unsigned int p, unsigned int q>
const bool fibonacci_validation<T, p, q>::is_specialized;
#endif

#define BOOST_RANDOM_FIBONACCI_VAL(T,P,Q,V,E) \
template<> \
struct fibonacci_validation<T, P, Q>  \
{                                     \
  BOOST_STATIC_CONSTANT(bool, is_specialized = true);     \
  static T value() { return V; }      \
  static T tolerance()                \
{ return (std::max)(E, static_cast<T>(5*std::numeric_limits<T>::epsilon())); } \
};
// (The extra static_cast<T> in the std::max call above is actually
// unnecessary except for HP aCC 1.30, which claims that
// numeric_limits<double>::epsilon() doesn't actually return a double.)

BOOST_RANDOM_FIBONACCI_VAL(double, 607, 273, 0.4293817707235914, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 1279, 418, 0.9421630240437659, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 2281, 1252, 0.1768114046909004, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 3217, 576, 0.1956232694868209, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 4423, 2098, 0.9499762202147172, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 9689, 5502, 0.05737836943695162, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 19937, 9842, 0.5076528587449834, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 23209, 13470, 0.5414473810619185, 1e-14)
BOOST_RANDOM_FIBONACCI_VAL(double, 44497,21034, 0.254135073399297, 1e-14)

#undef BOOST_RANDOM_FIBONACCI_VAL

/// \endcond

/**
 * Instantiations of class template @c lagged_fibonacci_01 model a
 * \pseudo_random_number_generator. It uses a lagged Fibonacci
 * algorithm with two lags @c p and @c q, evaluated in floating-point
 * arithmetic: x(i) = x(i-p) + x(i-q) (mod 1) with p > q. See
 *
 *  @blockquote
 *  "Uniform random number generators for supercomputers", Richard Brent,
 *  Proc. of Fifth Australian Supercomputer Conference, Melbourne,
 *  Dec. 1992, pp. 704-706.
 *  @endblockquote
 *
 * @xmlnote
 * The quality of the generator crucially depends on the choice
 * of the parameters. User code should employ one of the sensibly
 * parameterized generators such as \lagged_fibonacci607 instead.
 * @endxmlnote
 *
 * The generator requires considerable amounts of memory for the storage
 * of its state array. For example, \lagged_fibonacci607 requires about
 * 4856 bytes and \lagged_fibonacci44497 requires about 350 KBytes.
 */
template<class RealType, int w, unsigned int p, unsigned int q>
class lagged_fibonacci_01
{
public:
  typedef RealType result_type;
  BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);
  BOOST_STATIC_CONSTANT(int, word_size = w);
  BOOST_STATIC_CONSTANT(unsigned int, long_lag = p);
  BOOST_STATIC_CONSTANT(unsigned int, short_lag = q);

  /** Constructs a @c lagged_fibonacci_01 generator and calls @c seed(). */
  lagged_fibonacci_01() { init_modulus(); seed(); }
  /** Constructs a @c lagged_fibonacci_01 generator and calls @c seed(value). */
  BOOST_RANDOM_DETAIL_ARITHMETIC_CONSTRUCTOR(lagged_fibonacci_01, uint32_t, value)
  { init_modulus(); seed(value); }
  /** Constructs a @c lagged_fibonacci_01 generator and calls @c seed(gen). */
  BOOST_RANDOM_DETAIL_GENERATOR_CONSTRUCTOR(lagged_fibonacci_01, Generator, gen)
  { init_modulus(); seed(gen); }
  template<class It> lagged_fibonacci_01(It& first, It last)
  { init_modulus(); seed(first, last); }
  // compiler-generated copy ctor and assignment operator are fine

private:
  /// \cond hide_private_members
  void init_modulus()
  {
#ifndef BOOST_NO_STDC_NAMESPACE
    // allow for Koenig lookup
    using std::pow;
#endif
    _modulus = pow(RealType(2), word_size);
  }
  /// \endcond

public:
  /** Calls seed(331u). */
  void seed() { seed(331u); }
  /**
   * Constructs a \minstd_rand0 generator with the constructor parameter
   * value and calls seed with it. Distinct seeds in the range
   * [1, 2147483647) will produce generators with different states. Other
   * seeds will be equivalent to some seed within this range. See
   * \linear_congruential for details.
   */
  BOOST_RANDOM_DETAIL_ARITHMETIC_SEED(lagged_fibonacci_01, uint32_t, value)
  {
    minstd_rand0 intgen(value);
    seed(intgen);
  }

  /**
   * Sets the state of this @c lagged_fibonacci_01 to the values returned
   * by p invocations of \uniform_01<code>\<RealType\>()(gen)</code>.
   *
   * Complexity: Exactly p invocations of gen.
   */
  BOOST_RANDOM_DETAIL_GENERATOR_SEED(lagged_fibonacci, Generator, gen)
  {
    // use pass-by-reference, but wrap argument in pass_through_engine
    typedef detail::pass_through_engine<Generator&> ref_gen;
    uniform_01<ref_gen, RealType> gen01 =
      uniform_01<ref_gen, RealType>(ref_gen(gen));
    // I could have used std::generate_n, but it takes "gen" by value
    for(unsigned int j = 0; j < long_lag; ++j)
      x[j] = gen01();
    i = long_lag;
  }

  template<class It>
  void seed(It& first, It last)
  {
#ifndef BOOST_NO_STDC_NAMESPACE
    // allow for Koenig lookup
    using std::fmod;
    using std::pow;
#endif
    unsigned long mask = ~((~0u) << (w%32));   // now lowest w bits set
    RealType two32 = pow(RealType(2), 32);
    unsigned int j;
    for(j = 0; j < long_lag && first != last; ++j) {
      x[j] = RealType(0);
      for(int k = 0; k < w/32 && first != last; ++k, ++first)
        x[j] += *first / pow(two32,k+1);
      if(first != last && mask != 0) {
        x[j] += fmod((*first & mask) / _modulus, RealType(1));
        ++first;
      }
    }
    i = long_lag;
    if(first == last && j < long_lag)
      throw std::invalid_argument("lagged_fibonacci_01::seed");
  }

  result_type min BOOST_PREVENT_MACRO_SUBSTITUTION () const { return result_type(0); }
  result_type max BOOST_PREVENT_MACRO_SUBSTITUTION () const { return result_type(1); }

  result_type operator()()
  {
    if(i >= long_lag)
      fill();
    return x[i++];
  }

  static bool validation(result_type x)
  {
    result_type v = fibonacci_validation<result_type, p, q>::value();
    result_type epsilon = fibonacci_validation<result_type, p, q>::tolerance();
    // std::abs is a source of trouble: sometimes, it's not overloaded
    // for double, plus the usual namespace std noncompliance -> avoid it
    // using std::abs;
    // return abs(x - v) < 5 * epsilon
    return x > v - epsilon && x < v + epsilon;
  }
  
#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE

#ifndef BOOST_RANDOM_NO_STREAM_OPERATORS
  template<class CharT, class Traits>
  friend std::basic_ostream<CharT,Traits>&
  operator<<(std::basic_ostream<CharT,Traits>& os, const lagged_fibonacci_01&f)
  {
#ifndef BOOST_NO_STDC_NAMESPACE
    // allow for Koenig lookup
    using std::pow;
#endif
    os << f.i << " ";
    std::ios_base::fmtflags oldflags = os.flags(os.dec | os.fixed | os.left); 
    for(unsigned int i = 0; i < f.long_lag; ++i)
      os << f.x[i] * f._modulus << " ";
    os.flags(oldflags);
    return os;
  }

  template<class CharT, class Traits>
  friend std::basic_istream<CharT, Traits>&
  operator>>(std::basic_istream<CharT, Traits>& is, lagged_fibonacci_01& f)
    {
# ifdef BOOST_RANDOM_EXTRACT_LF
        return detail::extract_lagged_fibonacci_01(is, f, f.i, f.x, f._modulus);
# else
        is >> f.i >> std::ws;
        for(unsigned int i = 0; i < f.long_lag; ++i) {
            typename lagged_fibonacci_01::result_type value;
            is >> value >> std::ws;
            f.x[i] = value / f._modulus;
        }
        return is;
# endif 
    }
#endif

  friend bool operator==(const lagged_fibonacci_01& x,
                         const lagged_fibonacci_01& y)
  { return x.i == y.i && std::equal(x.x, x.x+long_lag, y.x); }
  friend bool operator!=(const lagged_fibonacci_01& x,
                         const lagged_fibonacci_01& y)
  { return !(x == y); }
#else
  // Use a member function; Streamable concept not supported.
  bool operator==(const lagged_fibonacci_01& rhs) const
  { return i == rhs.i && std::equal(x, x+long_lag, rhs.x); }
  bool operator!=(const lagged_fibonacci_01& rhs) const
  { return !(*this == rhs); }
#endif

private:
  /// \cond hide_private_members
  void fill();
  /// \endcond
  unsigned int i;
  RealType x[long_lag];
  RealType _modulus;
};

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION
//  A definition is required even for integral static constants
template<class RealType, int w, unsigned int p, unsigned int q>
const bool lagged_fibonacci_01<RealType, w, p, q>::has_fixed_range;
template<class RealType, int w, unsigned int p, unsigned int q>
const unsigned int lagged_fibonacci_01<RealType, w, p, q>::long_lag;
template<class RealType, int w, unsigned int p, unsigned int q>
const unsigned int lagged_fibonacci_01<RealType, w, p, q>::short_lag;
template<class RealType, int w, unsigned int p, unsigned int q>
const int lagged_fibonacci_01<RealType,w,p,q>::word_size;

#endif

/// \cond hide_private_members
template<class RealType, int w, unsigned int p, unsigned int q>
void lagged_fibonacci_01<RealType, w, p, q>::fill()
{
  // two loops to avoid costly modulo operations
  {  // extra scope for MSVC brokenness w.r.t. for scope
  for(unsigned int j = 0; j < short_lag; ++j) {
    RealType t = x[j] + x[j+(long_lag-short_lag)];
    if(t >= RealType(1))
      t -= RealType(1);
    x[j] = t;
  }
  }
  for(unsigned int j = short_lag; j < long_lag; ++j) {
    RealType t = x[j] + x[j-short_lag];
    if(t >= RealType(1))
      t -= RealType(1);
    x[j] = t;
  }
  i = 0;
}
/// \endcond

} // namespace random

#ifdef BOOST_RANDOM_DOXYGEN
namespace detail {
/**
 * The specializations lagged_fibonacci607 ... lagged_fibonacci44497
 * use well tested lags.
 *
 * See
 *
 *  @blockquote
 *  "On the Periods of Generalized Fibonacci Recurrences", Richard P. Brent
 *  Computer Sciences Laboratory Australian National University, December 1992
 *  @endblockquote
 *
 * The lags used here can be found in
 *
 *  @blockquote
 *  "Uniform random number generators for supercomputers", Richard Brent,
 *  Proc. of Fifth Australian Supercomputer Conference, Melbourne,
 *  Dec. 1992, pp. 704-706.
 *  @endblockquote
 */
struct lagged_fibonacci_doc {};
}
#endif

/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 607, 273> lagged_fibonacci607;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 1279, 418> lagged_fibonacci1279;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 2281, 1252> lagged_fibonacci2281;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 3217, 576> lagged_fibonacci3217;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 4423, 2098> lagged_fibonacci4423;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 9689, 5502> lagged_fibonacci9689;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 19937, 9842> lagged_fibonacci19937;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 23209, 13470> lagged_fibonacci23209;
/**
 * @copydoc boost::detail::lagged_fibonacci_doc
 */
typedef random::lagged_fibonacci_01<double, 48, 44497, 21034> lagged_fibonacci44497;


// It is possible to partially specialize uniform_01<> on lagged_fibonacci_01<>
// to help the compiler generate efficient code.  For GCC, this seems useless,
// because GCC optimizes (x-0)/(1-0) to (x-0).  This is good enough for now.

} // namespace boost

#endif // BOOST_RANDOM_LAGGED_FIBONACCI_HPP
