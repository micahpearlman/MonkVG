//  Copyright (c) 2001-2010 Hartmut Kaiser
//  Copyright (c) 2001-2010 Joel de Guzman
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_REPEAT_MAY_18_2009_0926AM)
#define SPIRIT_KARMA_REPEAT_MAY_18_2009_0926AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/detail/output_iterator.hpp>
#include <boost/spirit/home/karma/detail/get_stricttag.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/auxiliary/lazy.hpp>
#include <boost/spirit/home/karma/operator/kleene.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/karma/detail/attributes.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/fusion/include/at.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_directive<karma::domain, tag::repeat>   // enables repeat[p]
      : mpl::true_ {};

    template <typename T>
    struct use_directive<karma::domain
      , terminal_ex<tag::repeat                     // enables repeat(exact)[p]
        , fusion::vector1<T> >
    > : mpl::true_ {};

    template <typename T>
    struct use_directive<karma::domain
      , terminal_ex<tag::repeat                     // enables repeat(min, max)[p]
        , fusion::vector2<T, T> >
    > : mpl::true_ {};

    template <typename T>
    struct use_directive<karma::domain
      , terminal_ex<tag::repeat                     // enables repeat(min, inf)[p]
        , fusion::vector2<T, inf_type> >
    > : mpl::true_ {};

    template <>                                     // enables *lazy* repeat(exact)[p]
    struct use_lazy_directive<
        karma::domain
      , tag::repeat
      , 1 // arity
    > : mpl::true_ {};

    template <>                                     // enables *lazy* repeat(min, max)[p]
    struct use_lazy_directive<                      // and repeat(min, inf)[p]
        karma::domain
      , tag::repeat
      , 2 // arity
    > : mpl::true_ {};
}}

namespace boost { namespace spirit { namespace karma
{
    using spirit::repeat;
    using spirit::repeat_type;
    using spirit::inf;
    using spirit::inf_type;

    ///////////////////////////////////////////////////////////////////////////
    // handles repeat(exact)[p]
    template <typename T>
    struct exact_iterator
    {
        exact_iterator(T const exact)
          : exact(exact) {}

        typedef T type;
        T start() const { return 0; }
        bool got_max(T i) const { return i >= exact; }
        bool got_min(T i) const { return i >= exact; }

        T const exact;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        exact_iterator& operator= (exact_iterator const&);
    };

    // handles repeat(min, max)[p]
    template <typename T>
    struct finite_iterator
    {
        finite_iterator(T const min, T const max)
          : min BOOST_PREVENT_MACRO_SUBSTITUTION (min)
          , max BOOST_PREVENT_MACRO_SUBSTITUTION (max) {}

        typedef T type;
        T start() const { return 0; }
        bool got_max(T i) const { return i >= max; }
        bool got_min(T i) const { return i >= min; }

        T const min;
        T const max;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        finite_iterator& operator= (finite_iterator const&);
    };

    // handles repeat(min, inf)[p]
    template <typename T>
    struct infinite_iterator
    {
        infinite_iterator(T const min)
          : min BOOST_PREVENT_MACRO_SUBSTITUTION (min) {}

        typedef T type;
        T start() const { return 0; }
        bool got_max(T /*i*/) const { return false; }
        bool got_min(T i) const { return i >= min; }

        T const min;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        infinite_iterator& operator= (infinite_iterator const&);
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename LoopIter, typename Strict
      , typename Derived>
    struct base_repeat_generator : unary_generator<Derived>
    {
    private:
        // iterate over the given container until its exhausted or the embedded
        // (left) generator succeeds
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Iterator, typename Attribute>
        bool generate_subject(OutputIterator& sink, Context& ctx
          , Delimiter const& d, Iterator& it, Iterator& end, Attribute const&) const
        {
            // Failing subject generators are just skipped. This allows to 
            // selectively generate items in the provided attribute.
            while (!traits::compare(it, end))
            {
                if (subject.generate(sink, ctx, d, traits::deref(it)))
                    return true;
                if (Strict::value)
                    return false;
                traits::next(it);
            }
            return false;
        }

        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Iterator>
        bool generate_subject(OutputIterator& sink, Context& ctx
          , Delimiter const& d, Iterator&, Iterator&, unused_type) const
        {
            // There is no way to distinguish a failed generator from a 
            // generator to be skipped. We assume the user takes responsibility
            // for ending the loop if no attribute is specified.
            return subject.generate(sink, ctx, d, unused);
        }

    public:
        typedef Subject subject_type;

        typedef mpl::int_<subject_type::properties::value> properties;

        // Build a std::vector from the subject's attribute. Note
        // that build_std_vector may return unused_type if the
        // subject's attribute is an unused_type.
        template <typename Context, typename Iterator>
        struct attribute
          : traits::build_std_vector<
                typename traits::attribute_of<Subject, Context, Iterator>::type
            >
        {};

        base_repeat_generator(Subject const& subject, LoopIter const& iter)
          : subject(subject), iter(iter) {}

        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx, Delimiter const& d
          , Attribute const& attr) const
        {
            typedef typename traits::container_iterator<
                typename add_const<Attribute>::type
            >::type iterator_type;

            iterator_type it = traits::begin(attr);
            iterator_type end = traits::end(attr);
            typename LoopIter::type i = iter.start();

            // generate the minimal required amount of output
            for (/**/; !iter.got_min(i); ++i, traits::next(it))
            {
                if (!generate_subject(sink, ctx, d, it, end, attr))
                {
                    // if we fail before reaching the minimum iteration
                    // required, do not output anything and return false
                    return false;
                }
            }

            // generate some more up to the maximum specified
            for (/**/; detail::sink_is_good(sink) && !iter.got_max(i); 
                 ++i, traits::next(it))
            {
                if (!generate_subject(sink, ctx, d, it, end, attr))
                    break;
            }
            return detail::sink_is_good(sink);
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("repeat", subject.what(context));
        }

        Subject subject;
        LoopIter iter;
    };

    template <typename Subject, typename LoopIter>
    struct repeat_generator 
      : base_repeat_generator<
            Subject, LoopIter, mpl::false_
          , repeat_generator<Subject, LoopIter> >
    {
        typedef base_repeat_generator<
            Subject, LoopIter, mpl::false_, repeat_generator
        > base_repeat_generator_;

        repeat_generator(Subject const& subject, LoopIter const& iter)
          : base_repeat_generator_(subject, iter) {}
    };

    template <typename Subject, typename LoopIter>
    struct strict_repeat_generator 
      : base_repeat_generator<
            Subject, LoopIter, mpl::true_
          , strict_repeat_generator<Subject, LoopIter> >
    {
        typedef base_repeat_generator<
            Subject, LoopIter, mpl::true_, strict_repeat_generator
        > base_repeat_generator_;

        strict_repeat_generator(Subject const& subject, LoopIter const& iter)
          : base_repeat_generator_(subject, iter) {}
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Modifiers>
    struct make_directive<tag::repeat, Subject, Modifiers>
    {
        typedef typename mpl::if_<
            detail::get_stricttag<Modifiers>
          , strict_kleene<Subject>, kleene<Subject>
        >::type result_type;

        result_type operator()(unused_type, Subject const& subject
          , unused_type) const
        {
            return result_type(subject);
        }
    };

    template <typename T, typename Subject, typename Modifiers>
    struct make_directive<
        terminal_ex<tag::repeat, fusion::vector1<T> >, Subject, Modifiers>
    {
        typedef exact_iterator<T> iterator_type;

        typedef typename mpl::if_<
            detail::get_stricttag<Modifiers>
          , strict_repeat_generator<Subject, iterator_type>
          , repeat_generator<Subject, iterator_type>
        >::type result_type;

        template <typename Terminal>
        result_type operator()(
            Terminal const& term, Subject const& subject, unused_type) const
        {
            return result_type(subject, fusion::at_c<0>(term.args));
        }
    };

    template <typename T, typename Subject, typename Modifiers>
    struct make_directive<
        terminal_ex<tag::repeat, fusion::vector2<T, T> >, Subject, Modifiers>
    {
        typedef finite_iterator<T> iterator_type;

        typedef typename mpl::if_<
            detail::get_stricttag<Modifiers>
          , strict_repeat_generator<Subject, iterator_type>
          , repeat_generator<Subject, iterator_type>
        >::type result_type;

        template <typename Terminal>
        result_type operator()(
            Terminal const& term, Subject const& subject, unused_type) const
        {
            return result_type(subject,
                iterator_type(
                    fusion::at_c<0>(term.args)
                  , fusion::at_c<1>(term.args)
                )
            );
        }
    };

    template <typename T, typename Subject, typename Modifiers>
    struct make_directive<
        terminal_ex<tag::repeat
        , fusion::vector2<T, inf_type> >, Subject, Modifiers>
    {
        typedef infinite_iterator<T> iterator_type;

        typedef typename mpl::if_<
            detail::get_stricttag<Modifiers>
          , strict_repeat_generator<Subject, iterator_type>
          , repeat_generator<Subject, iterator_type>
        >::type result_type;

        template <typename Terminal>
        result_type operator()(
            Terminal const& term, Subject const& subject, unused_type) const
        {
            return result_type(subject, fusion::at_c<0>(term.args));
        }
    };

}}}

namespace boost { namespace spirit { namespace traits
{
    template <typename Subject, typename LoopIter>
    struct has_semantic_action<karma::repeat_generator<Subject, LoopIter> >
      : unary_has_semantic_action<Subject> {};

    template <typename Subject, typename LoopIter>
    struct has_semantic_action<karma::strict_repeat_generator<Subject, LoopIter> >
      : unary_has_semantic_action<Subject> {};
}}}

#endif
