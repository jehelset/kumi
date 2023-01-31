//==================================================================================================
/*
  KUMI - Compact Tuple Tools
  Copyright : KUMI Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#pragma once

#include <cstddef>
#include <utility>

namespace kumi::detail
{
  //================================================================================================
  // We optimize layout for tuple of size 1->5 and for homogeneous layout
  // This shaves a bit of compile time and it makes symbol length of tuple NTTP shorter
  //================================================================================================

  // We usually do'nt want to optimize tuple of references
  template<typename... Ts>
  inline constexpr bool no_references = (true && ... && !std::is_reference_v<Ts>);

  // We care about homogeneous tuple
  template<typename T0, typename... Ts>
  inline constexpr bool all_the_same = (true && ... && std::is_same_v<T0,Ts>);

  //================================================================================================
  // Optimized binder for homogeneous layout if:
  //  - Size is greater than 1
  //  - All types are the same and non-reference
  //================================================================================================
  template<int... Is, typename T0, typename T1, typename... Ts>
  requires(all_the_same<T0,T1,Ts...> && no_references<T0,T1,Ts...>)
  struct binder<std::integer_sequence<int,Is...>, T0, T1, Ts...>
  {
    using kumi_unique_type = T0;
    T0 members[2+sizeof...(Ts)] = {};
  };

  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_unique_type; }
  constexpr auto& get_leaf(Binder &arg) noexcept
  {
    return arg.members[I];
  }

  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_unique_type; }
  constexpr auto&& get_leaf(Binder &&arg) noexcept
  {
    return static_cast<typename Binder::kumi_unique_type &&>(arg.members[I]);
  }

  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_unique_type; }
  constexpr auto const&& get_leaf(Binder const &&arg) noexcept
  {
    return static_cast<typename Binder::kumi_unique_type const &&>(arg.members[I]);
  }

  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_unique_type; }
  constexpr auto const& get_leaf(Binder const &arg) noexcept
  {
    return arg.members[I];
  }

  //================================================================================================
  // Optimized binder for 1->6 elements, none being reference
  //================================================================================================
  template<typename T>
  requires(no_references<T>)
  struct binder<std::integer_sequence<int,0>, T>
  {
    using kumi_specific_layout = void;
    using member0_type = T;
    member0_type member0;
  };

  template<typename T0, typename T1>
  requires(no_references<T0,T1>)
  struct binder<std::integer_sequence<int,0,1>, T0, T1>
  {
    using kumi_specific_layout = void;
    using member0_type = T0;
    using member1_type = T1;
    member0_type member0;
    member1_type member1;
  };

  template<typename T0, typename T1, typename T2>
  requires(no_references<T0,T1,T2>)
  struct binder<std::integer_sequence<int,0,1,2>, T0, T1, T2>
  {
    using kumi_specific_layout = void;
    using member0_type = T0;
    using member1_type = T1;
    using member2_type = T2;
    member0_type member0;
    member1_type member1;
    member2_type member2;
  };

  template<typename T0, typename T1, typename T2, typename T3>
  requires(no_references<T0,T1,T2,T3>)
  struct binder<std::integer_sequence<int,0,1,2,3>, T0, T1, T2, T3>
  {
    using kumi_specific_layout = void;

    using member0_type = T0;
    using member1_type = T1;
    using member2_type = T2;
    using member3_type = T3;

    member0_type member0;
    member1_type member1;
    member2_type member2;
    member3_type member3;
  };

  template<typename T0, typename T1, typename T2, typename T3, typename T4>
  requires(no_references<T0,T1,T2,T3,T4>)
  struct binder<std::integer_sequence<int,0,1,2,3,4>, T0, T1, T2, T3, T4>
  {
    using kumi_specific_layout = void;

    using member0_type = T0;
    using member1_type = T1;
    using member2_type = T2;
    using member3_type = T3;
    using member4_type = T4;

    member0_type member0;
    member1_type member1;
    member2_type member2;
    member3_type member3;
    member4_type member4;
  };

  template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
  requires(no_references<T0,T1,T2,T3,T4,T5>)
  struct binder<std::integer_sequence<int,0,1,2,3,4,5>, T0, T1, T2, T3, T4, T5>
  {
    using kumi_specific_layout = void;

    using member0_type = T0;
    using member1_type = T1;
    using member2_type = T2;
    using member3_type = T3;
    using member4_type = T4;
    using member5_type = T5;

    member0_type member0;
    member1_type member1;
    member2_type member2;
    member3_type member3;
    member4_type member4;
    member5_type member5;
  };

  //================================================================================================
  // Optimized get_leaf for all binders of 1->5 elements
  //================================================================================================
  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_specific_layout; }
  constexpr auto &get_leaf(Binder &arg) noexcept
  {
    if constexpr(I == 0) return arg.member0;
    if constexpr(I == 1) return arg.member1;
    if constexpr(I == 2) return arg.member2;
    if constexpr(I == 3) return arg.member3;
    if constexpr(I == 4) return arg.member4;
    if constexpr(I == 5) return arg.member5;
  }

  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_specific_layout; }
  constexpr auto &&get_leaf(Binder &&arg) noexcept
  {
    if constexpr(I == 0) return static_cast<typename Binder::member0_type &&>(arg.member0);
    if constexpr(I == 1) return static_cast<typename Binder::member1_type &&>(arg.member1);
    if constexpr(I == 2) return static_cast<typename Binder::member2_type &&>(arg.member2);
    if constexpr(I == 3) return static_cast<typename Binder::member3_type &&>(arg.member3);
    if constexpr(I == 4) return static_cast<typename Binder::member4_type &&>(arg.member4);
    if constexpr(I == 5) return static_cast<typename Binder::member5_type &&>(arg.member5);
  }

  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_specific_layout; }
  constexpr auto const &&get_leaf(Binder const &&arg) noexcept
  {
    if constexpr(I == 0) return static_cast<typename Binder::member0_type const&&>(arg.member0);
    if constexpr(I == 1) return static_cast<typename Binder::member1_type const&&>(arg.member1);
    if constexpr(I == 2) return static_cast<typename Binder::member2_type const&&>(arg.member2);
    if constexpr(I == 3) return static_cast<typename Binder::member3_type const&&>(arg.member3);
    if constexpr(I == 4) return static_cast<typename Binder::member4_type const&&>(arg.member4);
    if constexpr(I == 5) return static_cast<typename Binder::member5_type const&&>(arg.member5);
  }

  template<std::size_t I,typename Binder>
  requires requires(Binder) { typename Binder::kumi_specific_layout; }
  constexpr auto const &get_leaf(Binder const &arg) noexcept
  {
    if constexpr(I == 0) return arg.member0;
    if constexpr(I == 1) return arg.member1;
    if constexpr(I == 2) return arg.member2;
    if constexpr(I == 3) return arg.member3;
    if constexpr(I == 4) return arg.member4;
    if constexpr(I == 5) return arg.member5;
  }
}