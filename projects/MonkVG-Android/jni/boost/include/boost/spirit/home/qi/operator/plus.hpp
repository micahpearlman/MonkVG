/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_PLUS_MARCH_13_2007_0127PM)
#define SPIRIT_PLUS_MARCH_13_2007_0127PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/qi/detail/attributes.hpp>
#include <boost/spirit/home/support/info.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<qi::domain, proto::tag::unary_plus> // enables +p
      : mpl::true_ {};
}}

namespace boost { namespace spirit { namespace qi
{
    template <typename Subject>
    struct plus : unary_parser<plus<Subject> >
    {
        typedef Subject subject_type;

        template <typename Context, typename Iterator>
        struct attribute
        {
            // Build a std::vector from the subject's attribute. Note
            // that build_std_vector may return unused_type if the
            // subject's attribute is an unused_type.
            typedef typename
                traits::build_std_vector<
                    typename traits::attribute_of<
                        Subject, Context, Iterator>::type
                >::type
            type;
        };

        plus(Subject const& subject)
          : subject(subject) {}

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr) const
        {
            // create a local value if Attribute is not unused_type
            typedef typename traits::container_value<Attribute>::type 
                value_type;
            value_type val = value_type();

            Iterator save = first;
            if (!subject.parse(save, last, context, skipper, val) ||
                !traits::push_back(attr, val))
            {
                return false;
            }
            first = save;
            traits::clear(val);

            while (subject.parse(save, last, context, skipper, val))
            {
                if (!traits::push_back(attr, val))
                    break;

                first = save;
                traits::clear(val);
            }
            return true;
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("plus", subject.what(context));
        }

        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Elements, typename Modifiers>
    struct make_composite<proto::tag::unary_plus, Elements, Modifiers>
      : make_unary_composite<Elements, plus>
    {};
}}}

namespace boost { namespace spirit { namespace traits
{
    template <typename Subject>
    struct has_semantic_action<qi::plus<Subject> >
      : unary_has_semantic_action<Subject> {};
}}}

#endif
