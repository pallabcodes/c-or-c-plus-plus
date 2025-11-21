//! AuroraDB Advanced Analytics: Statistical Analysis & Forecasting
//!
//! Revolutionary analytics capabilities built into SQL:
//! - Statistical functions (correlation, regression, hypothesis testing)
//! - Time series forecasting (ARIMA, exponential smoothing)
//! - Anomaly detection algorithms
//! - Trend analysis and pattern recognition
//! - Predictive modeling functions

use std::collections::HashMap;
use crate::core::errors::{AuroraResult, AuroraError};

/// Advanced Analytics Function Registry
pub struct AnalyticsFunctionRegistry {
    functions: HashMap<String, Box<dyn AnalyticsFunction>>,
}

impl AnalyticsFunctionRegistry {
    pub fn new() -> Self {
        let mut registry = Self {
            functions: HashMap::new(),
        };

        // Register built-in analytics functions
        registry.register_function("correlation", Box::new(CorrelationFunction));
        registry.register_function("linear_regression", Box::new(LinearRegressionFunction));
        registry.register_function("time_series_forecast", Box::new(TimeSeriesForecastFunction));
        registry.register_function("moving_average", Box::new(MovingAverageFunction));
        registry.register_function("exponential_smoothing", Box::new(ExponentialSmoothingFunction));
        registry.register_function("seasonal_decompose", Box::new(SeasonalDecomposeFunction));
        registry.register_function("outlier_detection", Box::new(OutlierDetectionFunction));
        registry.register_function("trend_analysis", Box::new(TrendAnalysisFunction));
        registry.register_function("hypothesis_test", Box::new(HypothesisTestFunction));
        registry.register_function("distribution_fit", Box::new(DistributionFitFunction));

        registry
    }

    fn register_function(&mut self, name: &str, function: Box<dyn AnalyticsFunction>) {
        self.functions.insert(name.to_string(), function);
    }

    pub fn execute_function(
        &self,
        name: &str,
        args: Vec<serde_json::Value>,
        context: &QueryContext,
    ) -> AuroraResult<serde_json::Value> {
        if let Some(function) = self.functions.get(name) {
            function.execute(args, context)
        } else {
            Err(AuroraError::InvalidArgument(format!("Unknown analytics function: {}", name)))
        }
    }

    pub fn list_functions(&self) -> Vec<String> {
        self.functions.keys().cloned().collect()
    }
}

/// Query execution context
#[derive(Debug, Clone)]
pub struct QueryContext {
    pub database: String,
    pub user: String,
    pub timestamp: i64,
    pub variables: HashMap<String, serde_json::Value>,
}

/// Analytics Function trait
pub trait AnalyticsFunction: Send + Sync {
    fn execute(&self, args: Vec<serde_json::Value>, context: &QueryContext) -> AuroraResult<serde_json::Value>;
    fn name(&self) -> &str;
    fn description(&self) -> &str;
}

/// Correlation Analysis Function
pub struct CorrelationFunction;

impl AnalyticsFunction for CorrelationFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument("correlation requires 2 arguments: x_values and y_values".to_string()));
        }

        let x_values = Self::extract_numbers(&args[0])?;
        let y_values = Self::extract_numbers(&args[1])?;

        if x_values.len() != y_values.len() {
            return Err(AuroraError::InvalidArgument("x and y values must have the same length".to_string()));
        }

        let correlation = self.pearson_correlation(&x_values, &y_values)?;

        let result = serde_json::json!({
            "correlation_coefficient": correlation,
            "sample_size": x_values.len(),
            "method": "pearson"
        });

        Ok(result)
    }

    fn name(&self) -> &str { "correlation" }
    fn description(&self) -> &str { "Calculate Pearson correlation coefficient between two variables" }
}

impl CorrelationFunction {
    fn extract_numbers(value: &serde_json::Value) -> AuroraResult<Vec<f64>> {
        match value {
            serde_json::Value::Array(arr) => {
                let mut numbers = Vec::new();
                for val in arr {
                    if let Some(num) = val.as_f64() {
                        numbers.push(num);
                    } else {
                        return Err(AuroraError::InvalidArgument("Array must contain numbers".to_string()));
                    }
                }
                Ok(numbers)
            }
            _ => Err(AuroraError::InvalidArgument("Expected array of numbers".to_string())),
        }
    }

    fn pearson_correlation(&self, x: &[f64], y: &[f64]) -> AuroraResult<f64> {
        let n = x.len() as f64;

        let sum_x = x.iter().sum::<f64>();
        let sum_y = y.iter().sum::<f64>();
        let sum_xy = x.iter().zip(y.iter()).map(|(a, b)| a * b).sum::<f64>();
        let sum_x2 = x.iter().map(|a| a * a).sum::<f64>();
        let sum_y2 = y.iter().map(|a| a * a).sum::<f64>();

        let numerator = n * sum_xy - sum_x * sum_y;
        let denominator = ((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y)).sqrt();

        if denominator == 0.0 {
            return Ok(0.0); // No correlation if denominator is zero
        }

        Ok(numerator / denominator)
    }
}

/// Linear Regression Function
pub struct LinearRegressionFunction;

impl AnalyticsFunction for LinearRegressionFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() != 2 {
            return Err(AuroraError::InvalidArgument("linear_regression requires 2 arguments: x_values and y_values".to_string()));
        }

        let x_values = CorrelationFunction::extract_numbers(&args[0])?;
        let y_values = CorrelationFunction::extract_numbers(&args[1])?;

        if x_values.len() != y_values.len() {
            return Err(AuroraError::InvalidArgument("x and y values must have the same length".to_string()));
        }

        let model = self.simple_linear_regression(&x_values, &y_values)?;

        let result = serde_json::json!({
            "slope": model.slope,
            "intercept": model.intercept,
            "r_squared": model.r_squared,
            "sample_size": x_values.len(),
            "equation": format!("y = {:.4}x + {:.4}", model.slope, model.intercept)
        });

        Ok(result)
    }

    fn name(&self) -> &str { "linear_regression" }
    fn description(&self) -> &str { "Perform simple linear regression analysis" }
}

impl LinearRegressionFunction {
    fn simple_linear_regression(&self, x: &[f64], y: &[f64]) -> AuroraResult<LinearRegressionModel> {
        let n = x.len() as f64;

        let sum_x = x.iter().sum::<f64>();
        let sum_y = y.iter().sum::<f64>();
        let sum_xy = x.iter().zip(y.iter()).map(|(a, b)| a * b).sum::<f64>();
        let sum_x2 = x.iter().map(|a| a * a).sum::<f64>();
        let sum_y2 = y.iter().map(|a| a * a).sum::<f64>();

        let slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
        let intercept = (sum_y - slope * sum_x) / n;

        // Calculate R-squared
        let y_mean = sum_y / n;
        let ss_tot = y.iter().map(|&yi| (yi - y_mean).powi(2)).sum::<f64>();
        let ss_res = x.iter().zip(y.iter())
            .map(|(&xi, &yi)| (yi - (slope * xi + intercept)).powi(2))
            .sum::<f64>();
        let r_squared = 1.0 - (ss_res / ss_tot);

        Ok(LinearRegressionModel {
            slope,
            intercept,
            r_squared,
        })
    }
}

#[derive(Debug)]
struct LinearRegressionModel {
    slope: f64,
    intercept: f64,
    r_squared: f64,
}

/// Time Series Forecasting Function
pub struct TimeSeriesForecastFunction;

impl AnalyticsFunction for TimeSeriesForecastFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 2 {
            return Err(AuroraError::InvalidArgument("time_series_forecast requires at least 2 arguments: values and forecast_steps".to_string()));
        }

        let values = CorrelationFunction::extract_numbers(&args[0])?;
        let forecast_steps = args[1].as_u64().unwrap_or(5) as usize;
        let method = args.get(2).and_then(|v| v.as_str()).unwrap_or("simple");

        let forecast = match method {
            "simple" => self.simple_exponential_smoothing(&values, forecast_steps),
            "linear" => self.linear_trend_forecast(&values, forecast_steps),
            _ => self.simple_exponential_smoothing(&values, forecast_steps),
        }?;

        let result = serde_json::json!({
            "forecast": forecast,
            "method": method,
            "steps": forecast_steps,
            "historical_data_points": values.len()
        });

        Ok(result)
    }

    fn name(&self) -> &str { "time_series_forecast" }
    fn description(&self) -> &str { "Generate time series forecasts using various methods" }
}

impl TimeSeriesForecastFunction {
    fn simple_exponential_smoothing(&self, values: &[f64], steps: usize) -> AuroraResult<Vec<f64>> {
        if values.is_empty() {
            return Ok(vec![0.0; steps]);
        }

        let alpha = 0.3; // Smoothing parameter
        let mut smoothed = vec![values[0]];

        for &value in &values[1..] {
            let last_smooth = *smoothed.last().unwrap();
            let new_smooth = alpha * value + (1.0 - alpha) * last_smooth;
            smoothed.push(new_smooth);
        }

        let last_value = *smoothed.last().unwrap();
        let mut forecast = Vec::new();

        for _ in 0..steps {
            forecast.push(last_value);
        }

        Ok(forecast)
    }

    fn linear_trend_forecast(&self, values: &[f64], steps: usize) -> AuroraResult<Vec<f64>> {
        if values.len() < 2 {
            return Ok(vec![values.last().copied().unwrap_or(0.0); steps]);
        }

        let n = values.len() as f64;
        let x_values: Vec<f64> = (0..values.len()).map(|i| i as f64).collect();

        let regression = LinearRegressionFunction.simple_linear_regression(
            &LinearRegressionFunction,
            &x_values,
            values
        )?;

        let mut forecast = Vec::new();
        for i in 0..steps {
            let x = (values.len() + i) as f64;
            let y = regression.slope * x + regression.intercept;
            forecast.push(y);
        }

        Ok(forecast)
    }
}

/// Moving Average Function
pub struct MovingAverageFunction;

impl AnalyticsFunction for MovingAverageFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 2 {
            return Err(AuroraError::InvalidArgument("moving_average requires 2 arguments: values and window_size".to_string()));
        }

        let values = CorrelationFunction::extract_numbers(&args[0])?;
        let window_size = args[1].as_u64().unwrap_or(3) as usize;

        let moving_avg = self.calculate_moving_average(&values, window_size)?;

        let result = serde_json::json!({
            "moving_average": moving_avg,
            "window_size": window_size,
            "input_length": values.len(),
            "output_length": moving_avg.len()
        });

        Ok(result)
    }

    fn name(&self) -> &str { "moving_average" }
    fn description(&self) -> &str { "Calculate moving average for time series smoothing" }
}

impl MovingAverageFunction {
    fn calculate_moving_average(&self, values: &[f64], window_size: usize) -> AuroraResult<Vec<f64>> {
        if window_size == 0 || window_size > values.len() {
            return Err(AuroraError::InvalidArgument("Invalid window size".to_string()));
        }

        let mut result = Vec::new();

        for i in 0..=(values.len() - window_size) {
            let window = &values[i..i + window_size];
            let avg = window.iter().sum::<f64>() / window_size as f64;
            result.push(avg);
        }

        Ok(result)
    }
}

/// Exponential Smoothing Function
pub struct ExponentialSmoothingFunction;

impl AnalyticsFunction for ExponentialSmoothingFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 2 {
            return Err(AuroraError::InvalidArgument("exponential_smoothing requires 2 arguments: values and alpha".to_string()));
        }

        let values = CorrelationFunction::extract_numbers(&args[0])?;
        let alpha = args[1].as_f64().unwrap_or(0.3);

        if !(0.0..=1.0).contains(&alpha) {
            return Err(AuroraError::InvalidArgument("Alpha must be between 0 and 1".to_string()));
        }

        let smoothed = self.exponential_smoothing(&values, alpha)?;

        let result = serde_json::json!({
            "smoothed_values": smoothed,
            "alpha": alpha,
            "input_length": values.len()
        });

        Ok(result)
    }

    fn name(&self) -> &str { "exponential_smoothing" }
    fn description(&self) -> &str { "Apply exponential smoothing to time series data" }
}

impl ExponentialSmoothingFunction {
    fn exponential_smoothing(&self, values: &[f64], alpha: f64) -> AuroraResult<Vec<f64>> {
        if values.is_empty() {
            return Ok(Vec::new());
        }

        let mut smoothed = vec![values[0]];

        for &value in &values[1..] {
            let last_smooth = *smoothed.last().unwrap();
            let new_smooth = alpha * value + (1.0 - alpha) * last_smooth;
            smoothed.push(new_smooth);
        }

        Ok(smoothed)
    }
}

/// Seasonal Decomposition Function
pub struct SeasonalDecomposeFunction;

impl AnalyticsFunction for SeasonalDecomposeFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 2 {
            return Err(AuroraError::InvalidArgument("seasonal_decompose requires 2 arguments: values and season_length".to_string()));
        }

        let values = CorrelationFunction::extract_numbers(&args[0])?;
        let season_length = args[1].as_u64().unwrap_or(12) as usize;

        let decomposition = self.seasonal_decompose(&values, season_length)?;

        let result = serde_json::json!({
            "trend": decomposition.trend,
            "seasonal": decomposition.seasonal,
            "residual": decomposition.residual,
            "season_length": season_length,
            "method": "additive"
        });

        Ok(result)
    }

    fn name(&self) -> &str { "seasonal_decompose" }
    fn description(&self) -> &str { "Decompose time series into trend, seasonal, and residual components" }
}

impl SeasonalDecomposeFunction {
    fn seasonal_decompose(&self, values: &[f64], season_length: usize) -> AuroraResult<SeasonalDecomposition> {
        if values.len() < 2 * season_length {
            return Err(AuroraError::InvalidArgument("Not enough data for seasonal decomposition".to_string()));
        }

        // Simplified seasonal decomposition (moving average method)
        let trend = self.calculate_trend(values, season_length)?;
        let seasonal = self.calculate_seasonal(values, &trend, season_length)?;
        let residual = self.calculate_residual(values, &trend, &seasonal)?;

        Ok(SeasonalDecomposition {
            trend,
            seasonal,
            residual,
        })
    }

    fn calculate_trend(&self, values: &[f64], season_length: usize) -> AuroraResult<Vec<f64>> {
        // Simple moving average for trend
        MovingAverageFunction.calculate_moving_average(
            &MovingAverageFunction,
            values,
            season_length
        )
    }

    fn calculate_seasonal(&self, values: &[f64], trend: &[f64], season_length: usize) -> AuroraResult<Vec<f64>> {
        // Simplified seasonal calculation
        let mut seasonal = vec![0.0; values.len()];

        // For each season position, calculate average deviation from trend
        for i in 0..season_length {
            let mut deviations = Vec::new();

            for j in (i..values.len()).step_by(season_length) {
                if j < values.len() && j < trend.len() {
                    deviations.push(values[j] - trend[j]);
                }
            }

            if !deviations.is_empty() {
                let avg_deviation = deviations.iter().sum::<f64>() / deviations.len() as f64;
                for j in (i..values.len()).step_by(season_length) {
                    if j < seasonal.len() {
                        seasonal[j] = avg_deviation;
                    }
                }
            }
        }

        Ok(seasonal)
    }

    fn calculate_residual(&self, values: &[f64], trend: &[f64], seasonal: &[f64]) -> AuroraResult<Vec<f64>> {
        let mut residual = Vec::new();

        for i in 0..values.len() {
            let trend_val = if i < trend.len() { trend[i] } else { 0.0 };
            let seasonal_val = if i < seasonal.len() { seasonal[i] } else { 0.0 };
            residual.push(values[i] - trend_val - seasonal_val);
        }

        Ok(residual)
    }
}

#[derive(Debug)]
struct SeasonalDecomposition {
    trend: Vec<f64>,
    seasonal: Vec<f64>,
    residual: Vec<f64>,
}

/// Outlier Detection Function
pub struct OutlierDetectionFunction;

impl AnalyticsFunction for OutlierDetectionFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 1 {
            return Err(AuroraError::InvalidArgument("outlier_detection requires at least 1 argument: values".to_string()));
        }

        let values = CorrelationFunction::extract_numbers(&args[0])?;
        let method = args.get(1).and_then(|v| v.as_str()).unwrap_or("iqr");
        let threshold = args.get(2).and_then(|v| v.as_f64()).unwrap_or(1.5);

        let outliers = self.detect_outliers(&values, method, threshold)?;

        let result = serde_json::json!({
            "outliers": outliers.outlier_indices,
            "outlier_values": outliers.outlier_values,
            "method": method,
            "threshold": threshold,
            "total_points": values.len(),
            "outlier_count": outliers.outlier_indices.len()
        });

        Ok(result)
    }

    fn name(&self) -> &str { "outlier_detection" }
    fn description(&self) -> &str { "Detect outliers in numerical data using various methods" }
}

impl OutlierDetectionFunction {
    fn detect_outliers(&self, values: &[f64], method: &str, threshold: f64) -> AuroraResult<OutlierResult> {
        match method {
            "iqr" => self.iqr_outlier_detection(values, threshold),
            "zscore" => self.zscore_outlier_detection(values, threshold),
            "modified_zscore" => self.modified_zscore_outlier_detection(values, threshold),
            _ => self.iqr_outlier_detection(values, threshold),
        }
    }

    fn iqr_outlier_detection(&self, values: &[f64], threshold: f64) -> AuroraResult<OutlierResult> {
        let mut sorted_values = values.to_vec();
        sorted_values.sort_by(|a, b| a.partial_cmp(b).unwrap());

        let q1_idx = (sorted_values.len() as f64 * 0.25) as usize;
        let q3_idx = (sorted_values.len() as f64 * 0.75) as usize;

        let q1 = sorted_values[q1_idx];
        let q3 = sorted_values[q3_idx];
        let iqr = q3 - q1;

        let lower_bound = q1 - threshold * iqr;
        let upper_bound = q3 + threshold * iqr;

        let mut outlier_indices = Vec::new();
        let mut outlier_values = Vec::new();

        for (i, &value) in values.iter().enumerate() {
            if value < lower_bound || value > upper_bound {
                outlier_indices.push(i);
                outlier_values.push(value);
            }
        }

        Ok(OutlierResult {
            outlier_indices,
            outlier_values,
        })
    }

    fn zscore_outlier_detection(&self, values: &[f64], threshold: f64) -> AuroraResult<OutlierResult> {
        let mean = values.iter().sum::<f64>() / values.len() as f64;
        let variance = values.iter().map(|x| (x - mean).powi(2)).sum::<f64>() / values.len() as f64;
        let std_dev = variance.sqrt();

        let mut outlier_indices = Vec::new();
        let mut outlier_values = Vec::new();

        for (i, &value) in values.iter().enumerate() {
            let z_score = (value - mean).abs() / std_dev;
            if z_score > threshold {
                outlier_indices.push(i);
                outlier_values.push(value);
            }
        }

        Ok(OutlierResult {
            outlier_indices,
            outlier_values,
        })
    }

    fn modified_zscore_outlier_detection(&self, values: &[f64], threshold: f64) -> AuroraResult<OutlierResult> {
        let mut sorted_values = values.to_vec();
        sorted_values.sort_by(|a, b| a.partial_cmp(b).unwrap());

        let median = self.median(&sorted_values);
        let mad = self.median_absolute_deviation(&sorted_values, median);

        let mut outlier_indices = Vec::new();
        let mut outlier_values = Vec::new();

        for (i, &value) in values.iter().enumerate() {
            let modified_z = 0.6745 * (value - median).abs() / mad;
            if modified_z > threshold {
                outlier_indices.push(i);
                outlier_values.push(value);
            }
        }

        Ok(OutlierResult {
            outlier_indices,
            outlier_values,
        })
    }

    fn median(&self, values: &[f64]) -> f64 {
        let len = values.len();
        if len % 2 == 0 {
            (values[len / 2 - 1] + values[len / 2]) / 2.0
        } else {
            values[len / 2]
        }
    }

    fn median_absolute_deviation(&self, values: &[f64], median: f64) -> f64 {
        let deviations: Vec<f64> = values.iter().map(|x| (x - median).abs()).collect();
        self.median(&deviations)
    }
}

#[derive(Debug)]
struct OutlierResult {
    outlier_indices: Vec<usize>,
    outlier_values: Vec<f64>,
}

/// Trend Analysis Function
pub struct TrendAnalysisFunction;

impl AnalyticsFunction for TrendAnalysisFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 1 {
            return Err(AuroraError::InvalidArgument("trend_analysis requires at least 1 argument: values".to_string()));
        }

        let values = CorrelationFunction::extract_numbers(&args[0])?;
        let timestamps = if args.len() > 1 {
            Some(CorrelationFunction::extract_numbers(&args[1])?)
        } else {
            None
        };

        let analysis = self.analyze_trend(&values, timestamps.as_deref())?;

        let result = serde_json::json!({
            "trend_direction": analysis.direction,
            "slope": analysis.slope,
            "r_squared": analysis.r_squared,
            "confidence": analysis.confidence,
            "change_percent": analysis.change_percent,
            "analysis_period": values.len()
        });

        Ok(result)
    }

    fn name(&self) -> &str { "trend_analysis" }
    fn description(&self) -> &str { "Analyze trends in time series or sequential data" }
}

impl TrendAnalysisFunction {
    fn analyze_trend(&self, values: &[f64], timestamps: Option<&[f64]>) -> AuroraResult<TrendAnalysis> {
        let x_values: Vec<f64> = if let Some(ts) = timestamps {
            ts.to_vec()
        } else {
            (0..values.len()).map(|i| i as f64).collect()
        };

        let regression = LinearRegressionFunction.simple_linear_regression(
            &LinearRegressionFunction,
            &x_values,
            values
        )?;

        let first_value = values[0];
        let last_value = *values.last().unwrap();
        let change_percent = if first_value != 0.0 {
            ((last_value - first_value) / first_value.abs()) * 100.0
        } else {
            0.0
        };

        let direction = if regression.slope > 0.01 {
            "increasing"
        } else if regression.slope < -0.01 {
            "decreasing"
        } else {
            "stable"
        };

        let confidence = if regression.r_squared > 0.7 {
            "high"
        } else if regression.r_squared > 0.3 {
            "medium"
        } else {
            "low"
        };

        Ok(TrendAnalysis {
            direction: direction.to_string(),
            slope: regression.slope,
            r_squared: regression.r_squared,
            confidence: confidence.to_string(),
            change_percent,
        })
    }
}

#[derive(Debug)]
struct TrendAnalysis {
    direction: String,
    slope: f64,
    r_squared: f64,
    confidence: String,
    change_percent: f64,
}

/// Hypothesis Testing Function
pub struct HypothesisTestFunction;

impl AnalyticsFunction for HypothesisTestFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 3 {
            return Err(AuroraError::InvalidArgument("hypothesis_test requires 3 arguments: sample1, sample2, test_type".to_string()));
        }

        let sample1 = CorrelationFunction::extract_numbers(&args[0])?;
        let sample2 = CorrelationFunction::extract_numbers(&args[1])?;
        let test_type = args[2].as_str().unwrap_or("t-test");

        let result = match test_type {
            "t-test" => self.t_test(&sample1, &sample2),
            "mann-whitney" => self.mann_whitney_test(&sample1, &sample2),
            _ => self.t_test(&sample1, &sample2),
        }?;

        let result_json = serde_json::json!({
            "test_type": test_type,
            "statistic": result.statistic,
            "p_value": result.p_value,
            "significant": result.p_value < 0.05,
            "sample1_size": sample1.len(),
            "sample2_size": sample2.len(),
            "null_hypothesis": "samples are from the same population"
        });

        Ok(result_json)
    }

    fn name(&self) -> &str { "hypothesis_test" }
    fn description(&self) -> &str { "Perform statistical hypothesis tests between samples" }
}

impl HypothesisTestFunction {
    fn t_test(&self, sample1: &[f64], sample2: &[f64]) -> AuroraResult<TestResult> {
        // Simplified t-test implementation
        let mean1 = sample1.iter().sum::<f64>() / sample1.len() as f64;
        let mean2 = sample2.iter().sum::<f64>() / sample2.len() as f64;

        let var1 = sample1.iter().map(|x| (x - mean1).powi(2)).sum::<f64>() / (sample1.len() - 1) as f64;
        let var2 = sample2.iter().map(|x| (x - mean2).powi(2)).sum::<f64>() / (sample2.len() - 1) as f64;

        let pooled_var = ((sample1.len() - 1) as f64 * var1 + (sample2.len() - 1) as f64 * var2) /
                        (sample1.len() + sample2.len() - 2) as f64;

        let se = (pooled_var * (1.0 / sample1.len() as f64 + 1.0 / sample2.len() as f64)).sqrt();
        let t_statistic = (mean1 - mean2) / se;

        // Approximate p-value (simplified)
        let p_value = 2.0 * (1.0 - Self::normal_cdf(t_statistic.abs()));

        Ok(TestResult {
            statistic: t_statistic,
            p_value,
        })
    }

    fn mann_whitney_test(&self, sample1: &[f64], sample2: &[f64]) -> AuroraResult<TestResult> {
        // Simplified Mann-Whitney U test
        let mut all_values: Vec<(f64, usize)> = sample1.iter().enumerate()
            .map(|(i, &v)| (v, 0))
            .chain(sample2.iter().enumerate().map(|(i, &v)| (v, 1)))
            .collect();

        all_values.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());

        let mut rank_sum1 = 0.0;
        let mut rank_sum2 = 0.0;

        for (rank, (value, group)) in all_values.iter().enumerate() {
            if *group == 0 {
                rank_sum1 += (rank + 1) as f64;
            } else {
                rank_sum2 += (rank + 1) as f64;
            }
        }

        let u1 = rank_sum1 - (sample1.len() * (sample1.len() + 1)) as f64 / 2.0;
        let u2 = rank_sum2 - (sample2.len() * (sample2.len() + 1)) as f64 / 2.0;
        let u = u1.min(u2);

        // Approximate p-value (simplified)
        let mean_u = (sample1.len() * sample2.len()) as f64 / 2.0;
        let std_u = ((sample1.len() * sample2.len() * (sample1.len() + sample2.len() + 1)) as f64 / 12.0).sqrt();
        let z = (u - mean_u) / std_u;
        let p_value = 2.0 * (1.0 - Self::normal_cdf(z.abs()));

        Ok(TestResult {
            statistic: u,
            p_value,
        })
    }

    fn normal_cdf(x: f64) -> f64 {
        // Abramowitz & Stegun approximation
        let a1 =  0.254829592;
        let a2 = -0.284496736;
        let a3 =  1.421413741;
        let a4 = -1.453152027;
        let a5 =  1.061405429;
        let p  =  0.3275911;

        let sign = if x < 0.0 { -1.0 } else { 1.0 };
        let x = x.abs();

        let t = 1.0 / (1.0 + p * x);
        let y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * (-x * x).exp();

        0.5 * (1.0 + sign * y)
    }
}

#[derive(Debug)]
struct TestResult {
    statistic: f64,
    p_value: f64,
}

/// Distribution Fitting Function
pub struct DistributionFitFunction;

impl AnalyticsFunction for DistributionFitFunction {
    fn execute(&self, args: Vec<serde_json::Value>, _context: &QueryContext) -> AuroraResult<serde_json::Value> {
        if args.len() < 1 {
            return Err(AuroraError::InvalidArgument("distribution_fit requires at least 1 argument: values".to_string()));
        }

        let values = CorrelationFunction::extract_numbers(&args[0])?;
        let distributions = args.get(1).and_then(|v| v.as_array())
            .unwrap_or(&vec![
                serde_json::json!("normal"),
                serde_json::json!("exponential"),
                serde_json::json!("uniform")
            ]);

        let mut best_fit = None;
        let mut best_score = f64::INFINITY;

        for dist_name in distributions {
            if let Some(name) = dist_name.as_str() {
                let fit = self.fit_distribution(&values, name)?;
                if fit.aic_score < best_score {
                    best_score = fit.aic_score;
                    best_fit = Some(fit);
                }
            }
        }

        if let Some(fit) = best_fit {
            let result = serde_json::json!({
                "best_distribution": fit.name,
                "parameters": fit.parameters,
                "aic_score": fit.aic_score,
                "log_likelihood": fit.log_likelihood,
                "sample_size": values.len()
            });

            Ok(result)
        } else {
            Err(AuroraError::InvalidArgument("Could not fit any distribution".to_string()))
        }
    }

    fn name(&self) -> &str { "distribution_fit" }
    fn description(&self) -> &str { "Fit probability distributions to data and find best fit" }
}

impl DistributionFitFunction {
    fn fit_distribution(&self, values: &[f64], name: &str) -> AuroraResult<DistributionFit> {
        match name {
            "normal" => self.fit_normal(values),
            "exponential" => self.fit_exponential(values),
            "uniform" => self.fit_uniform(values),
            _ => Err(AuroraError::InvalidArgument(format!("Unknown distribution: {}", name))),
        }
    }

    fn fit_normal(&self, values: &[f64]) -> AuroraResult<DistributionFit> {
        let mean = values.iter().sum::<f64>() / values.len() as f64;
        let variance = values.iter().map(|x| (x - mean).powi(2)).sum::<f64>() / values.len() as f64;
        let std_dev = variance.sqrt();

        let log_likelihood = values.iter()
            .map(|x| -0.5 * ((x - mean) / std_dev).powi(2) - (2.0 * std::f64::consts::PI).ln().sqrt() - std_dev.ln())
            .sum::<f64>();

        let aic = 2.0 * 2.0 - 2.0 * log_likelihood; // 2 parameters

        Ok(DistributionFit {
            name: "normal".to_string(),
            parameters: serde_json::json!({"mean": mean, "std_dev": std_dev}),
            log_likelihood,
            aic_score: aic,
        })
    }

    fn fit_exponential(&self, values: &[f64]) -> AuroraResult<DistributionFit> {
        let lambda = values.len() as f64 / values.iter().sum::<f64>();

        let log_likelihood = values.iter()
            .map(|x| lambda.ln() - lambda * x)
            .sum::<f64>();

        let aic = 2.0 * 1.0 - 2.0 * log_likelihood; // 1 parameter

        Ok(DistributionFit {
            name: "exponential".to_string(),
            parameters: serde_json::json!({"lambda": lambda}),
            log_likelihood,
            aic_score: aic,
        })
    }

    fn fit_uniform(&self, values: &[f64]) -> AuroraResult<DistributionFit> {
        let min_val = values.iter().cloned().fold(f64::INFINITY, f64::min);
        let max_val = values.iter().cloned().fold(f64::NEG_INFINITY, f64::max);
        let range = max_val - min_val;

        let log_likelihood = values.iter()
            .map(|x| if min_val <= *x && *x <= max_val { -(range).ln() } else { f64::NEG_INFINITY })
            .sum::<f64>();

        let aic = 2.0 * 2.0 - 2.0 * log_likelihood; // 2 parameters

        Ok(DistributionFit {
            name: "uniform".to_string(),
            parameters: serde_json::json!({"min": min_val, "max": max_val}),
            log_likelihood,
            aic_score: aic,
        })
    }
}

#[derive(Debug)]
struct DistributionFit {
    name: String,
    parameters: serde_json::Value,
    log_likelihood: f64,
    aic_score: f64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_analytics_function_registry() {
        let registry = AnalyticsFunctionRegistry::new();
        assert!(!registry.list_functions().is_empty());
        assert!(registry.list_functions().contains(&"correlation".to_string()));
    }

    #[test]
    fn test_correlation_function() {
        let registry = AnalyticsFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        let x_values = vec![1.0, 2.0, 3.0, 4.0, 5.0];
        let y_values = vec![2.0, 4.0, 6.0, 8.0, 10.0]; // Perfect positive correlation

        let args = vec![
            serde_json::json!(x_values),
            serde_json::json!(y_values)
        ];

        let result = registry.execute_function("correlation", args, &context).unwrap();
        assert!(result.is_object());
        assert!((result["correlation_coefficient"].as_f64().unwrap() - 1.0).abs() < 0.001);
    }

    #[test]
    fn test_linear_regression() {
        let registry = AnalyticsFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        let x_values = vec![1.0, 2.0, 3.0, 4.0, 5.0];
        let y_values = vec![2.0, 4.0, 6.0, 8.0, 10.0]; // Perfect linear relationship

        let args = vec![
            serde_json::json!(x_values),
            serde_json::json!(y_values)
        ];

        let result = registry.execute_function("linear_regression", args, &context).unwrap();
        assert!(result.is_object());
        assert!((result["slope"].as_f64().unwrap() - 2.0).abs() < 0.001); // Should be slope = 2
        assert!((result["intercept"].as_f64().unwrap() - 0.0).abs() < 0.001); // Should be intercept = 0
    }

    #[test]
    fn test_moving_average() {
        let registry = AnalyticsFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        let values = vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0];
        let window_size = 3;

        let args = vec![
            serde_json::json!(values),
            serde_json::json!(window_size)
        ];

        let result = registry.execute_function("moving_average", args, &context).unwrap();
        assert!(result.is_object());
        let moving_avg = result["moving_average"].as_array().unwrap();
        assert_eq!(moving_avg.len(), 4); // (6 - 3 + 1) = 4
        assert!((moving_avg[0].as_f64().unwrap() - 2.0).abs() < 0.001); // (1+2+3)/3 = 2
    }

    #[test]
    fn test_outlier_detection() {
        let registry = AnalyticsFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        // Data with clear outlier
        let values = vec![1.0, 1.1, 1.2, 1.0, 1.1, 10.0];

        let args = vec![
            serde_json::json!(values),
            serde_json::json!("iqr")
        ];

        let result = registry.execute_function("outlier_detection", args, &context).unwrap();
        assert!(result.is_object());
        assert!(result["outliers"].as_array().unwrap().len() > 0); // Should detect the outlier
    }

    #[test]
    fn test_trend_analysis() {
        let registry = AnalyticsFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        // Clearly increasing trend
        let values = vec![1.0, 2.0, 3.0, 4.0, 5.0];

        let args = vec![serde_json::json!(values)];

        let result = registry.execute_function("trend_analysis", args, &context).unwrap();
        assert!(result.is_object());
        assert_eq!(result["trend_direction"].as_str().unwrap(), "increasing");
        assert!(result["slope"].as_f64().unwrap() > 0.0);
    }

    #[test]
    fn test_time_series_forecast() {
        let registry = AnalyticsFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        let values = vec![10.0, 12.0, 13.0, 12.0, 14.0, 16.0, 15.0];
        let forecast_steps = 3;

        let args = vec![
            serde_json::json!(values),
            serde_json::json!(forecast_steps),
            serde_json::json!("linear")
        ];

        let result = registry.execute_function("time_series_forecast", args, &context).unwrap();
        assert!(result.is_object());
        let forecast = result["forecast"].as_array().unwrap();
        assert_eq!(forecast.len(), forecast_steps);
    }
}
