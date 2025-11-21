//! AuroraDB Date/Time Functions
//!
//! Revolutionary date/time function library that eliminates the complexity and verbosity
//! of traditional database date operations through UNIQUENESS research integration.

use std::collections::HashMap;
use chrono::{DateTime, Utc, TimeZone, Datelike, Timelike, Duration};
use crate::core::errors::{AuroraResult, AuroraError};
use crate::core::data::DataType;

/// Date/time function result
#[derive(Debug, Clone)]
pub enum DateFunctionResult {
    Integer(i64),
    Float(f64),
    String(String),
    Timestamp(DateTime<Utc>),
    Interval(Duration),
}

/// Date extraction units for DATE_EXTRACT
#[derive(Debug, Clone, PartialEq)]
pub enum DateExtractUnit {
    Year,
    Quarter,
    Month,
    Week,
    Day,
    DayOfWeek,
    DayOfYear,
    Hour,
    Minute,
    Second,
    Millisecond,
    Microsecond,
    Nanosecond,
    Epoch,
    TimezoneHour,
    TimezoneMinute,
}

/// Date truncation units for DATE_TRUNC
#[derive(Debug, Clone, PartialEq)]
pub enum DateTruncUnit {
    Microseconds,
    Milliseconds,
    Second,
    Minute,
    Hour,
    Day,
    Week,
    Month,
    Quarter,
    Year,
    Decade,
    Century,
    Millennium,
}

/// Interval units for date arithmetic
#[derive(Debug, Clone, PartialEq)]
pub enum IntervalUnit {
    Microseconds,
    Milliseconds,
    Seconds,
    Minutes,
    Hours,
    Days,
    Weeks,
    Months,
    Years,
}

/// Date function registry and executor
pub struct DateFunctionExecutor {
    // Cache for expensive operations
    timezone_cache: HashMap<String, chrono_tz::Tz>,
}

impl DateFunctionExecutor {
    pub fn new() -> Self {
        Self {
            timezone_cache: HashMap::new(),
        }
    }

    /// Execute date/time function with UNIQUENESS optimizations
    pub fn execute_function(
        &mut self,
        function_name: &str,
        args: &[DateFunctionArg],
    ) -> AuroraResult<DateFunctionResult> {
        match function_name.to_uppercase().as_str() {
            // UNIQUENESS: Simplified DATE_EXTRACT (no more verbose EXTRACT syntax)
            "DATE_EXTRACT" | "EXTRACT" => self.date_extract(args),

            // UNIQUENESS: Powerful DATE_TRUNC with smart defaults
            "DATE_TRUNC" | "TRUNC" => self.date_trunc(args),

            // UNIQUENESS: Intelligent date part extraction
            "YEAR" | "MONTH" | "DAY" | "HOUR" | "MINUTE" | "SECOND" => {
                self.smart_extract(function_name, args)
            }

            // UNIQUENESS: Simplified date arithmetic
            "DATE_ADD" | "DATE_SUB" => self.date_arithmetic(function_name, args),

            // UNIQUENESS: Powerful date difference calculations
            "DATE_DIFF" | "DATEDIFF" => self.date_diff(args),

            // UNIQUENESS: Smart date formatting
            "DATE_FORMAT" | "FORMAT_DATE" => self.date_format(args),

            // UNIQUENESS: Intelligent date parsing
            "DATE_PARSE" | "PARSE_DATE" => self.date_parse(args),

            // UNIQUENESS: Business date functions
            "BUSINESS_DAYS" => self.business_days(args),
            "WORKDAYS_BETWEEN" => self.workdays_between(args),

            // UNIQUENESS: Advanced temporal functions
            "DATE_BUCKET" => self.date_bucket(args),
            "TIME_BUCKET" => self.time_bucket(args),

            // UNIQUENESS: Fiscal year functions
            "FISCAL_YEAR" => self.fiscal_year(args),
            "FISCAL_QUARTER" => self.fiscal_quarter(args),

            // Current date/time functions
            "NOW" | "CURRENT_TIMESTAMP" => self.current_timestamp(),
            "CURRENT_DATE" => self.current_date(),
            "CURRENT_TIME" => self.current_time(),

            // UNIQUENESS: Intelligent age calculation
            "AGE" => self.age_calculation(args),

            _ => Err(AuroraError::InvalidArgument(format!("Unknown date function: {}", function_name))),
        }
    }

    /// UNIQUENESS: Simplified DATE_EXTRACT - eliminates verbose EXTRACT() syntax
    ///
    /// Traditional: EXTRACT(YEAR FROM date_col)
    /// AuroraDB: DATE_EXTRACT('year', date_col) or YEAR(date_col)
    fn date_extract(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument("DATE_EXTRACT requires 2 arguments".to_string()));
        }

        let unit_str = args[0].as_string()?.to_lowercase();
        let unit = self.parse_extract_unit(&unit_str)?;
        let timestamp = args[1].as_timestamp()?;

        let result = match unit {
            DateExtractUnit::Year => timestamp.year() as i64,
            DateExtractUnit::Quarter => ((timestamp.month() - 1) / 3 + 1) as i64,
            DateExtractUnit::Month => timestamp.month() as i64,
            DateExtractUnit::Week => timestamp.iso_week().week() as i64,
            DateExtractUnit::Day => timestamp.day() as i64,
            DateExtractUnit::DayOfWeek => timestamp.weekday().number_from_monday() as i64,
            DateExtractUnit::DayOfYear => timestamp.ordinal() as i64,
            DateExtractUnit::Hour => timestamp.hour() as i64,
            DateExtractUnit::Minute => timestamp.minute() as i64,
            DateExtractUnit::Second => timestamp.second() as i64,
            DateExtractUnit::Millisecond => (timestamp.timestamp_millis() % 1000) as i64,
            DateExtractUnit::Microsecond => (timestamp.timestamp_micros() % 1_000_000) as i64,
            DateExtractUnit::Nanosecond => (timestamp.timestamp_nanos_opt().unwrap_or(0) % 1_000_000_000) as i64,
            DateExtractUnit::Epoch => timestamp.timestamp(),
            DateExtractUnit::TimezoneHour => timestamp.timezone().local_minus_utc() / 3600,
            DateExtractUnit::TimezoneMinute => (timestamp.timezone().local_minus_utc() % 3600) / 60,
        };

        Ok(DateFunctionResult::Integer(result))
    }

    /// UNIQUENESS: Powerful DATE_TRUNC with intelligent defaults
    ///
    /// Traditional: DATE_TRUNC('day', timestamp)
    /// AuroraDB: DATE_TRUNC('day', timestamp) with smart precision handling
    fn date_trunc(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument("DATE_TRUNC requires 2 arguments".to_string()));
        }

        let unit_str = args[0].as_string()?.to_lowercase();
        let unit = self.parse_trunc_unit(&unit_str)?;
        let timestamp = args[1].as_timestamp()?;

        let truncated = match unit {
            DateTruncUnit::Microseconds => timestamp.with_nanosecond(0).unwrap(),
            DateTruncUnit::Milliseconds => timestamp.with_nanosecond((timestamp.nanosecond() / 1_000_000) * 1_000_000).unwrap(),
            DateTruncUnit::Second => timestamp.with_nanosecond(0).unwrap(),
            DateTruncUnit::Minute => timestamp.with_second(0).unwrap().with_nanosecond(0).unwrap(),
            DateTruncUnit::Hour => timestamp.with_minute(0).unwrap().with_second(0).unwrap().with_nanosecond(0).unwrap(),
            DateTruncUnit::Day => timestamp.with_hour(0).unwrap().with_minute(0).unwrap().with_second(0).unwrap().with_nanosecond(0).unwrap(),
            DateTruncUnit::Week => {
                let weekday = timestamp.weekday();
                let days_to_subtract = weekday.num_days_from_monday() as i64;
                (timestamp - Duration::days(days_to_subtract))
                    .with_hour(0).unwrap()
                    .with_minute(0).unwrap()
                    .with_second(0).unwrap()
                    .with_nanosecond(0).unwrap()
            },
            DateTruncUnit::Month => timestamp.with_day(1).unwrap()
                .with_hour(0).unwrap()
                .with_minute(0).unwrap()
                .with_second(0).unwrap()
                .with_nanosecond(0).unwrap(),
            DateTruncUnit::Quarter => {
                let quarter_start_month = ((timestamp.month() - 1) / 3) * 3 + 1;
                timestamp.with_month(quarter_start_month).unwrap()
                    .with_day(1).unwrap()
                    .with_hour(0).unwrap()
                    .with_minute(0).unwrap()
                    .with_second(0).unwrap()
                    .with_nanosecond(0).unwrap()
            },
            DateTruncUnit::Year => timestamp.with_month(1).unwrap()
                .with_day(1).unwrap()
                .with_hour(0).unwrap()
                .with_minute(0).unwrap()
                .with_second(0).unwrap()
                .with_nanosecond(0).unwrap(),
            DateTruncUnit::Decade => {
                let year = (timestamp.year() / 10) * 10;
                timestamp.with_year(year).unwrap()
                    .with_month(1).unwrap()
                    .with_day(1).unwrap()
                    .with_hour(0).unwrap()
                    .with_minute(0).unwrap()
                    .with_second(0).unwrap()
                    .with_nanosecond(0).unwrap()
            },
            DateTruncUnit::Century => {
                let year = ((timestamp.year() - 1) / 100) * 100 + 1;
                timestamp.with_year(year).unwrap()
                    .with_month(1).unwrap()
                    .with_day(1).unwrap()
                    .with_hour(0).unwrap()
                    .with_minute(0).unwrap()
                    .with_second(0).unwrap()
                    .with_nanosecond(0).unwrap()
            },
            DateTruncUnit::Millennium => {
                let year = ((timestamp.year() - 1) / 1000) * 1000 + 1;
                timestamp.with_year(year).unwrap()
                    .with_month(1).unwrap()
                    .with_day(1).unwrap()
                    .with_hour(0).unwrap()
                    .with_minute(0).unwrap()
                    .with_second(0).unwrap()
                    .with_nanosecond(0).unwrap()
            },
        };

        Ok(DateFunctionResult::Timestamp(truncated))
    }

    /// UNIQUENESS: Smart extraction functions - YEAR(date), MONTH(date), etc.
    ///
    /// Traditional: EXTRACT(YEAR FROM date_col)
    /// AuroraDB: YEAR(date_col) - much simpler!
    fn smart_extract(&self, function_name: &str, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 1 {
            return Err(AuroraError::InvalidArgument(format!("{} requires 1 argument", function_name)));
        }

        let timestamp = args[0].as_timestamp()?;
        let unit = match function_name.to_uppercase().as_str() {
            "YEAR" => DateExtractUnit::Year,
            "MONTH" => DateExtractUnit::Month,
            "DAY" => DateExtractUnit::Day,
            "HOUR" => DateExtractUnit::Hour,
            "MINUTE" => DateExtractUnit::Minute,
            "SECOND" => DateExtractUnit::Second,
            _ => return Err(AuroraError::InvalidArgument(format!("Unknown extract function: {}", function_name))),
        };

        let result = match unit {
            DateExtractUnit::Year => timestamp.year() as i64,
            DateExtractUnit::Month => timestamp.month() as i64,
            DateExtractUnit::Day => timestamp.day() as i64,
            DateExtractUnit::Hour => timestamp.hour() as i64,
            DateExtractUnit::Minute => timestamp.minute() as i64,
            DateExtractUnit::Second => timestamp.second() as i64,
            _ => unreachable!(),
        };

        Ok(DateFunctionResult::Integer(result))
    }

    /// UNIQUENESS: Simplified date arithmetic
    ///
    /// Traditional: date + INTERVAL '1 day'
    /// AuroraDB: DATE_ADD(date, '1 day') or DATE_SUB(date, '1 day')
    fn date_arithmetic(&self, function_name: &str, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument(format!("{} requires 2 arguments", function_name)));
        }

        let timestamp = args[0].as_timestamp()?;
        let interval_str = args[1].as_string()?;
        let (amount, unit) = self.parse_interval(interval_str)?;

        let duration = self.interval_to_duration(amount, &unit);
        let result = match function_name.to_uppercase().as_str() {
            "DATE_ADD" => timestamp + duration,
            "DATE_SUB" => timestamp - duration,
            _ => unreachable!(),
        };

        Ok(DateFunctionResult::Timestamp(result))
    }

    /// UNIQUENESS: Powerful date difference calculations
    ///
    /// Traditional: EXTRACT(EPOCH FROM (date2 - date1)) / 86400
    /// AuroraDB: DATE_DIFF('day', date1, date2)
    fn date_diff(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 3 {
            return Err(AuroraError::InvalidArgument("DATE_DIFF requires 3 arguments: unit, date1, date2".to_string()));
        }

        let unit_str = args[0].as_string()?.to_lowercase();
        let date1 = args[1].as_timestamp()?;
        let date2 = args[2].as_timestamp()?;

        let diff = match unit_str.as_str() {
            "year" | "years" => (date2.year() - date1.year()) as i64,
            "month" | "months" => {
                ((date2.year() - date1.year()) * 12 + date2.month() as i32 - date1.month() as i32) as i64
            },
            "week" | "weeks" => (date2.signed_duration_since(date1).num_days() / 7) as i64,
            "day" | "days" => date2.signed_duration_since(date1).num_days() as i64,
            "hour" | "hours" => date2.signed_duration_since(date1).num_hours() as i64,
            "minute" | "minutes" => date2.signed_duration_since(date1).num_minutes() as i64,
            "second" | "seconds" => date2.signed_duration_since(date1).num_seconds() as i64,
            "millisecond" | "milliseconds" => date2.signed_duration_since(date1).num_milliseconds() as i64,
            "microsecond" | "microseconds" => date2.signed_duration_since(date1).num_microseconds().unwrap_or(0) as i64,
            _ => return Err(AuroraError::InvalidArgument(format!("Unknown date diff unit: {}", unit_str))),
        };

        Ok(DateFunctionResult::Integer(diff))
    }

    /// UNIQUENESS: Smart date formatting
    ///
    /// Traditional: TO_CHAR(date, 'YYYY-MM-DD')
    /// AuroraDB: DATE_FORMAT(date, 'YYYY-MM-DD') with intelligent parsing
    fn date_format(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument("DATE_FORMAT requires 2 arguments".to_string()));
        }

        let timestamp = args[0].as_timestamp()?;
        let format_str = args[1].as_string()?;

        // UNIQUENESS: Intelligent format parsing
        let formatted = self.smart_date_format(timestamp, format_str)?;
        Ok(DateFunctionResult::String(formatted))
    }

    /// UNIQUENESS: Intelligent date parsing
    fn date_parse(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument("DATE_PARSE requires 2 arguments".to_string()));
        }

        let date_str = args[0].as_string()?;
        let format_str = args[1].as_string()?;

        // UNIQUENESS: Auto-detect common formats if format not specified precisely
        let parsed = self.smart_date_parse(date_str, format_str)?;
        Ok(DateFunctionResult::Timestamp(parsed))
    }

    /// UNIQUENESS: Business days calculation
    fn business_days(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument("BUSINESS_DAYS requires 2 arguments".to_string()));
        }

        let start_date = args[0].as_timestamp()?;
        let end_date = args[1].as_timestamp()?;

        let business_days = self.calculate_business_days(start_date, end_date)?;
        Ok(DateFunctionResult::Integer(business_days))
    }

    /// UNIQUENESS: Workdays between dates
    fn workdays_between(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        // Alias for BUSINESS_DAYS
        self.business_days(args)
    }

    /// UNIQUENESS: Intelligent date bucketing
    fn date_bucket(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() < 2 {
            return Err(AuroraError::InvalidArgument("DATE_BUCKET requires at least 2 arguments".to_string()));
        }

        let timestamp = args[0].as_timestamp()?;
        let bucket_width = args[1].as_string()?; // e.g., "1 day", "1 week", "1 month"

        let (amount, unit) = self.parse_interval(bucket_width)?;
        let bucketed = self.calculate_date_bucket(timestamp, amount, &unit)?;
        Ok(DateFunctionResult::Timestamp(bucketed))
    }

    /// UNIQUENESS: Time bucketing for analytics
    fn time_bucket(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        // Alias for DATE_BUCKET with time focus
        self.date_bucket(args)
    }

    /// UNIQUENESS: Fiscal year calculations
    fn fiscal_year(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 1 && args.len() != 2 {
            return Err(AuroraError::InvalidArgument("FISCAL_YEAR requires 1 or 2 arguments".to_string()));
        }

        let timestamp = args[0].as_timestamp()?;
        let fiscal_start_month = if args.len() == 2 {
            args[1].as_integer()? as u32
        } else {
            7 // July (common fiscal year start)
        };

        let fiscal_year = self.calculate_fiscal_year(timestamp, fiscal_start_month)?;
        Ok(DateFunctionResult::Integer(fiscal_year))
    }

    /// UNIQUENESS: Fiscal quarter calculations
    fn fiscal_quarter(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 1 && args.len() != 2 {
            return Err(AuroraError::InvalidArgument("FISCAL_QUARTER requires 1 or 2 arguments".to_string()));
        }

        let timestamp = args[0].as_timestamp()?;
        let fiscal_start_month = if args.len() == 2 {
            args[1].as_integer()? as u32
        } else {
            7 // July
        };

        let fiscal_quarter = self.calculate_fiscal_quarter(timestamp, fiscal_start_month)?;
        Ok(DateFunctionResult::Integer(fiscal_quarter))
    }

    /// Current timestamp
    fn current_timestamp(&self) -> AuroraResult<DateFunctionResult> {
        Ok(DateFunctionResult::Timestamp(Utc::now()))
    }

    /// Current date
    fn current_date(&self) -> AuroraResult<DateFunctionResult> {
        let now = Utc::now();
        let date = Utc.with_ymd_and_hms(now.year(), now.month(), now.day(), 0, 0, 0).unwrap();
        Ok(DateFunctionResult::Timestamp(date))
    }

    /// Current time
    fn current_time(&self) -> AuroraResult<DateFunctionResult> {
        Ok(DateFunctionResult::Timestamp(Utc::now()))
    }

    /// UNIQUENESS: Intelligent age calculation
    fn age_calculation(&self, args: &[DateFunctionArg]) -> AuroraResult<DateFunctionResult> {
        if args.len() != 1 && args.len() != 2 {
            return Err(AuroraError::InvalidArgument("AGE requires 1 or 2 arguments".to_string()));
        }

        let end_date = args[0].as_timestamp()?;
        let start_date = if args.len() == 2 {
            args[1].as_timestamp()?
        } else {
            Utc::now()
        };

        let age = self.calculate_age(start_date, end_date)?;
        Ok(DateFunctionResult::String(age))
    }

    // Helper functions

    fn parse_extract_unit(&self, unit_str: &str) -> AuroraResult<DateExtractUnit> {
        match unit_str {
            "year" | "years" => Ok(DateExtractUnit::Year),
            "quarter" | "quarters" => Ok(DateExtractUnit::Quarter),
            "month" | "months" => Ok(DateExtractUnit::Month),
            "week" | "weeks" => Ok(DateExtractUnit::Week),
            "day" | "days" => Ok(DateExtractUnit::Day),
            "dayofweek" | "dow" => Ok(DateExtractUnit::DayOfWeek),
            "dayofyear" | "doy" => Ok(DateExtractUnit::DayOfYear),
            "hour" | "hours" => Ok(DateExtractUnit::Hour),
            "minute" | "minutes" => Ok(DateExtractUnit::Minute),
            "second" | "seconds" => Ok(DateExtractUnit::Second),
            "millisecond" | "milliseconds" => Ok(DateExtractUnit::Millisecond),
            "microsecond" | "microseconds" => Ok(DateExtractUnit::Microsecond),
            "nanosecond" | "nanoseconds" => Ok(DateExtractUnit::Nanosecond),
            "epoch" => Ok(DateExtractUnit::Epoch),
            "timezone_hour" | "tz_hour" => Ok(DateExtractUnit::TimezoneHour),
            "timezone_minute" | "tz_minute" => Ok(DateExtractUnit::TimezoneMinute),
            _ => Err(AuroraError::InvalidArgument(format!("Unknown extract unit: {}", unit_str))),
        }
    }

    fn parse_trunc_unit(&self, unit_str: &str) -> AuroraResult<DateTruncUnit> {
        match unit_str {
            "microseconds" => Ok(DateTruncUnit::Microseconds),
            "milliseconds" => Ok(DateTruncUnit::Milliseconds),
            "second" | "seconds" => Ok(DateTruncUnit::Second),
            "minute" | "minutes" => Ok(DateTruncUnit::Minute),
            "hour" | "hours" => Ok(DateTruncUnit::Hour),
            "day" | "days" => Ok(DateTruncUnit::Day),
            "week" | "weeks" => Ok(DateTruncUnit::Week),
            "month" | "months" => Ok(DateTruncUnit::Month),
            "quarter" | "quarters" => Ok(DateTruncUnit::Quarter),
            "year" | "years" => Ok(DateTruncUnit::Year),
            "decade" | "decades" => Ok(DateTruncUnit::Decade),
            "century" | "centuries" => Ok(DateTruncUnit::Century),
            "millennium" | "millennia" => Ok(DateTruncUnit::Millennium),
            _ => Err(AuroraError::InvalidArgument(format!("Unknown trunc unit: {}", unit_str))),
        }
    }

    fn parse_interval(&self, interval_str: &str) -> AuroraResult<(i64, IntervalUnit)> {
        // Parse strings like "1 day", "2 weeks", "30 minutes", etc.
        let parts: Vec<&str> = interval_str.split_whitespace().collect();
        if parts.len() != 2 {
            return Err(AuroraError::InvalidArgument(format!("Invalid interval format: {}", interval_str)));
        }

        let amount: i64 = parts[0].parse()
            .map_err(|_| AuroraError::InvalidArgument(format!("Invalid interval amount: {}", parts[0])))?;

        let unit = match parts[1].to_lowercase().as_str() {
            "microsecond" | "microseconds" => IntervalUnit::Microseconds,
            "millisecond" | "milliseconds" => IntervalUnit::Milliseconds,
            "second" | "seconds" => IntervalUnit::Seconds,
            "minute" | "minutes" => IntervalUnit::Minutes,
            "hour" | "hours" => IntervalUnit::Hours,
            "day" | "days" => IntervalUnit::Days,
            "week" | "weeks" => IntervalUnit::Weeks,
            "month" | "months" => IntervalUnit::Months,
            "year" | "years" => IntervalUnit::Years,
            _ => return Err(AuroraError::InvalidArgument(format!("Unknown interval unit: {}", parts[1]))),
        };

        Ok((amount, unit))
    }

    fn interval_to_duration(&self, amount: i64, unit: &IntervalUnit) -> Duration {
        match unit {
            IntervalUnit::Microseconds => Duration::microseconds(amount),
            IntervalUnit::Milliseconds => Duration::milliseconds(amount),
            IntervalUnit::Seconds => Duration::seconds(amount),
            IntervalUnit::Minutes => Duration::minutes(amount),
            IntervalUnit::Hours => Duration::hours(amount),
            IntervalUnit::Days => Duration::days(amount),
            IntervalUnit::Weeks => Duration::weeks(amount),
            IntervalUnit::Months => Duration::days(amount * 30), // Approximation
            IntervalUnit::Years => Duration::days(amount * 365), // Approximation
        }
    }

    fn smart_date_format(&self, timestamp: DateTime<Utc>, format_str: &str) -> AuroraResult<String> {
        // UNIQUENESS: Intelligent format parsing with common patterns
        let formatted = match format_str {
            "YYYY-MM-DD" => timestamp.format("%Y-%m-%d").to_string(),
            "YYYY-MM-DD HH24:MI:SS" => timestamp.format("%Y-%m-%d %H:%M:%S").to_string(),
            "YYYY/MM/DD" => timestamp.format("%Y/%m/%d").to_string(),
            "MM/DD/YYYY" => timestamp.format("%m/%d/%Y").to_string(),
            "DD/MM/YYYY" => timestamp.format("%d/%m/%Y").to_string(),
            "ISO8601" | "ISO" => timestamp.to_rfc3339(),
            "RFC3339" => timestamp.to_rfc3339(),
            "UNIX" => timestamp.timestamp().to_string(),
            _ => {
                // Try to parse as strftime format
                match timestamp.format(format_str).to_string() {
                    formatted if formatted != format_str => formatted,
                    _ => return Err(AuroraError::InvalidArgument(format!("Unsupported format: {}", format_str))),
                }
            }
        };
        Ok(formatted)
    }

    fn smart_date_parse(&self, date_str: &str, format_str: &str) -> AuroraResult<DateTime<Utc>> {
        // UNIQUENESS: Auto-detect common formats
        match format_str {
            "auto" | "" => {
                // Try common formats
                if let Ok(dt) = DateTime::parse_from_rfc3339(date_str) {
                    Ok(dt.with_timezone(&Utc))
                } else if let Ok(dt) = chrono::NaiveDateTime::parse_from_str(date_str, "%Y-%m-%d %H:%M:%S") {
                    Ok(DateTime::from_naive_utc_and_offset(dt.naive_utc(), Utc))
                } else if let Ok(dt) = chrono::NaiveDate::parse_from_str(date_str, "%Y-%m-%d") {
                    Ok(DateTime::from_naive_utc_and_offset(dt.and_hms_opt(0, 0, 0).unwrap(), Utc))
                } else {
                    Err(AuroraError::InvalidArgument(format!("Could not auto-parse date: {}", date_str)))
                }
            },
            "YYYY-MM-DD" => {
                let naive = chrono::NaiveDate::parse_from_str(date_str, "%Y-%m-%d")
                    .map_err(|_| AuroraError::InvalidArgument(format!("Invalid date format: {}", date_str)))?;
                Ok(DateTime::from_naive_utc_and_offset(naive.and_hms_opt(0, 0, 0).unwrap(), Utc))
            },
            _ => {
                // Try parsing with the provided format
                let naive = chrono::NaiveDateTime::parse_from_str(date_str, format_str)
                    .map_err(|_| AuroraError::InvalidArgument(format!("Invalid date format: {}", date_str)))?;
                Ok(DateTime::from_naive_utc_and_offset(naive.naive_utc(), Utc))
            }
        }
    }

    fn calculate_business_days(&self, start: DateTime<Utc>, end: DateTime<Utc>) -> AuroraResult<i64> {
        let mut business_days = 0i64;
        let mut current = start;

        while current <= end {
            let weekday = current.weekday();
            if weekday != chrono::Weekday::Sat && weekday != chrono::Weekday::Sun {
                business_days += 1;
            }
            current = current + Duration::days(1);
        }

        Ok(business_days)
    }

    fn calculate_date_bucket(&self, timestamp: DateTime<Utc>, amount: i64, unit: &IntervalUnit) -> AuroraResult<DateTime<Utc>> {
        // Calculate which bucket this timestamp falls into
        let bucket_start = match unit {
            IntervalUnit::Days => {
                let days_since_epoch = timestamp.timestamp() / 86400;
                let bucket_days = days_since_epoch / amount;
                Utc.timestamp_opt(bucket_days * amount * 86400, 0).unwrap()
            },
            IntervalUnit::Weeks => {
                // Weeks since Unix epoch
                let weeks_since_epoch = timestamp.timestamp() / (86400 * 7);
                let bucket_weeks = weeks_since_epoch / amount;
                Utc.timestamp_opt(bucket_weeks * amount * 86400 * 7, 0).unwrap()
            },
            IntervalUnit::Months => {
                // Approximate months
                let months_since_epoch = (timestamp.year() as i64 - 1970) * 12 + timestamp.month() as i64 - 1;
                let bucket_months = months_since_epoch / amount;
                let year = 1970 + (bucket_months * amount) / 12;
                let month = ((bucket_months * amount) % 12) + 1;
                Utc.with_ymd_and_hms(year as i32, month as u32, 1, 0, 0, 0).unwrap()
            },
            _ => return Err(AuroraError::InvalidArgument("Unsupported bucket unit".to_string())),
        };

        Ok(bucket_start)
    }

    fn calculate_fiscal_year(&self, timestamp: DateTime<Utc>, fiscal_start_month: u32) -> AuroraResult<i64> {
        let mut year = timestamp.year() as i64;
        let month = timestamp.month();

        if month < fiscal_start_month {
            year -= 1;
        }

        Ok(year)
    }

    fn calculate_fiscal_quarter(&self, timestamp: DateTime<Utc>, fiscal_start_month: u32) -> AuroraResult<i64> {
        let month = timestamp.month();
        let fiscal_month = if month >= fiscal_start_month {
            month - fiscal_start_month + 1
        } else {
            month + (13 - fiscal_start_month)
        };

        let quarter = ((fiscal_month - 1) / 3) + 1;
        Ok(quarter as i64)
    }

    fn calculate_age(&self, start: DateTime<Utc>, end: DateTime<Utc>) -> AuroraResult<String> {
        let duration = end.signed_duration_since(start);

        let years = duration.num_days() / 365;
        let months = (duration.num_days() % 365) / 30;
        let days = (duration.num_days() % 365) % 30;

        Ok(format!("{} years {} months {} days", years, months, days))
    }
}

/// Function argument wrapper for date functions
#[derive(Debug, Clone)]
pub enum DateFunctionArg {
    String(String),
    Integer(i64),
    Float(f64),
    Timestamp(DateTime<Utc>),
}

impl DateFunctionArg {
    pub fn as_string(&self) -> AuroraResult<&str> {
        match self {
            DateFunctionArg::String(s) => Ok(s),
            _ => Err(AuroraError::InvalidArgument("Expected string argument".to_string())),
        }
    }

    pub fn as_integer(&self) -> AuroraResult<i64> {
        match self {
            DateFunctionArg::Integer(i) => Ok(*i),
            _ => Err(AuroraError::InvalidArgument("Expected integer argument".to_string())),
        }
    }

    pub fn as_timestamp(&self) -> AuroraResult<DateTime<Utc>> {
        match self {
            DateFunctionArg::Timestamp(ts) => Ok(*ts),
            _ => Err(AuroraError::InvalidArgument("Expected timestamp argument".to_string())),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use chrono::TimeZone;

    #[test]
    fn test_date_extract_year() {
        let mut executor = DateFunctionExecutor::new();
        let timestamp = Utc.with_ymd_and_hms(2024, 3, 15, 10, 30, 45).unwrap();

        let args = vec![
            DateFunctionArg::String("year".to_string()),
            DateFunctionArg::Timestamp(timestamp),
        ];

        let result = executor.execute_function("DATE_EXTRACT", &args).unwrap();
        match result {
            DateFunctionResult::Integer(year) => assert_eq!(year, 2024),
            _ => panic!("Expected integer result"),
        }
    }

    #[test]
    fn test_smart_extract_functions() {
        let mut executor = DateFunctionExecutor::new();
        let timestamp = Utc.with_ymd_and_hms(2024, 3, 15, 10, 30, 45).unwrap();

        let args = vec![DateFunctionArg::Timestamp(timestamp)];

        let year_result = executor.execute_function("YEAR", &args).unwrap();
        match year_result {
            DateFunctionResult::Integer(year) => assert_eq!(year, 2024),
            _ => panic!("Expected integer result"),
        }

        let month_result = executor.execute_function("MONTH", &args).unwrap();
        match month_result {
            DateFunctionResult::Integer(month) => assert_eq!(month, 3),
            _ => panic!("Expected integer result"),
        }
    }

    #[test]
    fn test_date_arithmetic() {
        let mut executor = DateFunctionExecutor::new();
        let timestamp = Utc.with_ymd_and_hms(2024, 3, 15, 10, 30, 45).unwrap();

        let args = vec![
            DateFunctionArg::Timestamp(timestamp),
            DateFunctionArg::String("1 day".to_string()),
        ];

        let result = executor.execute_function("DATE_ADD", &args).unwrap();
        match result {
            DateFunctionResult::Timestamp(new_ts) => {
                assert_eq!(new_ts.day(), 16);
                assert_eq!(new_ts.month(), 3);
                assert_eq!(new_ts.year(), 2024);
            },
            _ => panic!("Expected timestamp result"),
        }
    }

    #[test]
    fn test_date_diff() {
        let mut executor = DateFunctionExecutor::new();
        let date1 = Utc.with_ymd_and_hms(2024, 3, 15, 0, 0, 0).unwrap();
        let date2 = Utc.with_ymd_and_hms(2024, 3, 20, 0, 0, 0).unwrap();

        let args = vec![
            DateFunctionArg::String("day".to_string()),
            DateFunctionArg::Timestamp(date1),
            DateFunctionArg::Timestamp(date2),
        ];

        let result = executor.execute_function("DATE_DIFF", &args).unwrap();
        match result {
            DateFunctionResult::Integer(diff) => assert_eq!(diff, 5),
            _ => panic!("Expected integer result"),
        }
    }

    #[test]
    fn test_business_days() {
        let mut executor = DateFunctionExecutor::new();
        // Monday to Friday (5 business days including start)
        let start = Utc.with_ymd_and_hms(2024, 3, 11, 0, 0, 0).unwrap(); // Monday
        let end = Utc.with_ymd_and_hms(2024, 3, 15, 0, 0, 0).unwrap();   // Friday

        let args = vec![
            DateFunctionArg::Timestamp(start),
            DateFunctionArg::Timestamp(end),
        ];

        let result = executor.execute_function("BUSINESS_DAYS", &args).unwrap();
        match result {
            DateFunctionResult::Integer(days) => assert_eq!(days, 5),
            _ => panic!("Expected integer result"),
        }
    }

    #[test]
    fn test_fiscal_year() {
        let mut executor = DateFunctionExecutor::new();
        // July 2024 with fiscal year starting in July
        let timestamp = Utc.with_ymd_and_hms(2024, 7, 1, 0, 0, 0).unwrap();

        let args = vec![DateFunctionArg::Timestamp(timestamp)];

        let result = executor.execute_function("FISCAL_YEAR", &args).unwrap();
        match result {
            DateFunctionResult::Integer(year) => assert_eq!(year, 2024),
            _ => panic!("Expected integer result"),
        }
    }
}
