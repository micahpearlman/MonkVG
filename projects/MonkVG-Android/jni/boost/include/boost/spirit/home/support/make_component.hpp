/*=============================================================================
  Copyright (c) 2001-2010 Joel de Guzman
  http://spirit.sourceforge.net/

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_MAKE_COMPONENT_OCTOBER_16_2008_1250PM
#define BOOST_SPIRIT_MAKE_COMPONENT_OCTOBER_16_2008_1250PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/proto/proto.hpp>
#include <boost/spirit/home/support/detail/make_cons.hpp>
#include <boost/spirit/home/support/modify.hpp>

namespace boost { namespace spirit
{
    // There is no real "component" class. Each domain is responsible
    // for creating its own components. You need to specialize this for
    // each component in your domain. Use this as a guide.

    template <typename Domain, typename Tag, typename Enable = void>
    struct make_component
    {
        template <typename Sig>
        struct result;

        template <typename This, typename Elements, typename Modifiers>
        struct result<This(Elements, Modifiers)>;

        template <typename Elements, typename Modifiers>
        typename result<make_component(Elements, Modifiers)>::type
        operator()(Elements const& elements, Modifiers const& modifiers) const;
    };

    namespace tag
    {
        // Normally, we use proto tags as-is to distinguish operators.
        // The special case is proto::tag::subscript. Spirit uses this
        // as either sementic actions or directives. To distinguish between
        // the two, we use these special tags below.

        struct directive;
        struct action;
    }

    template <typename Domain, typename T, typename Enable = void>
    struct flatten_tree;
}}

namespace boost { namespace spirit { namespace detail
{
    template <typename Domain>
    struct make_terminal : proto::transform<make_terminal<Domain> >
    {
        template<typename Expr, typename State, typename Data>
        struct impl : proto::transform_impl<Expr, State, Data>
        {
            typedef typename
                proto::result_of::value<Expr>::type 
            value;

            typedef typename result_of::make_cons<value>::type elements;

            typedef
                make_component<Domain, proto::tag::terminal>
            make_component_;

            typedef typename
                make_component_::template
                    result<make_component_(elements, Data)>::type
            result_type;

            result_type operator()(
                typename impl::expr_param expr
              , typename impl::state_param /*state*/
              , typename impl::data_param data
            ) const
            {
                return typename impl::make_component_()(
                    detail::make_cons(proto::value(expr))
                  , data
                );
            }
        };
    };

    template <typename Domain, typename Tag, typename Grammar>
    struct make_unary : proto::transform<make_unary<Domain, Tag, Grammar> >
    {
        template<typename Expr, typename State, typename Data>
        struct impl : proto::transform_impl<Expr, State, Data>
        {
            typedef typename
                proto::result_of::child_c<Expr, 0>::type
            child;

            typedef typename Grammar::
                template result<Grammar(child, State, Data)>::type
            child_component;

            typedef typename
                result_of::make_cons<child_component>::type
            elements;

            typedef make_component<Domain, Tag> make_component_;

            typedef typename
                make_component_::template
                    result<make_component_(elements, Data)>::type
            result_type;

            result_type operator()(
                typename impl::expr_param expr
              , typename impl::state_param state
              , typename impl::data_param data
            ) const
            {
                return typename impl::make_component_()(
                    detail::make_cons(
                        Grammar()(proto::child(expr), state, data))
                  , data
                );
            }
        };
    };

    // un-flattened version
    template <typename Domain, typename Tag, typename Grammar,
        bool flatten = flatten_tree<Domain, Tag>::value>
    struct make_binary
    {
        template<typename Expr, typename State, typename Data>
        struct impl : proto::transform_impl<Expr, State, Data>
        {
            typedef typename Grammar::
                template result<Grammar(
                    typename proto::result_of::child_c<Expr, 0>::type
                  , State, Data)>::type
            lhs_component;

            typedef typename Grammar::
                template result<Grammar(
                    typename proto::result_of::child_c<Expr, 1>::type
                  , State, Data)>::type
            rhs_component;

            typedef typename
                result_of::make_cons<
                    lhs_component
                  , typename result_of::make_cons<rhs_component>::type
                >::type
            elements_type;

            typedef make_component<Domain, Tag> make_component_;

            typedef typename
                make_component_::template
                    result<make_component_(elements_type, Data)>::type
            result_type;

            result_type operator()(
                typename impl::expr_param expr
              , typename impl::state_param state
              , typename impl::data_param data
            ) const
            {
                elements_type elements =
                    detail::make_cons(
                        Grammar()(
                            proto::child_c<0>(expr), state, data)       // LHS
                      , detail::make_cons(
                            Grammar()(
                                proto::child_c<1>(expr), state, data)   // RHS
                        )
                    );

                return make_component_()(elements, data);
            }
        };
    };

    template <typename Grammar>
    struct make_binary_helper : proto::transform<make_binary_helper<Grammar> >
    {
        template<typename Expr, typename State, typename Data>
        struct impl : proto::transform_impl<Expr, State, Data>
        {
            typedef typename Grammar::
                template result<Grammar(Expr, State, Data)>::type
            lhs;

            typedef typename result_of::make_cons<lhs, State>::type result_type;

            result_type operator()(
                typename impl::expr_param expr
              , typename impl::state_param state
              , typename impl::data_param data
            ) const
            {
                return detail::make_cons(Grammar()(expr, state, data), state);
            }
        };
    };

    // Flattened version
    template <typename Domain, typename Tag, typename Grammar>
    struct make_binary<Domain, Tag, Grammar, true>
      : proto::transform<make_binary<Domain, Tag, Grammar> >
    {
        template<typename Expr, typename State, typename Data>
        struct impl : proto::transform_impl<Expr, State, Data>
        {
            typedef typename
                proto::reverse_fold_tree<
                    proto::_
                  , proto::make<fusion::nil>
                  , make_binary_helper<Grammar>
                >::template impl<Expr, State, Data>
            reverse_fold_tree;

            typedef typename reverse_fold_tree::result_type elements;
            typedef make_component<Domain, Tag> make_component_;

            typedef typename
                make_component_::template
                    result<make_component_(elements, Data)>::type
            result_type;

            result_type operator()(
                typename impl::expr_param expr
              , typename impl::state_param state
              , typename impl::data_param data
            ) const
            {
                return make_component_()(
                    reverse_fold_tree()(expr, state, data), data);
            }
        };
    };

    template <typename Domain, typename Grammar>
    struct make_directive : proto::transform<make_directive<Domain, Grammar> >
    {
        template<typename Expr, typename State, typename Data>
        struct impl : proto::transform_impl<Expr, State, Data>
        {
            typedef typename
                proto::result_of::child_c<Expr, 0>::type
            lhs;

            typedef typename
                proto::result_of::value<lhs>::type
            tag_type;

            typedef typename modify<Domain>::
                template result<modify<Domain>(tag_type, Data)>::type
            modifier_type;

            typedef typename Grammar::
                template result<Grammar(
                    typename proto::result_of::child_c<Expr, 1>::type
                  , State
                  , modifier_type
                )>::type
            rhs_component;

            typedef typename
                result_of::make_cons<
                    tag_type
                  , typename result_of::make_cons<rhs_component>::type
                >::type
            elements_type;

            typedef make_component<Domain, tag::directive> make_component_;

            typedef typename
                make_component_::template
                    result<make_component_(elements_type, Data)>::type
            result_type;

            result_type operator()(
                typename impl::expr_param expr
              , typename impl::state_param state
              , typename impl::data_param data
            ) const
            {
                tag_type tag = proto::value(proto::child_c<0>(expr));
                typename remove_reference<modifier_type>::type
                    modifier = modify<Domain>()(tag, data);

                elements_type elements =
                    detail::make_cons(
                        tag                                 // LHS
                      , detail::make_cons(
                            Grammar()(
                                proto::child_c<1>(expr)     // RHS
                              , state, modifier)
                        )
                    );

                return make_component_()(elements, data);
            }
        };
    };

    template <typename Domain, typename Grammar>
    struct make_action : proto::transform<make_action<Domain, Grammar> >
    {
        template<typename Expr, typename State, typename Data>
        struct impl : proto::transform_impl<Expr, State, Data>
        {
            typedef typename Grammar::
                template result<Grammar(
                    typename proto::result_of::child_c<Expr, 0>::type
                  , State
                  , Data
                )>::type
            lhs_component;

            typedef typename
                proto::result_of::value<
                    typename proto::result_of::child_c<Expr, 1>::type
                >::type
            rhs_component;

            typedef typename
                result_of::make_cons<
                    lhs_component
                  , typename result_of::make_cons<rhs_component>::type
                >::type
            elements_type;

            typedef make_component<Domain, tag::action> make_component_;

            typedef typename
                make_component_::template
                    result<make_component_(elements_type, Data)>::type
            result_type;

            result_type operator()(
                typename impl::expr_param expr
              , typename impl::state_param state
              , typename impl::data_param data
            ) const
            {
                elements_type elements =
                    detail::make_cons(
                        Grammar()(
                            proto::child_c<0>(expr), state, data)   // LHS
                      , detail::make_cons(
                            proto::value(proto::child_c<1>(expr)))  // RHS
                    );

                return make_component_()(elements, data);
            }
        };
    };
}}}

#endif
