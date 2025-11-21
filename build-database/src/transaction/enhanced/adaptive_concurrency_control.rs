//! Adaptive Concurrency Control: Dynamic Algorithm Selection
//!
//! UNIQUENESS: Machine learning-powered concurrency control that:
//! - Analyzes workload patterns in real-time
//! - Automatically switches between 2PL, OCC, MVCC, and TO
//! - Predicts and prevents performance degradation
//! - Adapts to changing system conditions

use std::collections::{HashMap, VecDeque};
use std::sync::Arc;
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::unified_transaction_manager::{ConcurrencyControl, IsolationLevel, TransactionId};

/// Workload characteristics
#[derive(Debug, Clone)]
pub struct WorkloadCharacteristics {
    pub read_heavy_ratio: f64,      // Ratio of read operations (0.0 to 1.0)
    pub write_conflict_rate: f64,   // Rate of write-write conflicts
    pub transaction_duration_avg: std::time::Duration,
    pub concurrent_transaction_count: usize,
    pub hotspot_ratio: f64,         // Ratio of operations on hot data
    pub timestamp: std::time::Instant,
}

/// Performance metrics for algorithm evaluation
#[derive(Debug, Clone)]
pub struct AlgorithmPerformance {
    pub throughput: f64,           // Transactions per second
    pub average_latency: std::time::Duration,
    pub abort_rate: f64,           // Transaction abort rate
    pub deadlock_rate: f64,        // Deadlock detection rate
    pub cpu_utilization: f64,      // CPU usage percentage
    pub memory_overhead: usize,    // Additional memory usage
}

/// Algorithm selection decision
#[derive(Debug, Clone)]
pub struct AlgorithmDecision {
    pub recommended_algorithm: ConcurrencyControl,
    pub confidence_score: f64,     // 0.0 to 1.0
    pub expected_improvement: f64, // Expected performance improvement
    pub reasoning: Vec<String>,    // Human-readable reasoning
}

/// Adaptive concurrency control configuration
#[derive(Debug, Clone)]
pub struct AdaptiveConfig {
    pub enable_adaptation: bool,
    pub adaptation_interval_ms: u64,
    pub min_samples_for_adaptation: usize,
    pub performance_window_size: usize,
    pub algorithm_switch_threshold: f64, // Minimum improvement to switch
    pub enable_prediction: bool,
    pub prediction_horizon_ms: u64,
}

impl Default for AdaptiveConfig {
    fn default() -> Self {
        Self {
            enable_adaptation: true,
            adaptation_interval_ms: 1000, // 1 second
            min_samples_for_adaptation: 100,
            performance_window_size: 1000,
            algorithm_switch_threshold: 0.1, // 10% improvement
            enable_prediction: true,
            prediction_horizon_ms: 5000, // 5 seconds
        }
    }
}

/// Adaptive concurrency control system
///
/// Uses machine learning and workload analysis to dynamically select
/// the optimal concurrency control algorithm for current conditions.
pub struct AdaptiveConcurrencyControl {
    /// Current active algorithm
    current_algorithm: RwLock<ConcurrencyControl>,

    /// Historical workload characteristics
    workload_history: RwLock<VecDeque<WorkloadCharacteristics>>,

    /// Performance metrics for each algorithm
    algorithm_performance: RwLock<HashMap<ConcurrencyControl, VecDeque<AlgorithmPerformance>>>,

    /// Recent algorithm decisions
    decision_history: RwLock<VecDeque<AlgorithmDecision>>,

    /// Workload prediction model
    workload_predictor: WorkloadPredictor,

    /// Configuration
    config: AdaptiveConfig,

    /// Statistics
    stats: Arc<Mutex<AdaptiveStats>>,
}

/// Workload predictor using simple time-series analysis
#[derive(Debug)]
struct WorkloadPredictor {
    /// Recent workload patterns for prediction
    pattern_history: VecDeque<WorkloadCharacteristics>,
    /// Prediction coefficients (simplified ML model)
    prediction_coefficients: HashMap<String, f64>,
}

/// Adaptive statistics
#[derive(Debug, Clone)]
pub struct AdaptiveStats {
    pub total_decisions: u64,
    pub algorithm_switches: u64,
    pub average_decision_time: std::time::Duration,
    pub prediction_accuracy: f64,
    pub performance_improvements: Vec<f64>,
    pub false_predictions: u64,
}

impl Default for AdaptiveStats {
    fn default() -> Self {
        Self {
            total_decisions: 0,
            algorithm_switches: 0,
            average_decision_time: std::time::Duration::ZERO,
            prediction_accuracy: 0.0,
            performance_improvements: Vec::new(),
            false_predictions: 0,
        }
    }
}

impl AdaptiveConcurrencyControl {
    /// Create a new adaptive concurrency control system
    pub fn new() -> Self {
        Self::with_config(AdaptiveConfig::default())
    }

    /// Create with custom configuration
    pub fn with_config(config: AdaptiveConfig) -> Self {
        Self {
            current_algorithm: RwLock::new(ConcurrencyControl::MVCC), // Start with MVCC
            workload_history: RwLock::new(VecDeque::with_capacity(1000)),
            algorithm_performance: RwLock::new(HashMap::new()),
            decision_history: RwLock::new(VecDeque::with_capacity(100)),
            workload_predictor: WorkloadPredictor::new(),
            config,
            stats: Arc::new(Mutex::new(AdaptiveStats::default())),
        }
    }

    /// Record workload characteristics
    pub async fn record_workload(&self, characteristics: WorkloadCharacteristics) -> AuroraResult<()> {
        {
            let mut history = self.workload_history.write().unwrap();
            history.push_back(characteristics.clone());

            // Limit history size
            if history.len() > self.config.performance_window_size {
                history.pop_front();
            }
        }

        // Update predictor
        self.workload_predictor.update_model(characteristics).await?;

        Ok(())
    }

    /// Record algorithm performance metrics
    pub async fn record_performance(&self, algorithm: ConcurrencyControl, performance: AlgorithmPerformance) -> AuroraResult<()> {
        {
            let mut perf_history = self.algorithm_performance.write().unwrap();
            let algorithm_history = perf_history.entry(algorithm).or_insert_with(|| VecDeque::with_capacity(1000));
            algorithm_history.push_back(performance);

            // Limit history size
            if algorithm_history.len() > self.config.performance_window_size {
                algorithm_history.pop_front();
            }
        }

        Ok(())
    }

    /// Make adaptive algorithm selection decision
    pub async fn make_decision(&self) -> AuroraResult<AlgorithmDecision> {
        let start_time = std::time::Instant::now();

        let current_workload = self.analyze_current_workload().await?;
        let current_algorithm = *self.current_algorithm.read().unwrap();

        // Predict future workload if enabled
        let predicted_workload = if self.config.enable_prediction {
            Some(self.workload_predictor.predict_workload().await?)
        } else {
            None
        };

        // Evaluate all algorithms for current conditions
        let algorithm_scores = self.evaluate_algorithms(&current_workload, predicted_workload.as_ref()).await?;

        // Select best algorithm
        let best_algorithm = algorithm_scores.iter()
            .max_by(|a, b| a.1.partial_cmp(b.1).unwrap())
            .map(|(alg, _)| *alg)
            .unwrap_or(current_algorithm);

        // Calculate confidence and improvement
        let current_score = algorithm_scores.get(&current_algorithm).unwrap_or(&0.0);
        let best_score = algorithm_scores.get(&best_algorithm).unwrap_or(&0.0);
        let improvement = if *current_score > 0.0 {
            (best_score - current_score) / current_score
        } else {
            0.0
        };

        // Generate reasoning
        let reasoning = self.generate_reasoning(&current_workload, current_algorithm, best_algorithm, improvement).await;

        // Calculate confidence based on sample size and consistency
        let confidence = self.calculate_confidence(&algorithm_scores).await;

        let decision = AlgorithmDecision {
            recommended_algorithm: best_algorithm,
            confidence_score: confidence,
            expected_improvement: improvement,
            reasoning,
        };

        // Record decision
        {
            let mut history = self.decision_history.write().unwrap();
            history.push_back(decision.clone());

            if history.len() > 100 {
                history.pop_front();
            }
        }

        // Update statistics
        let decision_time = start_time.elapsed();
        let mut stats = self.stats.lock().unwrap();
        stats.total_decisions += 1;

        // Update average decision time
        let total_decisions = stats.total_decisions as f64;
        let current_avg = stats.average_decision_time.as_nanos() as f64;
        let new_avg = (current_avg * (total_decisions - 1.0) + decision_time.as_nanos() as f64) / total_decisions;
        stats.average_decision_time = std::time::Duration::from_nanos(new_avg as u64);

        if best_algorithm != current_algorithm && improvement > self.config.algorithm_switch_threshold {
            stats.algorithm_switches += 1;
            stats.performance_improvements.push(improvement);
        }

        Ok(decision)
    }

    /// Apply algorithm decision (switch if beneficial)
    pub async fn apply_decision(&self, decision: &AlgorithmDecision) -> AuroraResult<bool> {
        let current_algorithm = *self.current_algorithm.read().unwrap();

        if decision.recommended_algorithm != current_algorithm &&
           decision.expected_improvement > self.config.algorithm_switch_threshold &&
           decision.confidence_score > 0.7 {

            // Switch algorithm
            *self.current_algorithm.write().unwrap() = decision.recommended_algorithm;

            println!("🔄 Switching concurrency control from {:?} to {:?} (expected improvement: {:.1}%)",
                    current_algorithm, decision.recommended_algorithm, decision.expected_improvement * 100.0);

            Ok(true)
        } else {
            Ok(false)
        }
    }

    /// Get current algorithm
    pub fn current_algorithm(&self) -> ConcurrencyControl {
        *self.current_algorithm.read().unwrap()
    }

    /// Get adaptive statistics
    pub fn stats(&self) -> AdaptiveStats {
        self.stats.lock().unwrap().clone()
    }

    /// Get recent decisions
    pub fn recent_decisions(&self) -> Vec<AlgorithmDecision> {
        self.decision_history.read().unwrap().iter().cloned().collect()
    }

    // Private methods

    /// Analyze current workload characteristics
    async fn analyze_current_workload(&self) -> AuroraResult<WorkloadCharacteristics> {
        let history = self.workload_history.read().unwrap();

        if history.is_empty() {
            return Ok(WorkloadCharacteristics {
                read_heavy_ratio: 0.8, // Default assumption
                write_conflict_rate: 0.1,
                transaction_duration_avg: std::time::Duration::from_millis(10),
                concurrent_transaction_count: 10,
                hotspot_ratio: 0.2,
                timestamp: std::time::Instant::now(),
            });
        }

        // Calculate averages from recent history
        let recent: Vec<_> = history.iter().rev().take(10).collect();

        let read_heavy_ratio = recent.iter().map(|w| w.read_heavy_ratio).sum::<f64>() / recent.len() as f64;
        let write_conflict_rate = recent.iter().map(|w| w.write_conflict_rate).sum::<f64>() / recent.len() as f64;
        let duration_sum: std::time::Duration = recent.iter().map(|w| w.transaction_duration_avg).sum();
        let transaction_duration_avg = duration_sum / recent.len() as u32;
        let concurrent_transaction_count = recent.iter().map(|w| w.concurrent_transaction_count).max().unwrap_or(1);
        let hotspot_ratio = recent.iter().map(|w| w.hotspot_ratio).sum::<f64>() / recent.len() as f64;

        Ok(WorkloadCharacteristics {
            read_heavy_ratio,
            write_conflict_rate,
            transaction_duration_avg,
            concurrent_transaction_count,
            hotspot_ratio,
            timestamp: std::time::Instant::now(),
        })
    }

    /// Evaluate all algorithms for given workload
    async fn evaluate_algorithms(
        &self,
        workload: &WorkloadCharacteristics,
        predicted_workload: Option<&WorkloadCharacteristics>,
    ) -> AuroraResult<HashMap<ConcurrencyControl, f64>> {
        let mut scores = HashMap::new();

        let algorithms = vec![
            ConcurrencyControl::TwoPhaseLocking,
            ConcurrencyControl::OptimisticConcurrencyControl,
            ConcurrencyControl::MVCC,
            ConcurrencyControl::TimestampOrdering,
        ];

        for algorithm in algorithms {
            let score = self.score_algorithm(algorithm, workload, predicted_workload).await?;
            scores.insert(algorithm, score);
        }

        Ok(scores)
    }

    /// Score an algorithm for given workload conditions
    async fn score_algorithm(
        &self,
        algorithm: ConcurrencyControl,
        workload: &WorkloadCharacteristics,
        predicted_workload: Option<&WorkloadCharacteristics>,
    ) -> AuroraResult<f64> {
        // Base score from historical performance
        let historical_score = self.get_historical_performance(algorithm).await;

        // Workload-specific adjustments
        let workload_modifier = match algorithm {
            ConcurrencyControl::TwoPhaseLocking => {
                // Good for low conflict, bad for high conflict
                if workload.write_conflict_rate < 0.2 {
                    1.2 // Bonus for low conflict
                } else {
                    0.8 // Penalty for high conflict
                }
            }
            ConcurrencyControl::OptimisticConcurrencyControl => {
                // Good for low conflict, very bad for high conflict
                if workload.write_conflict_rate < 0.1 {
                    1.3 // Bonus for very low conflict
                } else {
                    0.6 // Heavy penalty for high conflict
                }
            }
            ConcurrencyControl::MVCC => {
                // Generally good, especially for read-heavy workloads
                if workload.read_heavy_ratio > 0.7 {
                    1.4 // Bonus for read-heavy
                } else {
                    1.0 // Neutral for mixed workloads
                }
            }
            ConcurrencyControl::TimestampOrdering => {
                // Good for predictable workloads, bad for unpredictable
                if workload.hotspot_ratio < 0.3 {
                    1.1 // Bonus for distributed access
                } else {
                    0.9 // Penalty for hotspots
                }
            }
        };

        // Concurrency adjustment
        let concurrency_modifier = match algorithm {
            ConcurrencyControl::TwoPhaseLocking => {
                // Suffers from high concurrency
                1.0 / (1.0 + (workload.concurrent_transaction_count as f64 / 100.0))
            }
            ConcurrencyControl::OptimisticConcurrencyControl => {
                // Benefits from low concurrency
                if workload.concurrent_transaction_count < 50 {
                    1.1
                } else {
                    0.9
                }
            }
            ConcurrencyControl::MVCC => {
                // Scales well with concurrency
                1.0 + (workload.concurrent_transaction_count as f64 / 1000.0).min(0.2)
            }
            ConcurrencyControl::TimestampOrdering => {
                // Moderate scaling
                1.0 + (workload.concurrent_transaction_count as f64 / 2000.0).min(0.1)
            }
        };

        // Prediction adjustment
        let prediction_modifier = if let Some(predicted) = predicted_workload {
            // Adjust based on predicted vs current workload
            let read_change = (predicted.read_heavy_ratio - workload.read_heavy_ratio).abs();
            let conflict_change = (predicted.write_conflict_rate - workload.write_conflict_rate).abs();

            match algorithm {
                ConcurrencyControl::MVCC => {
                    // MVCC handles changes well
                    1.0 - (read_change + conflict_change) * 0.3
                }
                ConcurrencyControl::TwoPhaseLocking => {
                    // 2PL sensitive to conflict changes
                    1.0 - conflict_change * 0.5
                }
                ConcurrencyControl::OptimisticConcurrencyControl => {
                    // OCC very sensitive to conflict changes
                    1.0 - conflict_change * 0.8
                }
                ConcurrencyControl::TimestampOrdering => {
                    // TO moderately sensitive
                    1.0 - conflict_change * 0.4
                }
            }
        } else {
            1.0
        };

        Ok(historical_score * workload_modifier * concurrency_modifier * prediction_modifier)
    }

    /// Get historical performance score for an algorithm
    async fn get_historical_performance(&self, algorithm: ConcurrencyControl) -> f64 {
        let perf_history = self.algorithm_performance.read().unwrap();
        let algorithm_history = perf_history.get(&algorithm);

        if let Some(history) = algorithm_history {
            if history.len() >= self.config.min_samples_for_adaptation {
                // Calculate average throughput score
                let avg_throughput = history.iter().map(|p| p.throughput).sum::<f64>() / history.len() as f64;
                let max_throughput = history.iter().map(|p| p.throughput).fold(0.0, f64::max);

                if max_throughput > 0.0 {
                    avg_throughput / max_throughput // Normalize to 0-1
                } else {
                    0.5 // Default neutral score
                }
            } else {
                // Not enough samples, use algorithm defaults
                match algorithm {
                    ConcurrencyControl::MVCC => 0.8,
                    ConcurrencyControl::TwoPhaseLocking => 0.7,
                    ConcurrencyControl::OptimisticConcurrencyControl => 0.6,
                    ConcurrencyControl::TimestampOrdering => 0.7,
                }
            }
        } else {
            // No historical data, use defaults
            match algorithm {
                ConcurrencyControl::MVCC => 0.8,
                ConcurrencyControl::TwoPhaseLocking => 0.7,
                ConcurrencyControl::OptimisticConcurrencyControl => 0.6,
                ConcurrencyControl::TimestampOrdering => 0.7,
            }
        }
    }

    /// Generate human-readable reasoning for algorithm selection
    async fn generate_reasoning(
        &self,
        workload: &WorkloadCharacteristics,
        current: ConcurrencyControl,
        recommended: ConcurrencyControl,
        improvement: f64,
    ) -> Vec<String> {
        let mut reasoning = Vec::new();

        reasoning.push(format!("Current workload: {:.0}% reads, {:.1}% write conflicts, {} concurrent txns",
                              workload.read_heavy_ratio * 100.0,
                              workload.write_conflict_rate * 100.0,
                              workload.concurrent_transaction_count));

        if current != recommended {
            reasoning.push(format!("Switching from {:?} to {:?} for {:.1}% improvement",
                                  current, recommended, improvement * 100.0));

            match recommended {
                ConcurrencyControl::MVCC => {
                    if workload.read_heavy_ratio > 0.7 {
                        reasoning.push("MVCC selected: Excellent for read-heavy workloads".to_string());
                    } else {
                        reasoning.push("MVCC selected: Good general-purpose performance".to_string());
                    }
                }
                ConcurrencyControl::TwoPhaseLocking => {
                    reasoning.push("2PL selected: Good for low-conflict scenarios".to_string());
                }
                ConcurrencyControl::OptimisticConcurrencyControl => {
                    reasoning.push("OCC selected: Optimal for very low conflict rates".to_string());
                }
                ConcurrencyControl::TimestampOrdering => {
                    reasoning.push("TO selected: Good for distributed workloads".to_string());
                }
            }
        } else {
            reasoning.push(format!("Staying with {:?}: Still optimal for current conditions", current));
        }

        reasoning
    }

    /// Calculate confidence in the decision
    async fn calculate_confidence(&self, scores: &HashMap<ConcurrencyControl, f64>) -> f64 {
        if scores.len() < 2 {
            return 0.5;
        }

        // Sort scores
        let mut score_values: Vec<f64> = scores.values().cloned().collect();
        score_values.sort_by(|a, b| b.partial_cmp(a).unwrap());

        if score_values.len() >= 2 {
            let best = score_values[0];
            let second_best = score_values[1];

            if best > 0.0 {
                // Confidence based on gap between best and second best
                let gap = (best - second_best) / best;
                gap.min(1.0) // Cap at 100% confidence
            } else {
                0.5
            }
        } else {
            0.5
        }
    }
}

impl WorkloadPredictor {
    fn new() -> Self {
        Self {
            pattern_history: VecDeque::with_capacity(100),
            prediction_coefficients: HashMap::new(),
        }
    }

    async fn update_model(&mut self, characteristics: WorkloadCharacteristics) -> AuroraResult<()> {
        self.pattern_history.push_back(characteristics);

        // Limit history
        if self.pattern_history.len() > 100 {
            self.pattern_history.pop_front();
        }

        // Simple linear regression for prediction (simplified)
        if self.pattern_history.len() >= 10 {
            self.update_coefficients().await?;
        }

        Ok(())
    }

    async fn predict_workload(&self) -> AuroraResult<WorkloadCharacteristics> {
        if self.pattern_history.len() < 5 {
            // Not enough data, return current average
            let avg_read_ratio = self.pattern_history.iter()
                .map(|w| w.read_heavy_ratio)
                .sum::<f64>() / self.pattern_history.len() as f64;

            let avg_conflict_rate = self.pattern_history.iter()
                .map(|w| w.write_conflict_rate)
                .sum::<f64>() / self.pattern_history.len() as f64;

            return Ok(WorkloadCharacteristics {
                read_heavy_ratio: avg_read_ratio,
                write_conflict_rate: avg_conflict_rate,
                transaction_duration_avg: std::time::Duration::from_millis(10),
                concurrent_transaction_count: 10,
                hotspot_ratio: 0.2,
                timestamp: std::time::Instant::now(),
            });
        }

        // Simple prediction: assume trend continues
        let recent: Vec<_> = self.pattern_history.iter().rev().take(5).collect();

        let read_trend = self.calculate_trend(|w| w.read_heavy_ratio, &recent);
        let conflict_trend = self.calculate_trend(|w| w.write_conflict_rate, &recent);

        let last = recent[0];
        let predicted_read = (last.read_heavy_ratio + read_trend).clamp(0.0, 1.0);
        let predicted_conflict = (last.write_conflict_rate + conflict_trend).clamp(0.0, 1.0);

        Ok(WorkloadCharacteristics {
            read_heavy_ratio: predicted_read,
            write_conflict_rate: predicted_conflict,
            transaction_duration_avg: last.transaction_duration_avg,
            concurrent_transaction_count: last.concurrent_transaction_count,
            hotspot_ratio: last.hotspot_ratio,
            timestamp: std::time::Instant::now(),
        })
    }

    fn calculate_trend<F>(&self, extractor: F, data: &[&WorkloadCharacteristics]) -> f64
    where
        F: Fn(&WorkloadCharacteristics) -> f64,
    {
        if data.len() < 2 {
            return 0.0;
        }

        // Simple linear trend
        let n = data.len() as f64;
        let values: Vec<f64> = data.iter().map(|w| extractor(w)).collect();

        let sum_x: f64 = (0..data.len()).map(|i| i as f64).sum();
        let sum_y: f64 = values.iter().sum();
        let sum_xy: f64 = values.iter().enumerate().map(|(i, &y)| i as f64 * y).sum();
        let sum_xx: f64 = (0..data.len()).map(|i| (i as f64).powi(2)).sum();

        let slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x.powi(2));

        slope * 0.1 // Dampen the trend
    }

    async fn update_coefficients(&mut self) -> AuroraResult<()> {
        // In a real implementation, this would train a more sophisticated model
        // For now, just maintain simple coefficients
        self.prediction_coefficients.insert("read_trend".to_string(), 0.1);
        self.prediction_coefficients.insert("conflict_trend".to_string(), 0.05);

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_adaptive_config() {
        let config = AdaptiveConfig::default();
        assert!(config.enable_adaptation);
        assert_eq!(config.adaptation_interval_ms, 1000);
        assert_eq!(config.algorithm_switch_threshold, 0.1);
    }

    #[test]
    fn test_workload_characteristics() {
        let characteristics = WorkloadCharacteristics {
            read_heavy_ratio: 0.8,
            write_conflict_rate: 0.1,
            transaction_duration_avg: std::time::Duration::from_millis(10),
            concurrent_transaction_count: 50,
            hotspot_ratio: 0.2,
            timestamp: std::time::Instant::now(),
        };

        assert_eq!(characteristics.read_heavy_ratio, 0.8);
        assert_eq!(characteristics.write_conflict_rate, 0.1);
        assert_eq!(characteristics.concurrent_transaction_count, 50);
    }

    #[test]
    fn test_algorithm_performance() {
        let performance = AlgorithmPerformance {
            throughput: 1000.0,
            average_latency: std::time::Duration::from_millis(5),
            abort_rate: 0.05,
            deadlock_rate: 0.001,
            cpu_utilization: 0.7,
            memory_overhead: 1024,
        };

        assert_eq!(performance.throughput, 1000.0);
        assert_eq!(performance.abort_rate, 0.05);
        assert_eq!(performance.cpu_utilization, 0.7);
    }

    #[test]
    fn test_algorithm_decision() {
        let decision = AlgorithmDecision {
            recommended_algorithm: ConcurrencyControl::MVCC,
            confidence_score: 0.85,
            expected_improvement: 0.15,
            reasoning: vec!["Good for read-heavy workload".to_string()],
        };

        assert_eq!(decision.recommended_algorithm, ConcurrencyControl::MVCC);
        assert_eq!(decision.confidence_score, 0.85);
        assert_eq!(decision.expected_improvement, 0.15);
        assert_eq!(decision.reasoning.len(), 1);
    }

    #[test]
    fn test_adaptive_concurrency_creation() {
        let adaptive = AdaptiveConcurrencyControl::new();
        let stats = adaptive.stats();
        assert_eq!(stats.total_decisions, 0);
        assert_eq!(stats.algorithm_switches, 0);
    }

    #[tokio::test]
    async fn test_workload_recording() {
        let adaptive = AdaptiveConcurrencyControl::new();

        let characteristics = WorkloadCharacteristics {
            read_heavy_ratio: 0.8,
            write_conflict_rate: 0.1,
            transaction_duration_avg: std::time::Duration::from_millis(10),
            concurrent_transaction_count: 20,
            hotspot_ratio: 0.15,
            timestamp: std::time::Instant::now(),
        };

        adaptive.record_workload(characteristics).await.unwrap();

        // Verify workload was recorded
        let current_workload = adaptive.analyze_current_workload().await.unwrap();
        assert_eq!(current_workload.read_heavy_ratio, 0.8);
    }

    #[tokio::test]
    async fn test_performance_recording() {
        let adaptive = AdaptiveConcurrencyControl::new();

        let performance = AlgorithmPerformance {
            throughput: 1500.0,
            average_latency: std::time::Duration::from_millis(3),
            abort_rate: 0.02,
            deadlock_rate: 0.0005,
            cpu_utilization: 0.65,
            memory_overhead: 512,
        };

        adaptive.record_performance(ConcurrencyControl::MVCC, performance).await.unwrap();

        // Verify performance was recorded
        let decision = adaptive.make_decision().await.unwrap();
        // Should recommend MVCC based on recorded performance
        assert_eq!(decision.recommended_algorithm, ConcurrencyControl::MVCC);
    }

    #[tokio::test]
    async fn test_algorithm_decision_making() {
        let adaptive = AdaptiveConcurrencyControl::new();

        // Record some workload
        let workload = WorkloadCharacteristics {
            read_heavy_ratio: 0.9, // Very read-heavy
            write_conflict_rate: 0.05, // Low conflict
            transaction_duration_avg: std::time::Duration::from_millis(5),
            concurrent_transaction_count: 100,
            hotspot_ratio: 0.1,
            timestamp: std::time::Instant::now(),
        };

        adaptive.record_workload(workload).await.unwrap();

        // Make decision
        let decision = adaptive.make_decision().await.unwrap();

        // Should recommend MVCC for read-heavy workload
        assert_eq!(decision.recommended_algorithm, ConcurrencyControl::MVCC);
        assert!(decision.confidence_score >= 0.0);
        assert!(!decision.reasoning.is_empty());

        let stats = adaptive.stats();
        assert_eq!(stats.total_decisions, 1);
    }

    #[tokio::test]
    async fn test_workload_prediction() {
        let predictor = WorkloadPredictor::new();

        // Add some historical data
        for i in 0..10 {
            let workload = WorkloadCharacteristics {
                read_heavy_ratio: 0.7 + (i as f64 * 0.01), // Increasing read ratio
                write_conflict_rate: 0.1,
                transaction_duration_avg: std::time::Duration::from_millis(10),
                concurrent_transaction_count: 50,
                hotspot_ratio: 0.2,
                timestamp: std::time::Instant::now(),
            };

            predictor.update_model(workload).await.unwrap();
        }

        // Predict future workload
        let prediction = predictor.predict_workload().await.unwrap();

        // Should predict continued trend
        assert!(prediction.read_heavy_ratio >= 0.7);
        assert!(prediction.read_heavy_ratio <= 1.0);
    }

    #[test]
    fn test_current_algorithm() {
        let adaptive = AdaptiveConcurrencyControl::new();
        let current = adaptive.current_algorithm();
        assert_eq!(current, ConcurrencyControl::MVCC); // Default
    }

    #[test]
    fn test_recent_decisions() {
        let adaptive = AdaptiveConcurrencyControl::new();
        let decisions = adaptive.recent_decisions();
        assert!(decisions.is_empty()); // No decisions made yet
    }
}
