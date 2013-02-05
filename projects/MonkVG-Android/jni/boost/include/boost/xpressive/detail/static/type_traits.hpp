///////////////////////////////////////////////////////////////////////////////
// type_traits.hpp
//
//  Copyright 2008 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_TYPE_TRAITS_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_STATIC_TYPE_TRAITS_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/mpl/bool.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>

namespace boost { namespace xpressive { namespace detail
{

///////////////////////////////////////////////////////////////////////////////
// is_static_xpression
//
template<typename T>
struct is_static_xpression
  : mpl::false_
{
};

template<typename Matcher, typename Next>
struct is_static_xpression<static_xpression<Matcher, Next> >
  : mpl::true_
{
};

template<typename Top, typename Next>
struct is_static_xpression<stacked_xpression<Top, Next> >
  : mpl::true_
{
};

//////////////////////////////////////////////////////////////////////////
// is_random
//
template<typename BidiIter>
struct is_random
  : is_convertible
    <
        typename iterator_category<BidiIter>::type
      , std::random_access_iterator_tag
    >
{
};

}}} // namespace boost::xpressive::detail

#endif
