# Endian

A small, but much needed utility library for any program that needs to handle numbers during network or file IO.

## Usage

This is a header-only library, so just place the contents of the `include` folder in your system wide or project's include directory. Then, use the library by including endian, i.e.:
```c++
#include <endian/endian.hpp>
```

### Working with buffers

Convert from Network Byte Order to Host Byte Order, e.g. when using a receive buffer.
```c++
std::vector<char> buffer;
// ... receive data into buffer
const int64_t i = endian::read<endian::order::network, int64_t>(buffer.data());
```

Convert from Host Byte Order to Network Byte Order, e.g. when using a send buffer.
```c++
std::vector<char> buffer;
const int16_t number = 42; 
endian::write<endian::order::network>(number, buffer.data());
```

### Reversing byte order

```c++
const int16_t a = 0x1234; 
const int16_t b = endian::reverse(a);
// b is 0x3412
```

Alternatively, only reverse byte order if target endianness and host's endianness differ.
```c++
const auto i = endian::conditional_reverse<endian::order::big>(42);
```

### ntoh, hton

Convenience function to conditionally convert to Network Byte Order, same as the POSIX hton function.
```c++
const auto n = endian::host_to_network(h);
```

Convenience function to conditionally convert from Network Byte Order, same as the POSIX ntoh function.
```c++
const auto h = endian::network_to_host(n);
```
