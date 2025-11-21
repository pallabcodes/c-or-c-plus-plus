/// Database data types supported by AuroraDB
#[derive(Debug, Clone, PartialEq)]
pub enum DataType {
    // Scalar types
    Boolean,
    Integer8,
    Integer16,
    Integer32,
    Integer64,
    Float32,
    Float64,
    Decimal(u8, u8), // precision, scale

    // String types
    Varchar(u32), // max length
    Text,         // unlimited length
    Binary(u32),  // max length

    // Date/Time types
    Date,
    Time,
    Timestamp,
    TimestampTz,

    // Complex types
    Json,
    Vector(u32), // dimension for vector embeddings
    Array(Box<DataType>),
}

/// Row data representation in memory
#[derive(Debug, Clone)]
pub struct Row {
    pub id: RowId,
    pub data: Vec<Option<Vec<u8>>>, // NULL values represented as None
}

/// Query execution statistics for performance monitoring
#[derive(Debug, Clone, Default)]
pub struct QueryStats {
    pub execution_time_ms: u64,
    pub rows_processed: u64,
    pub pages_accessed: u64,
    pub network_bytes: u64,
    pub cache_hits: u64,
    pub cache_misses: u64,
}

impl QueryStats {
    /// Calculate cache hit ratio
    pub fn cache_hit_ratio(&self) -> f64 {
        let total = self.cache_hits + self.cache_misses;
        if total == 0 { 0.0 } else { self.cache_hits as f64 / total as f64 }
    }

    /// Calculate average bytes per row processed
    pub fn avg_bytes_per_row(&self) -> f64 {
        if self.rows_processed == 0 { 0.0 } else { self.network_bytes as f64 / self.rows_processed as f64 }
    }
}
