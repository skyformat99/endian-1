#ifndef MND_ENDIAN_IMPl
#define MND_ENDIAN_IMPl

#include "../endian.hpp"
#include "type_traits.hpp"

#include <cstdint>

namespace endian {
namespace detail {

/**
 * Reads an integer of type `T` from buffer pointed to by `it` and converts it
 * from BIG endian order.
 */
template<order Order, typename T, typename InputIt>
constexpr typename std::enable_if<Order == order::big, T>::type
read(InputIt it) noexcept
{
    T h = 0;
    for(int i = 0; i < int(sizeof h); ++i)
    {
        h <<= 8;
        h |= static_cast<uint8_t>(*it++);
    }
    return h;
}

/**
 * Reads an integer of type `T` from buffer pointed to by `it` and converts it
 * from LITTLE endian order.
 */
template<order Order, typename T, typename InputIt>
constexpr typename std::enable_if<Order == order::little, T>::type
read(InputIt it) noexcept
{
    T h = 0;
    for(int i = 0; i < int(sizeof h); ++i)
    {
        h |= static_cast<uint8_t>(*it++) << i * 8;
    }
    return h;
}

/** Converts `h` to BIG endian order, writing it to buffer pointed to by `it`. */
template<order Order, typename T, typename OutputIt>
constexpr typename std::enable_if<Order == order::big, void>::type
write(const T& h, OutputIt it) noexcept
{
    for(int shift = 8 * (int(sizeof h) - 1); shift >= 0; shift -= 8)
    {
        *it++ = static_cast<uint8_t>((h >> shift) & 0xff);
    }
}

/** Converts `h` to LITTLE endian order, writing it to buffer pointed to by `it`. */
template<order Order, typename T, typename OutputIt>
constexpr typename std::enable_if<Order == order::little, void>::type
write(const T& h, OutputIt it) noexcept
{
    for(int i = 0; i < int(sizeof h); ++i)
    {
        *it++ = static_cast<uint8_t>((h >> i * 8) & 0xff);
    }
}

// --

template<size_t Size>
struct byte_swapper {};

template<>
struct byte_swapper<2>
{
    template<typename T>
    T operator()(const T& t) { return MND_BYTE_SWAP_16(t); }
};

template<>
struct byte_swapper<4>
{
    template<typename T>
    T operator()(const T& t) { return MND_BYTE_SWAP_32(t); }
};

template<>
struct byte_swapper<8>
{
    template<typename T>
    T operator()(const T& t) { return MND_BYTE_SWAP_64(t); }
};

// --

template<order Order>
struct conditional_reverser
{
    template<typename T>
    constexpr T operator()(const T& t) { return reverse(t); }
};

template<>
struct conditional_reverser<order::host>
{
    template<typename T>
    constexpr T operator()(const T& t) { return t; }
};

} // detail

template<order Order, typename T, typename InputIt>
constexpr T read(InputIt it) noexcept
{
    static_assert(detail::is_endian_reversible<T>::value,
        "T must be an integral or POD type");
    //static_assert(detail::is_input_iterator<InputIt>::value,
        //"Iterator type requirements not met");
    return detail::read<Order, T>(it);
}

template<order Order, typename T, typename OutputIt>
constexpr void write(const T& h, OutputIt it) noexcept
{
    static_assert(detail::is_endian_reversible<T>::value,
        "T must be an integral or POD type");
    //static_assert(detail::is_input_iterator<OutputIt>::value,
        //"Iterator type requirements not met");
    detail::write<Order, T>(h, it);
}

template<typename T>
constexpr T reverse(const T& t)
{
    return detail::byte_swapper<sizeof t>()(t);
}

template<order Order, typename T>
constexpr T conditional_convert(const T& t) noexcept
{
    return detail::conditional_reverser<Order>()(t);
}

template<typename T>
constexpr T host_to_network(const T& t)
{
    return conditional_convert<order::network>(t);
}

template<typename T>
constexpr T network_to_host(const T& t)
{
    // hton and ntoh are essentially the same, as they both do a byte swap if and only
    // if the host's and network's byte orders differ.
    return host_to_network(t);
}

} // endian

#endif // MND_ENDIAN_IMPl
