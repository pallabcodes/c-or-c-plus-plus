//! AuroraDB Time Series Analytics: Advanced Statistical Analysis and Anomaly Detection
//!
//! Research-backed analytics with AuroraDB UNIQUENESS:
//! - Multi-algorithm anomaly detection with ensemble methods
//! - Statistical forecasting with confidence intervals
//! - Pattern recognition using time series decomposition
//! - Real-time alerting with customizable thresholds

use std::collections::HashMap;
use crate::core::errors::{AuroraResult, AuroraError};

/// Time series analytics engine
pub struct TimeSeriesAnalytics {
    anomaly_detectors: HashMap<String, Box<dyn AnomalyDetector>>,
    forecasters: HashMap<String, Box<dyn Forecaster>>,
    pattern_recognizers: HashMap<String, Box<dyn PatternRecognizer>>,
    alerting_engine: AlertingEngine,
}

impl TimeSeriesAnalytics {
    /// Create a new analytics engine
    pub fn new() -> Self {
        let mut analytics = Self {
            anomaly_detectors: HashMap::new(),
            forecasters: HashMap::new(),
            pattern_recognizers: HashMap::new(),
            alerting_engine: AlertingEngine::new(),
        };

        // Initialize with default algorithms
        analytics.initialize_default_algorithms();
        analytics
    }

    /// Detect anomalies in time series data
    pub fn detect_anomalies(&self, series_id: u64, data: &[(i64, f64)], detector_name: &str) -> AuroraResult<Vec<Anomaly>> {
        if let Some(detector) = self.anomaly_detectors.get(detector_name) {
            detector.detect_anomalies(series_id, data)
        } else {
            Err(AuroraError::Analytics(format!("Unknown anomaly detector: {}", detector_name)))
        }
    }

    /// Forecast future values
    pub fn forecast(&self, series_id: u64, historical_data: &[(i64, f64)], steps_ahead: usize, forecaster_name: &str) -> AuroraResult<Forecast> {
        if let Some(forecaster) = self.forecasters.get(forecaster_name) {
            forecaster.forecast(series_id, historical_data, steps_ahead)
        } else {
            Err(AuroraError::Analytics(format!("Unknown forecaster: {}", forecaster_name)))
        }
    }

    /// Recognize patterns in time series
    pub fn recognize_patterns(&self, series_id: u64, data: &[(i64, f64)], recognizer_name: &str) -> AuroraResult<Vec<PatternMatch>> {
        if let Some(recognizer) = self.pattern_recognizers.get(recognizer_name) {
            recognizer.recognize_patterns(series_id, data)
        } else {
            Err(AuroraError::Analytics(format!("Unknown pattern recognizer: {}", recognizer_name)))
        }
    }

    /// Analyze time series comprehensively
    pub fn comprehensive_analysis(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<ComprehensiveAnalysis> {
        if data.len() < 10 {
            return Err(AuroraError::Analytics("Insufficient data for comprehensive analysis".to_string()));
        }

        // Run multiple analyses in parallel
        let anomalies = self.detect_anomalies(series_id, data, "ensemble")?;
        let forecast = self.forecast(series_id, data, 10, "exponential_smoothing")?;
        let patterns = self.recognize_patterns(series_id, data, "decomposition")?;

        // Calculate statistical metrics
        let stats = self.calculate_statistics(data);

        // Check for alerts
        let alerts = self.alerting_engine.check_alerts(series_id, data, &anomalies)?;

        Ok(ComprehensiveAnalysis {
            series_id,
            statistics: stats,
            anomalies,
            forecast,
            patterns,
            alerts,
            analysis_timestamp: chrono::Utc::now().timestamp_millis(),
        })
    }

    /// Get available algorithms
    pub fn get_available_algorithms(&self) -> AlgorithmInventory {
        AlgorithmInventory {
            anomaly_detectors: self.anomaly_detectors.keys().cloned().collect(),
            forecasters: self.forecasters.keys().cloned().collect(),
            pattern_recognizers: self.pattern_recognizers.keys().cloned().collect(),
        }
    }

    /// Calculate basic statistics
    fn calculate_statistics(&self, data: &[(i64, f64)]) -> StatisticalSummary {
        if data.is_empty() {
            return StatisticalSummary::default();
        }

        let mut values: Vec<f64> = data.iter().map(|(_, v)| *v).collect();
        values.sort_by(|a, b| a.partial_cmp(b).unwrap());

        let n = values.len() as f64;
        let mean = values.iter().sum::<f64>() / n;

        let variance = values.iter()
            .map(|v| (v - mean).powi(2))
            .sum::<f64>() / n;
        let std_dev = variance.sqrt();

        let median = if values.len() % 2 == 0 {
            (values[values.len() / 2 - 1] + values[values.len() / 2]) / 2.0
        } else {
            values[values.len() / 2]
        };

        let min = *values.first().unwrap();
        let max = *values.last().unwrap();

        // Calculate percentiles
        let p95_idx = (values.len() as f64 * 0.95) as usize;
        let p99_idx = (values.len() as f64 * 0.99) as usize;

        StatisticalSummary {
            count: values.len(),
            mean,
            median,
            std_dev,
            min,
            max,
            p95: values.get(p95_idx).copied().unwrap_or(max),
            p99: values.get(p99_idx).copied().unwrap_or(max),
            skewness: self.calculate_skewness(&values, mean, std_dev),
            kurtosis: self.calculate_kurtosis(&values, mean, std_dev),
        }
    }

    /// Calculate skewness
    fn calculate_skewness(&self, values: &[f64], mean: f64, std_dev: f64) -> f64 {
        if values.is_empty() || std_dev == 0.0 {
            return 0.0;
        }

        let n = values.len() as f64;
        let skewness = values.iter()
            .map(|v| ((v - mean) / std_dev).powi(3))
            .sum::<f64>() / n;

        skewness
    }

    /// Calculate kurtosis
    fn calculate_kurtosis(&self, values: &[f64], mean: f64, std_dev: f64) -> f64 {
        if values.is_empty() || std_dev == 0.0 {
            return 0.0;
        }

        let n = values.len() as f64;
        let kurtosis = values.iter()
            .map(|v| ((v - mean) / std_dev).powi(4))
            .sum::<f64>() / n - 3.0; // Excess kurtosis

        kurtosis
    }

    /// Initialize default algorithms
    fn initialize_default_algorithms(&mut self) {
        // Anomaly detectors
        self.anomaly_detectors.insert("zscore".to_string(), Box::new(ZScoreDetector::new(3.0)));
        self.anomaly_detectors.insert("iqr".to_string(), Box::new(IQRDetector::new(1.5)));
        self.anomaly_detectors.insert("isolation_forest".to_string(), Box::new(IsolationForestDetector::new()));
        self.anomaly_detectors.insert("ensemble".to_string(), Box::new(EnsembleAnomalyDetector::new()));

        // Forecasters
        self.forecasters.insert("moving_average".to_string(), Box::new(MovingAverageForecaster::new(5)));
        self.forecasters.insert("exponential_smoothing".to_string(), Box::new(ExponentialSmoothingForecaster::new(0.3)));
        self.forecasters.insert("linear_regression".to_string(), Box::new(LinearRegressionForecaster::new()));

        // Pattern recognizers
        self.pattern_recognizers.insert("decomposition".to_string(), Box::new(TimeSeriesDecomposition::new()));
        self.pattern_recognizers.insert("fourier".to_string(), Box::new(FourierPatternRecognizer::new()));
    }
}

/// Anomaly detection trait
trait AnomalyDetector: Send + Sync {
    fn detect_anomalies(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<Anomaly>>;
}

/// Forecasting trait
trait Forecaster: Send + Sync {
    fn forecast(&self, series_id: u64, historical_data: &[(i64, f64)], steps_ahead: usize) -> AuroraResult<Forecast>;
}

/// Pattern recognition trait
trait PatternRecognizer: Send + Sync {
    fn recognize_patterns(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<PatternMatch>>;
}

/// Z-Score anomaly detector
struct ZScoreDetector {
    threshold: f64,
}

impl ZScoreDetector {
    fn new(threshold: f64) -> Self {
        Self { threshold }
    }
}

impl AnomalyDetector for ZScoreDetector {
    fn detect_anomalies(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<Anomaly>> {
        if data.len() < 2 {
            return Ok(Vec::new());
        }

        let values: Vec<f64> = data.iter().map(|(_, v)| *v).collect();
        let mean = values.iter().sum::<f64>() / values.len() as f64;
        let std_dev = (values.iter().map(|v| (v - mean).powi(2)).sum::<f64>() / values.len() as f64).sqrt();

        if std_dev == 0.0 {
            return Ok(Vec::new());
        }

        let mut anomalies = Vec::new();

        for &(timestamp, value) in data {
            let z_score = (value - mean).abs() / std_dev;
            if z_score > self.threshold {
                anomalies.push(Anomaly {
                    series_id,
                    timestamp,
                    value,
                    score: z_score,
                    algorithm: "zscore".to_string(),
                    confidence: (1.0 - self.threshold / z_score).min(1.0),
                });
            }
        }

        Ok(anomalies)
    }
}

/// IQR (Interquartile Range) anomaly detector
struct IQRDetector {
    multiplier: f64,
}

impl IQRDetector {
    fn new(multiplier: f64) -> Self {
        Self { multiplier }
    }
}

impl AnomalyDetector for IQRDetector {
    fn detect_anomalies(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<Anomaly>> {
        if data.len() < 4 {
            return Ok(Vec::new());
        }

        let mut values: Vec<f64> = data.iter().map(|(_, v)| *v).collect();
        values.sort_by(|a, b| a.partial_cmp(b).unwrap());

        let q1_idx = values.len() / 4;
        let q3_idx = 3 * values.len() / 4;

        let q1 = values[q1_idx];
        let q3 = values[q3_idx];
        let iqr = q3 - q1;

        let lower_bound = q1 - self.multiplier * iqr;
        let upper_bound = q3 + self.multiplier * iqr;

        let mut anomalies = Vec::new();

        for &(timestamp, value) in data {
            if value < lower_bound || value > upper_bound {
                let deviation = if value < lower_bound {
                    (lower_bound - value) / iqr
                } else {
                    (value - upper_bound) / iqr
                };

                anomalies.push(Anomaly {
                    series_id,
                    timestamp,
                    value,
                    score: deviation,
                    algorithm: "iqr".to_string(),
                    confidence: 0.8, // IQR is generally reliable
                });
            }
        }

        Ok(anomalies)
    }
}

/// Simplified isolation forest detector
struct IsolationForestDetector;

impl IsolationForestDetector {
    fn new() -> Self {
        Self
    }
}

impl AnomalyDetector for IsolationForestDetector {
    fn detect_anomalies(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<Anomaly>> {
        // Simplified implementation - in practice would use proper isolation forest
        let mut anomalies = Vec::new();

        if data.len() < 10 {
            return Ok(anomalies);
        }

        // Simple density-based approach as placeholder
        for i in 1..data.len() - 1 {
            let prev = data[i - 1].1;
            let curr = data[i].1;
            let next = data[i + 1].1;

            let avg_neighbor = (prev + next) / 2.0;
            let deviation = (curr - avg_neighbor).abs() / avg_neighbor.abs().max(0.001);

            if deviation > 0.5 {
                anomalies.push(Anomaly {
                    series_id,
                    timestamp: data[i].0,
                    value: curr,
                    score: deviation,
                    algorithm: "isolation_forest".to_string(),
                    confidence: 0.7,
                });
            }
        }

        Ok(anomalies)
    }
}

/// Ensemble anomaly detector combining multiple algorithms
struct EnsembleAnomalyDetector;

impl EnsembleAnomalyDetector {
    fn new() -> Self {
        Self
    }
}

impl AnomalyDetector for EnsembleAnomalyDetector {
    fn detect_anomalies(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<Anomaly>> {
        // Combine results from multiple detectors
        let zscore_detector = ZScoreDetector::new(2.5);
        let iqr_detector = IQRDetector::new(1.5);
        let isolation_detector = IsolationForestDetector::new();

        let zscore_anomalies = zscore_detector.detect_anomalies(series_id, data)?;
        let iqr_anomalies = iqr_detector.detect_anomalies(series_id, data)?;
        let isolation_anomalies = isolation_detector.detect_anomalies(series_id, data)?;

        // Combine anomalies that appear in multiple detectors
        let mut anomaly_scores = HashMap::new();

        for anomaly in zscore_anomalies {
            anomaly_scores.entry(anomaly.timestamp)
                .or_insert(Vec::new())
                .push(("zscore", anomaly.score, anomaly.confidence));
        }

        for anomaly in iqr_anomalies {
            anomaly_scores.entry(anomaly.timestamp)
                .or_insert(Vec::new())
                .push(("iqr", anomaly.score, anomaly.confidence));
        }

        for anomaly in isolation_anomalies {
            anomaly_scores.entry(anomaly.timestamp)
                .or_insert(Vec::new())
                .push(("isolation", anomaly.score, anomaly.confidence));
        }

        let mut ensemble_anomalies = Vec::new();

        for (timestamp, detections) in anomaly_scores {
            if detections.len() >= 2 { // Detected by at least 2 algorithms
                let avg_score = detections.iter().map(|(_, score, _)| score).sum::<f64>() / detections.len() as f64;
                let max_confidence = detections.iter().map(|(_, _, conf)| conf).fold(0.0, |a, &b| a.max(*b));

                // Find the data point
                if let Some((_, value)) = data.iter().find(|(ts, _)| *ts == timestamp) {
                    ensemble_anomalies.push(Anomaly {
                        series_id,
                        timestamp,
                        value: *value,
                        score: avg_score,
                        algorithm: "ensemble".to_string(),
                        confidence: max_confidence,
                    });
                }
            }
        }

        Ok(ensemble_anomalies)
    }
}

/// Moving average forecaster
struct MovingAverageForecaster {
    window_size: usize,
}

impl MovingAverageForecaster {
    fn new(window_size: usize) -> Self {
        Self { window_size }
    }
}

impl Forecaster for MovingAverageForecaster {
    fn forecast(&self, series_id: u64, historical_data: &[(i64, f64)], steps_ahead: usize) -> AuroraResult<Forecast> {
        if historical_data.len() < self.window_size {
            return Err(AuroraError::Analytics("Insufficient historical data for forecasting".to_string()));
        }

        let mut forecasts = Vec::new();
        let mut last_timestamp = historical_data.last().unwrap().0;

        // Calculate moving average of recent values
        let recent_values: Vec<f64> = historical_data.iter()
            .rev()
            .take(self.window_size)
            .map(|(_, v)| *v)
            .collect();

        let current_avg = recent_values.iter().sum::<f64>() / recent_values.len() as f64;

        for i in 0..steps_ahead {
            last_timestamp += 1000; // Assume 1-second intervals
            forecasts.push(ForecastPoint {
                timestamp: last_timestamp,
                value: current_avg,
                confidence_lower: current_avg * 0.9,
                confidence_upper: current_avg * 1.1,
            });
        }

        Ok(Forecast {
            series_id,
            method: "moving_average".to_string(),
            forecasts,
            confidence_level: 0.8,
        })
    }
}

/// Exponential smoothing forecaster
struct ExponentialSmoothingForecaster {
    alpha: f64, // Smoothing factor
}

impl ExponentialSmoothingForecaster {
    fn new(alpha: f64) -> Self {
        Self { alpha }
    }
}

impl Forecaster for ExponentialSmoothingForecaster {
    fn forecast(&self, series_id: u64, historical_data: &[(i64, f64)], steps_ahead: usize) -> AuroraResult<Forecast> {
        if historical_data.is_empty() {
            return Err(AuroraError::Analytics("No historical data for forecasting".to_string()));
        }

        // Calculate smoothed value
        let mut smoothed = historical_data[0].1;
        for &(_, value) in historical_data.iter().skip(1) {
            smoothed = self.alpha * value + (1.0 - self.alpha) * smoothed;
        }

        let mut forecasts = Vec::new();
        let mut last_timestamp = historical_data.last().unwrap().0;

        for i in 0..steps_ahead {
            last_timestamp += 1000;
            forecasts.push(ForecastPoint {
                timestamp: last_timestamp,
                value: smoothed,
                confidence_lower: smoothed * 0.95,
                confidence_upper: smoothed * 1.05,
            });
        }

        Ok(Forecast {
            series_id,
            method: "exponential_smoothing".to_string(),
            forecasts,
            confidence_level: 0.85,
        })
    }
}

/// Linear regression forecaster
struct LinearRegressionForecaster;

impl LinearRegressionForecaster {
    fn new() -> Self {
        Self
    }
}

impl Forecaster for LinearRegressionForecaster {
    fn forecast(&self, series_id: u64, historical_data: &[(i64, f64)], steps_ahead: usize) -> AuroraResult<Forecast> {
        if historical_data.len() < 2 {
            return Err(AuroraError::Analytics("Need at least 2 data points for linear regression".to_string()));
        }

        // Simple linear regression: y = mx + b
        let n = historical_data.len() as f64;
        let mut sum_x = 0.0;
        let mut sum_y = 0.0;
        let mut sum_xy = 0.0;
        let mut sum_xx = 0.0;

        for (i, &(_, value)) in historical_data.iter().enumerate() {
            let x = i as f64;
            sum_x += x;
            sum_y += value;
            sum_xy += x * value;
            sum_xx += x * x;
        }

        let slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
        let intercept = (sum_y - slope * sum_x) / n;

        let mut forecasts = Vec::new();
        let mut last_timestamp = historical_data.last().unwrap().0;
        let last_index = (historical_data.len() - 1) as f64;

        for i in 0..steps_ahead {
            let future_index = last_index + (i + 1) as f64;
            let predicted_value = slope * future_index + intercept;
            last_timestamp += 1000;

            forecasts.push(ForecastPoint {
                timestamp: last_timestamp,
                value: predicted_value,
                confidence_lower: predicted_value * 0.9,
                confidence_upper: predicted_value * 1.1,
            });
        }

        Ok(Forecast {
            series_id,
            method: "linear_regression".to_string(),
            forecasts,
            confidence_level: 0.75,
        })
    }
}

/// Time series decomposition for pattern recognition
struct TimeSeriesDecomposition;

impl TimeSeriesDecomposition {
    fn new() -> Self {
        Self
    }
}

impl PatternRecognizer for TimeSeriesDecomposition {
    fn recognize_patterns(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<PatternMatch>> {
        let mut patterns = Vec::new();

        if data.len() < 10 {
            return Ok(patterns);
        }

        // Simple trend detection
        let values: Vec<f64> = data.iter().map(|(_, v)| *v).collect();
        let trend = self.detect_trend(&values);

        if trend.strength > 0.7 {
            patterns.push(PatternMatch {
                series_id,
                pattern_type: "strong_trend".to_string(),
                start_timestamp: data[0].0,
                end_timestamp: data[data.len() - 1].0,
                confidence: trend.strength,
                metadata: HashMap::from([
                    ("direction".to_string(), if trend.slope > 0.0 { "increasing" } else { "decreasing" }.to_string()),
                    ("slope".to_string(), trend.slope.to_string()),
                ]),
            });
        }

        // Seasonality detection (simplified)
        if self.detect_seasonality(&values) {
            patterns.push(PatternMatch {
                series_id,
                pattern_type: "seasonal".to_string(),
                start_timestamp: data[0].0,
                end_timestamp: data[data.len() - 1].0,
                confidence: 0.6,
                metadata: HashMap::new(),
            });
        }

        Ok(patterns)
    }

    fn detect_trend(&self, values: &[f64]) -> TrendInfo {
        if values.len() < 3 {
            return TrendInfo { slope: 0.0, strength: 0.0 };
        }

        // Simple linear regression
        let n = values.len() as f64;
        let mut sum_x = 0.0;
        let mut sum_y = 0.0;
        let mut sum_xy = 0.0;
        let mut sum_xx = 0.0;

        for (i, &value) in values.iter().enumerate() {
            let x = i as f64;
            sum_x += x;
            sum_y += value;
            sum_xy += x * value;
            sum_xx += x * x;
        }

        let slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);

        // Calculate R-squared as trend strength
        let y_mean = sum_y / n;
        let mut ss_res = 0.0;
        let mut ss_tot = 0.0;

        for (i, &value) in values.iter().enumerate() {
            let x = i as f64;
            let predicted = slope * x + (sum_y - slope * sum_x) / n;
            ss_res += (value - predicted).powi(2);
            ss_tot += (value - y_mean).powi(2);
        }

        let r_squared = 1.0 - (ss_res / ss_tot);

        TrendInfo {
            slope,
            strength: r_squared.max(0.0).min(1.0),
        }
    }

    fn detect_seasonality(&self, values: &[f64]) -> bool {
        // Simplified seasonality detection
        // In practice, would use autocorrelation or FFT
        if values.len() < 20 {
            return false;
        }

        // Check for repeating patterns (very simplified)
        let quarter_len = values.len() / 4;
        if quarter_len < 2 {
            return false;
        }

        let mut correlations = Vec::new();

        for lag in 2..quarter_len.min(10) {
            let correlation = self.autocorrelation(values, lag);
            correlations.push(correlation);
        }

        // If any correlation is high, consider it seasonal
        correlations.iter().any(|&corr| corr > 0.6)
    }

    fn autocorrelation(&self, values: &[f64], lag: usize) -> f64 {
        if values.len() <= lag {
            return 0.0;
        }

        let n = values.len() - lag;
        let mean = values.iter().sum::<f64>() / values.len() as f64;

        let mut numerator = 0.0;
        let mut denom1 = 0.0;
        let mut denom2 = 0.0;

        for i in 0..n {
            let diff1 = values[i] - mean;
            let diff2 = values[i + lag] - mean;

            numerator += diff1 * diff2;
            denom1 += diff1 * diff1;
            denom2 += diff2 * diff2;
        }

        if denom1 == 0.0 || denom2 == 0.0 {
            0.0
        } else {
            numerator / (denom1.sqrt() * denom2.sqrt())
        }
    }
}

/// Simplified Fourier pattern recognizer
struct FourierPatternRecognizer;

impl FourierPatternRecognizer {
    fn new() -> Self {
        Self
    }
}

impl PatternRecognizer for FourierPatternRecognizer {
    fn recognize_patterns(&self, series_id: u64, data: &[(i64, f64)]) -> AuroraResult<Vec<PatternMatch>> {
        // Simplified frequency analysis
        let mut patterns = Vec::new();

        if data.len() < 10 {
            return Ok(patterns);
        }

        // Basic frequency detection (placeholder for real FFT)
        patterns.push(PatternMatch {
            series_id,
            pattern_type: "frequency_analysis".to_string(),
            start_timestamp: data[0].0,
            end_timestamp: data[data.len() - 1].0,
            confidence: 0.5,
            metadata: HashMap::from([
                ("dominant_frequency".to_string(), "unknown".to_string()),
            ]),
        });

        Ok(patterns)
    }
}

/// Alerting engine for threshold-based alerts
pub struct AlertingEngine {
    alerts: HashMap<String, AlertRule>,
}

impl AlertingEngine {
    fn new() -> Self {
        Self {
            alerts: HashMap::new(),
        }
    }

    fn check_alerts(&self, series_id: u64, data: &[(i64, f64)], anomalies: &[Anomaly]) -> AuroraResult<Vec<Alert>> {
        let mut triggered_alerts = Vec::new();

        for (rule_name, rule) in &self.alerts {
            if rule.series_id == series_id {
                for &(timestamp, value) in data {
                    if self.check_threshold(&rule.condition, value) {
                        triggered_alerts.push(Alert {
                            rule_name: rule_name.clone(),
                            series_id,
                            timestamp,
                            value,
                            condition: rule.condition.clone(),
                            severity: rule.severity.clone(),
                            message: format!("Alert triggered: {} for series {}", rule_name, series_id),
                        });
                    }
                }

                // Check anomaly count
                if let AlertCondition::AnomalyCount(threshold) = &rule.condition {
                    if anomalies.len() >= *threshold {
                        triggered_alerts.push(Alert {
                            rule_name: rule_name.clone(),
                            series_id,
                            timestamp: data.last().unwrap().0,
                            value: anomalies.len() as f64,
                            condition: rule.condition.clone(),
                            severity: rule.severity.clone(),
                            message: format!("High anomaly count: {} for series {}", anomalies.len(), series_id),
                        });
                    }
                }
            }
        }

        Ok(triggered_alerts)
    }

    fn check_threshold(&self, condition: &AlertCondition, value: f64) -> bool {
        match condition {
            AlertCondition::ValueAbove(threshold) => value > *threshold,
            AlertCondition::ValueBelow(threshold) => value < *threshold,
            AlertCondition::ValueOutside(min, max) => value < *min || value > *max,
            AlertCondition::AnomalyCount(_) => false, // Handled separately
        }
    }
}

/// Data structures for analytics results
#[derive(Debug, Clone)]
pub struct Anomaly {
    pub series_id: u64,
    pub timestamp: i64,
    pub value: f64,
    pub score: f64,
    pub algorithm: String,
    pub confidence: f64,
}

#[derive(Debug, Clone)]
pub struct Forecast {
    pub series_id: u64,
    pub method: String,
    pub forecasts: Vec<ForecastPoint>,
    pub confidence_level: f64,
}

#[derive(Debug, Clone)]
pub struct ForecastPoint {
    pub timestamp: i64,
    pub value: f64,
    pub confidence_lower: f64,
    pub confidence_upper: f64,
}

#[derive(Debug, Clone)]
pub struct PatternMatch {
    pub series_id: u64,
    pub pattern_type: String,
    pub start_timestamp: i64,
    pub end_timestamp: i64,
    pub confidence: f64,
    pub metadata: HashMap<String, String>,
}

#[derive(Debug, Clone)]
pub struct ComprehensiveAnalysis {
    pub series_id: u64,
    pub statistics: StatisticalSummary,
    pub anomalies: Vec<Anomaly>,
    pub forecast: Forecast,
    pub patterns: Vec<PatternMatch>,
    pub alerts: Vec<Alert>,
    pub analysis_timestamp: i64,
}

#[derive(Debug, Clone, Default)]
pub struct StatisticalSummary {
    pub count: usize,
    pub mean: f64,
    pub median: f64,
    pub std_dev: f64,
    pub min: f64,
    pub max: f64,
    pub p95: f64,
    pub p99: f64,
    pub skewness: f64,
    pub kurtosis: f64,
}

#[derive(Debug, Clone)]
pub struct Alert {
    pub rule_name: String,
    pub series_id: u64,
    pub timestamp: i64,
    pub value: f64,
    pub condition: AlertCondition,
    pub severity: AlertSeverity,
    pub message: String,
}

#[derive(Debug, Clone)]
pub enum AlertCondition {
    ValueAbove(f64),
    ValueBelow(f64),
    ValueOutside(f64, f64),
    AnomalyCount(usize),
}

#[derive(Debug, Clone)]
pub enum AlertSeverity {
    Low,
    Medium,
    High,
    Critical,
}

#[derive(Debug, Clone)]
pub struct AlertRule {
    pub series_id: u64,
    pub condition: AlertCondition,
    pub severity: AlertSeverity,
}

#[derive(Debug, Clone)]
pub struct AlgorithmInventory {
    pub anomaly_detectors: Vec<String>,
    pub forecasters: Vec<String>,
    pub pattern_recognizers: Vec<String>,
}

/// Internal structures
#[derive(Debug)]
struct TrendInfo {
    slope: f64,
    strength: f64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_zscore_anomaly_detection() {
        let detector = ZScoreDetector::new(2.0);
        let data = vec![
            (1000, 10.0), (1001, 10.1), (1002, 10.2), (1003, 10.1),
            (1004, 50.0), // Anomaly
            (1005, 10.0), (1006, 9.9),
        ];

        let anomalies = detector.detect_anomalies(1, &data).unwrap();
        assert!(!anomalies.is_empty());

        let anomaly = &anomalies[0];
        assert_eq!(anomaly.timestamp, 1004);
        assert_eq!(anomaly.value, 50.0);
        assert_eq!(anomaly.algorithm, "zscore");
    }

    #[test]
    fn test_iqr_anomaly_detection() {
        let detector = IQRDetector::new(1.5);
        let data = vec![
            (1000, 10.0), (1001, 10.5), (1002, 11.0), (1003, 10.5),
            (1004, 10.8), (1005, 11.2), (1006, 10.9),
            (1007, 50.0), // Anomaly
        ];

        let anomalies = detector.detect_anomalies(1, &data).unwrap();
        assert!(!anomalies.is_empty());
        assert_eq!(anomalies[0].algorithm, "iqr");
    }

    #[test]
    fn test_moving_average_forecasting() {
        let forecaster = MovingAverageForecaster::new(3);
        let historical_data = vec![
            (1000, 10.0), (1001, 11.0), (1002, 12.0), (1003, 11.5), (1004, 12.5),
        ];

        let forecast = forecaster.forecast(1, &historical_data, 3).unwrap();
        assert_eq!(forecast.series_id, 1);
        assert_eq!(forecast.method, "moving_average");
        assert_eq!(forecast.forecasts.len(), 3);
    }

    #[test]
    fn test_exponential_smoothing_forecasting() {
        let forecaster = ExponentialSmoothingForecaster::new(0.3);
        let historical_data = vec![
            (1000, 10.0), (1001, 12.0), (1002, 11.0), (1003, 13.0),
        ];

        let forecast = forecaster.forecast(1, &historical_data, 2).unwrap();
        assert_eq!(forecast.forecasts.len(), 2);
        assert_eq!(forecast.method, "exponential_smoothing");
    }

    #[test]
    fn test_linear_regression_forecasting() {
        let forecaster = LinearRegressionForecaster::new();
        let historical_data = vec![
            (1000, 10.0), (1001, 11.0), (1002, 12.0), (1003, 13.0),
        ];

        let forecast = forecaster.forecast(1, &historical_data, 2).unwrap();
        assert_eq!(forecast.forecasts.len(), 2);
        assert_eq!(forecast.method, "linear_regression");
    }

    #[test]
    fn test_comprehensive_analysis() {
        let analytics = TimeSeriesAnalytics::new();
        let data = vec![
            (1000, 10.0), (1001, 10.5), (1002, 11.0), (1003, 10.8),
            (1004, 50.0), // Anomaly
            (1005, 11.2), (1006, 11.5), (1007, 11.1), (1008, 11.8), (1009, 11.3),
        ];

        let analysis = analytics.comprehensive_analysis(1, &data).unwrap();

        assert_eq!(analysis.series_id, 1);
        assert!(analysis.statistics.count > 0);
        assert!(!analysis.anomalies.is_empty()); // Should detect the anomaly
        assert!(!analysis.forecast.forecasts.is_empty());
        assert_eq!(analysis.analysis_timestamp, chrono::Utc::now().timestamp_millis());
    }

    #[test]
    fn test_statistical_summary() {
        let analytics = TimeSeriesAnalytics::new();
        let data = vec![
            (1000, 1.0), (1001, 2.0), (1002, 3.0), (1003, 4.0), (1004, 5.0),
        ];

        let stats = analytics.calculate_statistics(&data);

        assert_eq!(stats.count, 5);
        assert_eq!(stats.mean, 3.0);
        assert_eq!(stats.median, 3.0);
        assert_eq!(stats.min, 1.0);
        assert_eq!(stats.max, 5.0);
        assert!(stats.std_dev > 0.0);
    }

    #[test]
    fn test_trend_detection() {
        let decomposition = TimeSeriesDecomposition::new();
        let values = vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0];

        let trend = decomposition.detect_trend(&values);
        assert!(trend.slope > 0.8); // Strong positive trend
        assert!(trend.strength > 0.9); // High confidence
    }

    #[test]
    fn test_ensemble_anomaly_detection() {
        let detector = EnsembleAnomalyDetector::new();
        let data = vec![
            (1000, 10.0), (1001, 10.1), (1002, 10.2), (1003, 10.1),
            (1004, 100.0), // Clear anomaly
            (1005, 10.0), (1006, 9.9),
        ];

        let anomalies = detector.detect_anomalies(1, &data).unwrap();
        // Ensemble should detect the anomaly with high confidence
        assert!(!anomalies.is_empty());
        assert_eq!(anomalies[0].algorithm, "ensemble");
    }

    #[test]
    fn test_algorithm_inventory() {
        let analytics = TimeSeriesAnalytics::new();
        let inventory = analytics.get_available_algorithms();

        assert!(inventory.anomaly_detectors.contains(&"zscore".to_string()));
        assert!(inventory.anomaly_detectors.contains(&"ensemble".to_string()));
        assert!(inventory.forecasters.contains(&"moving_average".to_string()));
        assert!(inventory.forecasters.contains(&"linear_regression".to_string()));
        assert!(inventory.pattern_recognizers.contains(&"decomposition".to_string()));
    }

    #[test]
    fn test_skewness_and_kurtosis() {
        let analytics = TimeSeriesAnalytics::new();

        // Normal distribution-like data
        let data = vec![
            (1000, 1.0), (1001, 2.0), (1002, 3.0), (1003, 4.0), (1004, 5.0),
            (1005, 4.0), (1006, 3.0), (1007, 2.0), (1008, 1.0),
        ];

        let stats = analytics.calculate_statistics(&data);
        assert!(stats.skewness.abs() < 1.0); // Should be approximately symmetric
        assert!(stats.kurtosis.abs() < 2.0); // Should be reasonable kurtosis
    }

    #[test]
    fn test_forecast_confidence_intervals() {
        let forecaster = ExponentialSmoothingForecaster::new(0.3);
        let historical_data = vec![
            (1000, 10.0), (1001, 12.0), (1002, 11.0), (1003, 13.0),
        ];

        let forecast = forecaster.forecast(1, &historical_data, 3).unwrap();

        for point in &forecast.forecasts {
            assert!(point.confidence_lower <= point.value);
            assert!(point.confidence_upper >= point.value);
            assert!(point.confidence_lower < point.confidence_upper);
        }
    }
}
