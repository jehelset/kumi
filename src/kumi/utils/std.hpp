//==================================================================================================
/*
  KUMI - Compact Tuple Tools
  Copyright : KUMI Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#pragma once

#include <kumi/utils/concepts.hpp>
#include <type_traits>
#include <utility>

#if !defined(KUMI_DOXYGEN_INVOKED)
//==================================================================================================
// Structured binding adaptation
//==================================================================================================
template<std::size_t I, typename Head, typename... Tail>
struct  std::tuple_element<I, kumi::tuple<Head, Tail...>>
      : std::tuple_element<I - 1, kumi::tuple<Tail...>>
{
};

template<std::size_t I, typename... Ts> struct std::tuple_element<I, kumi::tuple<Ts...> const>
{
  using type = typename tuple_element<I, kumi::tuple<Ts...>>::type const;
};

template<typename Head, typename... Tail> struct std::tuple_element<0, kumi::tuple<Head, Tail...>>
{
  using type = Head;
};

template<typename... Ts>
struct std::tuple_size<kumi::tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
{
};

#if !defined( __ANDROID__ )
//==================================================================================================
// Common Reference support
//==================================================================================================
namespace kumi::_
{
  template< kumi::product_type TTuple,kumi::product_type UTuple
          , template<class> class TQual, template<class> class UQual
          , typename Indexer = std::make_index_sequence<kumi::size_v<TTuple>>
          >
  struct make_basic_common_ref;

  template< kumi::product_type TTuple,kumi::product_type UTuple
          , template<class> class TQual, template<class> class UQual
          , std::size_t... I
          >
  struct make_basic_common_ref<TTuple,UTuple,TQual,UQual
                              , std::index_sequence<I...>
                              >
  {
    using type = kumi::tuple<std::common_reference_t< TQual<std::tuple_element_t<I,TTuple>>
                                                    , UQual<std::tuple_element_t<I,UTuple>>
                                                    >...
                            >;
  };
}

template< kumi::product_type TTuple,kumi::product_type UTuple
        , template<class> class TQual, template<class> class UQual >
requires(kumi::size_v<TTuple> == kumi::size_v<UTuple>)
struct std::basic_common_reference<TTuple, UTuple, TQual, UQual>
: kumi::_::make_basic_common_ref<TTuple,UTuple,TQual,UQual>
{};

#endif

#endif