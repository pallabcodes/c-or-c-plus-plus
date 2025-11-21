//! AuroraDB Function Library
//!
//! Advanced database functions including date/time operations, string functions,
//! mathematical functions, and aggregate functions with UNIQUENESS optimizations.

pub mod date_functions;
pub mod string_functions;
pub mod math_functions;
pub mod aggregate_functions;

pub use date_functions::*;
pub use string_functions::*;
pub use math_functions::*;
pub use aggregate_functions::*;
