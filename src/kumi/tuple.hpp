//==================================================================================================
/*
  KUMI - Compact Tuple Tools
  Copyright : KUMI Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#ifndef KUMI_TUPLE_HPP_INCLUDED
#define KUMI_TUPLE_HPP_INCLUDED

#include <kumi/detail/binder.hpp>
#include <kumi/utils.hpp>

#include <iosfwd>

namespace kumi
{
  //================================================================================================
  //! @ingroup tuple
  //! @class tuple
  //! @brief Fixed-size collection of heterogeneous values.
  //!
  //! kumi::tuple provides an aggregate based implementation of a tuple. It provides algorithms and
  //! functions designed to facilitate tuple's handling and transformations.
  //!
  //! kumi::tuple is also compatible with standard tuple operations and structured bindings.
  //!
  //! @tparam Ts Sequence of types stored inside kumi::tuple.
  //================================================================================================
  template<typename... Ts> struct tuple
  {
    using is_product_type = void;
    detail::binder<std::make_index_sequence<sizeof...(Ts)>, Ts...> impl;

    //==============================================================================================
    //! @name Accessors
    //! @{
    //==============================================================================================

    //==============================================================================================
    //! @brief Extracts the Ith element from a kumi::tuple
    //!
    //! @note Does not participate in overload resolution if `I` is not in [0, sizeof...(Ts)).
    //! @param  i Compile-time index of the element to access
    //! @return A reference to the selected element of current tuple.
    //!
    //! ## Example:
    //! @include doc/subscript.cpp
    //==============================================================================================
    template<std::size_t I>
    requires(I < sizeof...(Ts)) constexpr decltype(auto) operator[](index_t<I>) &noexcept
    {
      return detail::get_leaf<I>(impl);
    }

    /// @overload
    template<std::size_t I>
    requires(I < sizeof...(Ts)) constexpr decltype(auto) operator[](index_t<I>) &&noexcept
    {
      return detail::get_leaf<I>(static_cast<decltype(impl) &&>(impl));
    }

    /// @overload
    template<std::size_t I>
    requires(I < sizeof...(Ts)) constexpr decltype(auto) operator[](index_t<I>) const &&noexcept
    {
      return detail::get_leaf<I>(static_cast<decltype(impl) const &&>(impl));
    }

    /// @overload
    template<std::size_t I>
    requires(I < sizeof...(Ts)) constexpr decltype(auto) operator[](index_t<I>) const &noexcept
    {
      return detail::get_leaf<I>(impl);
    }

    //==============================================================================================
    //! @brief Extracts a sub-tuple from a kumi::tuple
    //!
    //! @note Does not participate in overload resolution if `I0` and `I1` do not verify that
    //!       `0 <= I0 <= I1 <= sizeof...(Ts)`.
    //! @param  i0 Compile-time index of the first element to extract.
    //! @param  i1 Compile-time index past the last element to extract. By default, `i1` is equal to
    //!         `sizeof...(Ts)`.
    //! @return A new kumi::tuple containing to the selected elements of current tuple.
    //!
    //! ## Example:
    //! @include doc/extract.cpp
    //==============================================================================================
    template<std::size_t I0, std::size_t I1>
    requires((I1 - I0) <= sizeof...(Ts))
    [[nodiscard]] constexpr auto extract(index_t<I0> const &, index_t<I1> const &) const noexcept
    {
      return [&]<std::size_t... N>(std::index_sequence<N...>)
      {
        return tuple<std::tuple_element_t<N + I0, tuple>...> {(*this)[index<N + I0>]...};
      }
      (std::make_index_sequence<I1 - I0>());
    }

    /// @overload
    template<std::size_t I0>
    requires(I0 <= sizeof...(Ts))
    [[nodiscard]] constexpr auto extract(index_t<I0> const &) const noexcept
    {
      return [&]<std::size_t... N>(std::index_sequence<N...>)
      {
        return tuple<std::tuple_element_t<N + I0, tuple>...> {(*this)[index<N + I0>]...};
      }
      (std::make_index_sequence<sizeof...(Ts) - I0>());
    }

    //==============================================================================================
    //! @brief Split a tuple into two
    //!
    //! Split a kumi::tuple in two kumi::tuple containing all the elements before and after
    //! a given index.
    //!
    //! @note Does not participate in overload resolution if `I0` is not in `[0, sizeof...(Ts)[`.
    //!
    //! @param  i0 Compile-time index of the first element to extract.
    //! @return A new kumi::tuple containing the two sub-tuple cut at index I.
    //!
    //!
    //! ## Helper type
    //! @code
    //! namespace kumi::result
    //! {
    //!   template<std::size_t I0, product_type Tuple> struct split;
    //!
    //!   template<std::size_t I0, product_type Tuple>
    //!   using split_t = typename split<I0,Tuple>::type;
    //! }
    //! @endcode
    //!
    //! Computes the type returned by a call to split.
    //!
    //! ## Example:
    //! @include doc/split.cpp
    //==============================================================================================
    template<std::size_t I0>
    requires(I0 <= sizeof...(Ts)) [[nodiscard]] constexpr auto split(index_t<I0> const&) const noexcept;

    //==============================================================================================
    //! @}
    //==============================================================================================

    //==============================================================================================
    //! @name Properties
    //! @{
    //==============================================================================================
    /// Returns the number of elements in a kumi::tuple
    [[nodiscard]] static constexpr auto size() noexcept { return sizeof...(Ts); }

    /// Returns `true` if a kumi::tuple contains 0 elements
    [[nodiscard]] static constexpr bool empty() noexcept { return sizeof...(Ts) == 0; }

    //==============================================================================================
    //! @}
    //==============================================================================================

    //==============================================================================================
    //! @name Conversions
    //! @{
    //==============================================================================================

    //==============================================================================================
    //! @brief  Converts a tuple<Ts...> to a tuple<Us...>.
    //! @tparam Us Types composing the destination tuple
    //!
    //! ## Example:
    //! @include doc/cast.cpp
    //==============================================================================================
    template<typename... Us>
    requires(   detail::piecewise_convertible<tuple, tuple<Us...>>
            &&  (sizeof...(Us) == sizeof...(Ts))
            &&  (!std::same_as<Ts, Us> && ...)
            )
    [[nodiscard]] inline constexpr auto cast() const
    {
      return apply([](auto &&...elems) { return tuple<Us...> {static_cast<Us>(elems)...}; }, *this);
    }

    //==============================================================================================
    //! @}
    //==============================================================================================

    //==============================================================================================
    //! @brief Replaces the contents of the tuple with the contents of another tuple.
    //! @param other kumi::tuple to copy or move from
    //! @return `*this`
    //==============================================================================================
    template<typename... Us>
    requires(detail::piecewise_convertible<tuple, tuple<Us...>>) constexpr tuple &
    operator=(tuple<Us...> const &other)
    {
      [&]<std::size_t... I>(std::index_sequence<I...>) { ((get<I>(*this) = get<I>(other)), ...); }
      (std::make_index_sequence<sizeof...(Ts)>());

      return *this;
    }

    /// @overload
    template<typename... Us>
    requires(detail::piecewise_convertible<tuple, tuple<Us...>>) constexpr tuple &
    operator=(tuple<Us...> &&other)
    {
      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        ((get<I>(*this) = get<I>(KUMI_FWD(other))), ...);
      }
      (std::make_index_sequence<sizeof...(Ts)>());

      return *this;
    }

    //==============================================================================================
    //! @name Comparison operators
    //! @{
    //==============================================================================================

    /// @ingroup tuple
    /// @related kumi::tuple
    /// @brief Compares a tuple with an other kumi::product_type for equality
    template<sized_product_type<sizeof...(Ts)> Other>
    friend constexpr auto operator==(tuple const &self, Other const &other) noexcept
    requires( (sizeof...(Ts) != 0 ) && equality_comparable<tuple,Other> )
    {
      return [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        return ((get<I>(self) == get<I>(other)) && ...);
      }
      (std::make_index_sequence<sizeof...(Ts)>());
    }

#if !defined(KUMI_DOXYGEN_INVOKED)
    template<sized_product_type<0> Other>
    friend constexpr auto operator==(tuple const&, Other const &) noexcept
    {
      return true;
    }
#endif

    /// @ingroup tuple
    /// @related kumi::tuple
    /// @brief Compares a tuple with an other kumi::product_type for inequality
    template<sized_product_type<sizeof...(Ts)> Other>
    friend constexpr auto operator!=(tuple const &self, Other const &other) noexcept
    requires( (sizeof...(Ts) != 0 ) && equality_comparable<tuple,Other> )
    {
      return !(self == other);
    }

#if !defined(KUMI_DOXYGEN_INVOKED)
    template<sized_product_type<0> Other>
    friend constexpr auto operator!=(tuple const&, Other const &) noexcept
    {
      return false;
    }
#endif

    /// @ingroup tuple
    /// @related kumi::tuple
    /// @brief Compares tuple and product type value for lexicographical is less relation
    template<sized_product_type<sizeof...(Ts)> Other>
    friend constexpr auto operator<(tuple const &lhs, Other const &rhs) noexcept
    {
      // lexicographical order is defined as
      // (v0 < w0) || ... andnot(wi < vi, vi+1 < wi+1) ... || andnot(wn-1 < vn-1, vn < wn);
      auto res = get<0>(lhs) < get<0>(rhs);

      auto const order = [&]<typename Index>(Index i)
      {
        auto y_less_x_prev  = rhs[i]  < lhs[i];
        auto x_less_y       = lhs[index_t<Index::value+1>{}] < rhs[index_t<Index::value+1>{}];
        res                 = res || (x_less_y && !y_less_x_prev);
      };

      [&]<std::size_t... I>(std::index_sequence<I...>)
      {
        (order(index_t<I>{}),...);
      }
      (std::make_index_sequence<sizeof...(Ts)-1>());

      return res;
    }

    /// @ingroup tuple
    /// @related kumi::tuple
    /// @brief Compares tuple and product type value for lexicographical is less or equal relation
    template<product_type Other>
    friend constexpr auto operator<=(tuple const &lhs, Other const &rhs) noexcept
    {
      return !(rhs < lhs);
    }

    /// @ingroup tuple
    /// @related kumi::tuple
    /// @brief Compares tuple and product type value for lexicographical is greater relation
    template<product_type Other>
    friend constexpr auto operator>(tuple const &lhs, Other const &rhs) noexcept
    {
      return rhs < lhs;
    }

    /// @ingroup tuple
    /// @related kumi::tuple
    /// @brief Compares tuple and product type value for lexicographical is greater relation relation
    template<product_type Other>
    friend constexpr auto operator>=(tuple const &lhs, Other const &rhs) noexcept
    {
      return !(lhs < rhs);
    }

    //==============================================================================================
    //! @}
    //==============================================================================================

    //==============================================================================================
    //! @brief Invoke the Callable object f on each element of the current tuple.
    //!
    //! @param f	Callable object to be invoked
    //! @return The value returned by f.
    //!
    //==============================================================================================
    template<typename Function>
    constexpr decltype(auto) operator()(Function &&f) const&
    noexcept(noexcept(kumi::apply(KUMI_FWD(f), *this))) { return kumi::apply(KUMI_FWD(f), *this); }

#if !defined(KUMI_DOXYGEN_INVOKED)
    template<typename Function>
    constexpr decltype(auto) operator()(Function &&f) &
    noexcept(noexcept(kumi::apply(KUMI_FWD(f), *this)))
    {
      return kumi::apply(KUMI_FWD(f), *this);
    }

    template<typename Function>
    constexpr decltype(auto) operator()(Function &&f) const &&noexcept(
    noexcept(kumi::apply(KUMI_FWD(f), static_cast<tuple const &&>(*this))))
    {
      return kumi::apply(KUMI_FWD(f), static_cast<tuple const &&>(*this));
    }

    template<typename Function>
    constexpr decltype(auto) operator()(Function &&f) &&noexcept(
    noexcept(kumi::apply(KUMI_FWD(f), static_cast<tuple &&>(*this))))
    {
      return kumi::apply(KUMI_FWD(f), static_cast<tuple &&>(*this));
    }
#endif

    //==============================================================================================
    /// @ingroup tuple
    //! @related kumi::tuple
    //! @brief Inserts a kumi::tuple in an output stream
    //==============================================================================================
    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os,
                                                         tuple const &t) noexcept
    {
      os << "( ";
      kumi::for_each([&os](auto const &e) { os << e << " "; }, t);
      os << ")";

      return os;
    }
  };

  //================================================================================================
  //! @name Tuple construction
  //! @{
  //================================================================================================

  //================================================================================================
  //! @ingroup tuple
  //! @related kumi::tuple
  //! @brief kumi::tuple deduction guide
  //! @tparam Ts  Type lists to build the tuple with.
  //================================================================================================
  template<typename... Ts> tuple(Ts &&...) -> tuple<std::unwrap_ref_decay_t<Ts>...>;

  //================================================================================================
  //! @ingroup tuple
  //! @related kumi::tuple
  //! @brief Creates a kumi::tuple of lvalue references to its arguments.
  //! @param ts	Zero or more lvalue arguments to construct the tuple from.
  //! @return A kumi::tuple object containing lvalue references.
  //! ## Example:
  //! @include doc/tie.cpp
  //================================================================================================
  template<typename... Ts> [[nodiscard]] constexpr tuple<Ts &...> tie(Ts &...ts) { return {ts...}; }

  //================================================================================================
  //! @ingroup tuple
  //! @related kumi::tuple
  //! @brief Creates a kumi::tuple of forwarding references to its arguments.
  //!
  //! Constructs a tuple of references to the arguments in args suitable for forwarding as an
  //! argument to a function. The tuple has rvalue reference data members when rvalues are used as
  //! arguments, and otherwise has lvalue reference data members.
  //!
  //! @note If the arguments are temporaries, `forward_as_tuple` does not extend their lifetime;
  //!       they have to be used before the end of the full expression.
  //!
  //! @param ts	Zero or more lvalue arguments to construct the tuple from.
  //! @return A kumi::tuple constructed as `kumi::tuple<Ts&&...>(std::forward<Ts>(args)...)`
  //! ## Example:
  //! @include doc/forward_as_tuple.cpp
  //================================================================================================
  template<typename... Ts> [[nodiscard]] constexpr tuple<Ts &&...> forward_as_tuple(Ts &&...ts)
  {
    return {KUMI_FWD(ts)...};
  }

  //================================================================================================
  //! @ingroup tuple
  //! @related kumi::tuple
  //! @brief Creates a tuple object, deducing the target type from the types of arguments.
  //!
  //! @param ts	Zero or more lvalue arguments to construct the tuple from.
  //! @return A kumi::tuple constructed from the ts or their inner references when ts is an instance
  //!         of `std::reference_wrapper`.
  //! ## Example:
  //! @include doc/make_tuple.cpp
  //================================================================================================
  template<typename... Ts>
  [[nodiscard]] constexpr tuple<std::unwrap_ref_decay_t<Ts>...> make_tuple(Ts &&...ts)
  {
    return {KUMI_FWD(ts)...};
  }

  //================================================================================================
  //! @ingroup tuple
  //! @related kumi::tuple
  //! @brief Creates a kumi::tuple of references given a reference to a kumi::product_type.
  //!
  //! @param    t Compile-time index of the element to access
  //! @return   A tuple equivalent to the result of `kumi::apply([]<typename... T>(T&&... e)
  //!           { return kumi::forward_as_tuple(std::forward<T>(e)...); }, t)`
  //!
  //! ## Example:
  //! @include doc/to_ref.cpp
  //================================================================================================
  template<product_type Type> [[nodiscard]] constexpr auto to_ref(Type&& that)
  {
    return apply( [](auto&&... elems)
                  {
                    return kumi::forward_as_tuple(KUMI_FWD(elems)...);
                  }
                , KUMI_FWD(that)
                );
  }

  //================================================================================================
  //! @}
  //================================================================================================

  //================================================================================================
  //! @name Accessors
  //! @{
  //================================================================================================

  //================================================================================================
  //! @ingroup tuple
  //! @brief Extracts the Ith element from a kumi::tuple
  //!
  //! @note Does not participate in overload resolution if `I` is not in [0, sizeof...(Ts)).
  //! @tparam   I Compile-time index of the element to access
  //! @param    t Compile-time index of the element to access
  //! @return   A reference to the selected element of t.
  //! @related kumi::tuple
  //!
  //! ## Example:
  //! @include doc/get.cpp
  //================================================================================================
  template<std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts)) [[nodiscard]] constexpr decltype(auto) get(tuple<Ts...> &arg) noexcept
  {
    return arg[index<I>];
  }

  /// @overload
  template<std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts)) [[nodiscard]] constexpr decltype(auto)
  get(tuple<Ts...> &&arg) noexcept
  {
    return static_cast<tuple<Ts...> &&>(arg)[index<I>];
  }

  /// @overload
  template<std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts)) [[nodiscard]] constexpr decltype(auto)
  get(tuple<Ts...> const &arg) noexcept
  {
    return arg[index<I>];
  }

  /// @overload
  template<std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts)) [[nodiscard]] constexpr decltype(auto)
  get(tuple<Ts...> const &&arg) noexcept
  {
    return static_cast<tuple<Ts...> const &&>(arg)[index<I>];
  }

  //================================================================================================
  //! @}
  //================================================================================================

  template<typename... Ts>
  template<std::size_t I0>
  requires(I0 <= sizeof...(Ts))
  [[nodiscard]] constexpr auto tuple<Ts...>::split(index_t<I0> const &) const noexcept
  {
    return kumi::make_tuple(extract(index<0>, index<I0>), extract(index<I0>));
  }

  namespace result
  {
    template<product_type T, std::size_t I0> struct split
    {
      using type = decltype ( std::declval<T>().split(kumi::index_t<I0>{}) );
    };

    template<product_type T, std::size_t I0> using split_t = typename split<T,I0>::type;
  }
}

#include <kumi/algorithm.hpp>

#undef KUMI_FWD
#endif