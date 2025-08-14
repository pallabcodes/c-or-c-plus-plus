# Custom Printf and Write Implementation

This library provides a Google-grade implementation of `printf` and a robust version of `write`, designed for maximum correctness, performance, and extensibility.

## Features
- Full format string parsing (width, precision, flags, types)
- Efficient output buffering
- Locale and encoding support
- Error handling and status reporting
- Output redirection
- Thread safety
- Custom data structures and algorithms
- Reference to research papers

## Structure
- `printf_parser.c/h`: Format string parser
- `buffer_manager.c/h`: Output buffering
- `formatter.c/h`: Type conversion and formatting
- `custom_printf.c/h`: Main API
- `custom_write.c/h`: Robust write implementation
- `tests/`: Unit and integration tests

## References
- [ISO C Standard](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf)
- [Efficient Buffering Algorithms](https://dl.acm.org/doi/10.1145/800195.805928)
- [Thread Safety in C Libraries](https://www.usenix.org/legacy/publications/library/proceedings/als00/full_papers/robbins/robbins.pdf)
