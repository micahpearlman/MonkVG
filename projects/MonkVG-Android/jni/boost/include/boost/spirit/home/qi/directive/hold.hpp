/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_HOLD_FEBRUARY_6_2010_0917AM)
#define SPIRIT_HOLD_FEBRUARY_6_2010_0917AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/skip_over.hpp>
#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/has_semantic_action.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_directive<qi::domain, tag::hold> // enables hold
      : mpl::true_ {};
}}

namespace boost { namespace spirit { namespace qi
{
    using spirit::hold;
    using spirit::hold_type;

    template <typename Subject>
    struct hold_directive : unary_parser<hold_directive<Subject> >
    {
        typedef Subject subject_type;
        hold_directive(Subject const& subject)
          : subject(subject) {}

        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef typename
                traits::attribute_of<subject_type, Context, Iterator>::type
            type;
        };

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper, Attribute& attr) const
        {
            Attribute copy(attr);
            if (subject.parse(first, last, context, skipper, copy))
            {
                traits::swap_impl(copy, attr);
                return true;
            }
            return false;
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("hold", subject.what(context));

        }

        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Modifiers>
    struct make_directive<tag::hold, Subject, Modifiers>
    {
        typedef hold_directive<Subject> result_type;
        result_type operator()(unused_type, Subject const& subject, unused_type) const
        {
            return result_type(subject);
        }
    };
}}}

namespace boost { namespace spirit { namespace traits
{
    template <typename Subject>
    struct has_semantic_action<qi::hold_directive<Subject> >
      : unary_has_semantic_action<Subject> {};
}}}

#endif
