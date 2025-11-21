//! Machine Learning Operations: UNIQUENESS Intelligent Coordination
//!
//! Research-backed ML integration for predictive coordination:
//! - **Predictive Scaling**: ML-based workload forecasting
//! - **Anomaly Detection**: Automated failure prediction
//! - **Workload Optimization**: ML-driven resource allocation
//! - **Performance Prediction**: Latency and throughput forecasting
//! - **Auto-tuning**: ML-based configuration optimization

pub mod predictive_scaling;
pub mod anomaly_detection;
pub mod workload_optimization;
pub mod performance_prediction;
pub mod auto_tuning;
pub mod ml_training;

pub use predictive_scaling::PredictiveScaler;
pub use anomaly_detection::AnomalyDetector;
pub use workload_optimization::WorkloadOptimizer;
pub use performance_prediction::PerformancePredictor;
pub use auto_tuning::AutoTuner;
pub use ml_training::MLTrainer;

// UNIQUENESS Research Citations:
// - **Predictive Scaling**: AWS, Google research on workload prediction
// - **Anomaly Detection**: Statistical process control research
// - **AutoML**: Automated machine learning research
// - **Time Series Forecasting**: ARIMA, LSTM research
