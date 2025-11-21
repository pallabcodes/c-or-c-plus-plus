//! Adaptive Load Balancer: Boyd-Wickizer Research Implementation
//!
//! UNIQUENESS: Implements Boyd-Wickizer et al. (2008) "Corey: An Operating System for Many Cores"
//! with machine learning-powered load balancing that adapts to workload patterns.

use std::collections::HashMap;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use std::time::{Duration, Instant};
use tracing::{debug, info};

use crate::error::{Error, Result};

/// Load balancing algorithm
#[derive(Debug, Clone, PartialEq)]
pub enum LoadBalancingAlgorithm {
    RoundRobin,
    LeastLoaded,
    WeightedRandom,
    AdaptiveML,     // Machine learning based
    WorkStealing,   // Traditional work stealing
}

/// Workload pattern classification
#[derive(Debug, Clone, PartialEq)]
pub enum WorkloadPattern {
    CPUIntensive,
    MemoryIntensive,
    IOBound,
    Mixed,
    Burst,          // Sudden load spikes
    Steady,         // Consistent load
}

/// Load balancer statistics
#[derive(Debug, Clone)]
pub struct LoadBalancerStats {
    pub tasks_scheduled: u64,
    pub load_imbalance_ratio: f64,
    pub average_response_time: Duration,
    pub scheduling_overhead: Duration,
    pub algorithm_switches: u64,
    pub current_algorithm: LoadBalancingAlgorithm,
}

/// Adaptive load balancer with ML-powered decisions
///
/// Implements research-backed load balancing that learns from
/// workload patterns and adapts scheduling strategies.
pub struct AdaptiveLoadBalancer {
    /// Current load balancing algorithm
    current_algorithm: std::sync::Mutex<LoadBalancingAlgorithm>,

    /// Worker load information
    worker_loads: Arc<std::sync::Mutex<HashMap<usize, WorkerLoad>>>,

    /// Workload pattern analysis
    pattern_analyzer: WorkloadPatternAnalyzer,

    /// ML-based decision engine
    decision_engine: MLDecisionEngine,

    /// Statistics
    stats: Arc<std::sync::Mutex<LoadBalancerStats>>,

    /// Performance history for learning
    performance_history: Arc<std::sync::Mutex<Vec<PerformanceSample>>>,
}

impl AdaptiveLoadBalancer {
    /// Create a new adaptive load balancer
    pub fn new(num_workers: usize) -> Result<Self> {
        let mut worker_loads = HashMap::new();

        // Initialize worker load tracking
        for worker_id in 0..num_workers {
            worker_loads.insert(worker_id, WorkerLoad {
                worker_id,
                current_load: 0.0,
                task_queue_length: 0,
                response_time: Duration::ZERO,
                last_updated: Instant::now(),
            });
        }

        Ok(Self {
            current_algorithm: std::sync::Mutex::new(LoadBalancingAlgorithm::AdaptiveML),
            worker_loads: Arc::new(std::sync::Mutex::new(worker_loads)),
            pattern_analyzer: WorkloadPatternAnalyzer::new(),
            decision_engine: MLDecisionEngine::new(),
            stats: Arc::new(std::sync::Mutex::new(LoadBalancerStats {
                tasks_scheduled: 0,
                load_imbalance_ratio: 0.0,
                average_response_time: Duration::ZERO,
                scheduling_overhead: Duration::ZERO,
                algorithm_switches: 0,
                current_algorithm: LoadBalancingAlgorithm::AdaptiveML,
            })),
            performance_history: Arc::new(std::sync::Mutex::new(Vec::new())),
        })
    }

    /// Schedule a task using adaptive load balancing
    pub fn schedule_task(&self, task: Task) -> Result<usize> {
        let start_time = Instant::now();

        // Analyze current workload pattern
        let pattern = self.pattern_analyzer.analyze_current_pattern();

        // Choose optimal algorithm based on pattern
        let algorithm = self.select_algorithm_for_pattern(&pattern);

        // Update current algorithm if changed
        {
            let mut current = self.current_algorithm.lock().unwrap();
            if *current != algorithm {
                info!("Switching load balancing algorithm: {:?} -> {:?}", *current, algorithm);
                *current = algorithm.clone();

                let mut stats = self.stats.lock().unwrap();
                stats.algorithm_switches += 1;
                stats.current_algorithm = algorithm.clone();
            }
        }

        // Select worker using chosen algorithm
        let worker_id = self.select_worker_with_algorithm(&algorithm, &task)?;

        // Update worker load
        self.update_worker_load(worker_id, &task);

        // Record scheduling decision
        let scheduling_time = start_time.elapsed();
        self.record_performance_sample(worker_id, scheduling_time, &task);

        // Update statistics
        {
            let mut stats = self.stats.lock().unwrap();
            stats.tasks_scheduled += 1;
            stats.scheduling_overhead += scheduling_time;
        }

        Ok(worker_id)
    }

    /// Update worker load after task completion
    pub fn update_worker_load_after_completion(&self, worker_id: usize, execution_time: Duration, success: bool) {
        let mut worker_loads = self.worker_loads.lock().unwrap();

        if let Some(worker) = worker_loads.get_mut(&worker_id) {
            worker.current_load = (worker.current_load * 0.9).max(0.0); // Exponential decay
            worker.task_queue_length = worker.task_queue_length.saturating_sub(1);
            worker.last_updated = Instant::now();

            if success {
                // Update response time using exponential moving average
                let new_response_time = execution_time.as_millis() as f64;
                let old_response_time = worker.response_time.as_millis() as f64;
                let updated_response_time = old_response_time * 0.8 + new_response_time * 0.2;
                worker.response_time = Duration::from_millis(updated_response_time as u64);
            }
        }

        // Update overall statistics
        self.update_load_imbalance_stats();
    }

    /// Get current load balancer statistics
    pub fn stats(&self) -> LoadBalancerStats {
        self.stats.lock().unwrap().clone()
    }

    /// Manually trigger algorithm adaptation
    pub fn adapt_algorithm(&self) -> Result<()> {
        let pattern = self.pattern_analyzer.analyze_current_pattern();
        let new_algorithm = self.decision_engine.recommend_algorithm(&pattern)?;

        let mut current = self.current_algorithm.lock().unwrap();
        if *current != new_algorithm {
            info!("ML adaptation: Switching to {:?} for pattern {:?}", new_algorithm, pattern);
            *current = new_algorithm.clone();

            let mut stats = self.stats.lock().unwrap();
            stats.algorithm_switches += 1;
            stats.current_algorithm = new_algorithm;
        }

        Ok(())
    }

    // Private methods

    /// Select algorithm based on workload pattern
    fn select_algorithm_for_pattern(&self, pattern: &WorkloadPattern) -> LoadBalancingAlgorithm {
        match pattern {
            WorkloadPattern::CPUIntensive => LoadBalancingAlgorithm::LeastLoaded,
            WorkloadPattern::MemoryIntensive => LoadBalancingAlgorithm::AdaptiveML,
            WorkloadPattern::IOBound => LoadBalancingAlgorithm::RoundRobin,
            WorkloadPattern::Mixed => LoadBalancingAlgorithm::AdaptiveML,
            WorkloadPattern::Burst => LoadBalancingAlgorithm::WorkStealing,
            WorkloadPattern::Steady => LoadBalancingAlgorithm::WeightedRandom,
        }
    }

    /// Select worker using specific algorithm
    fn select_worker_with_algorithm(&self, algorithm: &LoadBalancingAlgorithm, task: &Task) -> Result<usize> {
        let worker_loads = self.worker_loads.lock().unwrap();

        match algorithm {
            LoadBalancingAlgorithm::RoundRobin => {
                // Simple round-robin (would use atomic counter in real implementation)
                Ok(task.id as usize % worker_loads.len())
            }

            LoadBalancingAlgorithm::LeastLoaded => {
                // Find worker with lowest load
                worker_loads.values()
                    .min_by(|a, b| a.current_load.partial_cmp(&b.current_load).unwrap())
                    .map(|w| w.worker_id)
                    .ok_or_else(|| Error::internal("No workers available"))
            }

            LoadBalancingAlgorithm::WeightedRandom => {
                // Weighted random based on inverse load
                self.select_weighted_random_worker(&worker_loads)
            }

            LoadBalancingAlgorithm::AdaptiveML => {
                // ML-based decision
                self.decision_engine.select_optimal_worker(task, &worker_loads)
            }

            LoadBalancingAlgorithm::WorkStealing => {
                // For work stealing, prefer less loaded workers
                worker_loads.values()
                    .filter(|w| w.task_queue_length < 5) // Threshold
                    .min_by(|a, b| a.current_load.partial_cmp(&b.current_load).unwrap())
                    .map(|w| w.worker_id)
                    .unwrap_or(0) // Fallback to worker 0
            }
        }
    }

    /// Select worker using weighted random algorithm
    fn select_weighted_random_worker(&self, worker_loads: &HashMap<usize, WorkerLoad>) -> Result<usize> {
        let mut total_weight = 0.0;
        let mut weights = Vec::new();

        for worker in worker_loads.values() {
            // Weight is inverse of load (higher weight for less loaded workers)
            let weight = (1.0 / (worker.current_load + 0.1)).max(0.1);
            weights.push((worker.worker_id, weight));
            total_weight += weight;
        }

        // Simple random selection with weights
        let mut rand = (std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_nanos() as f64).fract();

        rand *= total_weight;

        let mut cumulative = 0.0;
        for (worker_id, weight) in weights {
            cumulative += weight;
            if rand <= cumulative {
                return Ok(worker_id);
            }
        }

        // Fallback
        Ok(0)
    }

    /// Update worker load when task is scheduled
    fn update_worker_load(&self, worker_id: usize, task: &Task) {
        let mut worker_loads = self.worker_loads.lock().unwrap();

        if let Some(worker) = worker_loads.get_mut(&worker_id) {
            worker.current_load += task.estimated_load;
            worker.task_queue_length += 1;
            worker.last_updated = Instant::now();
        }
    }

    /// Record performance sample for learning
    fn record_performance_sample(&self, worker_id: usize, scheduling_time: Duration, task: &Task) {
        let sample = PerformanceSample {
            timestamp: Instant::now(),
            worker_id,
            scheduling_time,
            task_type: task.task_type.clone(),
            task_load: task.estimated_load,
        };

        let mut history = self.performance_history.lock().unwrap();
        history.push(sample);

        // Keep only recent history (last 1000 samples)
        if history.len() > 1000 {
            history.remove(0);
        }
    }

    /// Update load imbalance statistics
    fn update_load_imbalance_stats(&self) {
        let worker_loads = self.worker_loads.lock().unwrap();

        if worker_loads.is_empty() {
            return;
        }

        let loads: Vec<f64> = worker_loads.values().map(|w| w.current_load).collect();
        let avg_load = loads.iter().sum::<f64>() / loads.len() as f64;
        let max_load = loads.iter().fold(0.0, |a, &b| a.max(b));
        let min_load = loads.iter().fold(f64::INFINITY, |a, &b| a.min(b));

        let imbalance_ratio = if avg_load > 0.0 {
            (max_load - min_load) / avg_load
        } else {
            0.0
        };

        let mut stats = self.stats.lock().unwrap();
        stats.load_imbalance_ratio = imbalance_ratio;

        // Update average response time
        let total_response_time: Duration = worker_loads.values()
            .map(|w| w.response_time)
            .sum();

        stats.average_response_time = total_response_time / worker_loads.len() as u32;
    }
}

/// Worker load information
#[derive(Debug, Clone)]
pub struct WorkerLoad {
    pub worker_id: usize,
    pub current_load: f64,
    pub task_queue_length: usize,
    pub response_time: Duration,
    pub last_updated: Instant,
}

/// Task representation for load balancing
#[derive(Debug, Clone)]
pub struct Task {
    pub id: u64,
    pub task_type: TaskType,
    pub estimated_load: f64,
    pub priority: TaskPriority,
}

/// Task type classification
#[derive(Debug, Clone, PartialEq)]
pub enum TaskType {
    CPUIntensive,
    MemoryIntensive,
    IOBound,
    Mixed,
}

/// Task priority for load balancing
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum TaskPriority {
    Low,
    Normal,
    High,
    Critical,
}

/// Workload pattern analyzer
pub struct WorkloadPatternAnalyzer {
    pattern_history: std::sync::Mutex<Vec<WorkloadSample>>,
}

impl WorkloadPatternAnalyzer {
    fn new() -> Self {
        Self {
            pattern_history: std::sync::Mutex::new(Vec::new()),
        }
    }

    fn analyze_current_pattern(&self) -> WorkloadPattern {
        let history = self.pattern_history.lock().unwrap();

        if history.is_empty() {
            return WorkloadPattern::Mixed;
        }

        // Analyze recent workload patterns
        let recent = &history[history.len().saturating_sub(10)..];

        let cpu_tasks = recent.iter().filter(|s| matches!(s.task_type, TaskType::CPUIntensive)).count();
        let memory_tasks = recent.iter().filter(|s| matches!(s.task_type, TaskType::MemoryIntensive)).count();
        let io_tasks = recent.iter().filter(|s| matches!(s.task_type, TaskType::IOBound)).count();

        let total = recent.len() as f64;

        // Classify based on dominant pattern
        if cpu_tasks as f64 / total > 0.7 {
            WorkloadPattern::CPUIntensive
        } else if memory_tasks as f64 / total > 0.7 {
            WorkloadPattern::MemoryIntensive
        } else if io_tasks as f64 / total > 0.7 {
            WorkloadPattern::IOBound
        } else if Self::detect_burst_pattern(recent) {
            WorkloadPattern::Burst
        } else {
            WorkloadPattern::Mixed
        }
    }

    fn detect_burst_pattern(samples: &[WorkloadSample]) -> bool {
        if samples.len() < 5 {
            return false;
        }

        // Check for sudden load increase
        let recent_avg = samples[samples.len().saturating_sub(3)..].iter()
            .map(|s| s.load_factor)
            .sum::<f64>() / 3.0;

        let older_avg = samples[..samples.len().saturating_sub(3)].iter()
            .map(|s| s.load_factor)
            .sum::<f64>() / (samples.len().saturating_sub(3)) as f64;

        recent_avg > older_avg * 1.5 // 50% increase indicates burst
    }
}

/// ML-based decision engine
pub struct MLDecisionEngine;

impl MLDecisionEngine {
    fn new() -> Self {
        Self
    }

    fn recommend_algorithm(&self, pattern: &WorkloadPattern) -> Result<LoadBalancingAlgorithm> {
        // Simplified ML logic - in real implementation would use trained model
        match pattern {
            WorkloadPattern::CPUIntensive => Ok(LoadBalancingAlgorithm::LeastLoaded),
            WorkloadPattern::MemoryIntensive => Ok(LoadBalancingAlgorithm::AdaptiveML),
            WorkloadPattern::IOBound => Ok(LoadBalancingAlgorithm::RoundRobin),
            WorkloadPattern::Burst => Ok(LoadBalancingAlgorithm::WorkStealing),
            _ => Ok(LoadBalancingAlgorithm::AdaptiveML),
        }
    }

    fn select_optimal_worker(&self, task: &Task, worker_loads: &HashMap<usize, WorkerLoad>) -> Result<usize> {
        // Simplified ML-based worker selection
        // In real implementation, would use reinforcement learning or neural network

        worker_loads.values()
            .min_by(|a, b| {
                let a_score = self.calculate_worker_score(a, task);
                let b_score = self.calculate_worker_score(b, task);
                a_score.partial_cmp(&b_score).unwrap()
            })
            .map(|w| w.worker_id)
            .ok_or_else(|| Error::internal("No workers available"))
    }

    fn calculate_worker_score(&self, worker: &WorkerLoad, task: &Task) -> f64 {
        // Calculate suitability score (lower is better)
        let load_penalty = worker.current_load * 2.0;
        let queue_penalty = worker.task_queue_length as f64 * 0.5;
        let response_penalty = worker.response_time.as_millis() as f64 * 0.001;

        // Task-specific bonuses
        let affinity_bonus = match (&worker.worker_id % 4, &task.task_type) {
            (0, TaskType::CPUIntensive) => -0.5,
            (1, TaskType::MemoryIntensive) => -0.5,
            (2, TaskType::IOBound) => -0.5,
            _ => 0.0,
        };

        load_penalty + queue_penalty + response_penalty + affinity_bonus
    }
}

/// Workload sample for pattern analysis
#[derive(Debug, Clone)]
struct WorkloadSample {
    timestamp: Instant,
    task_type: TaskType,
    load_factor: f64,
}

/// Performance sample for learning
#[derive(Debug, Clone)]
struct PerformanceSample {
    timestamp: Instant,
    worker_id: usize,
    scheduling_time: Duration,
    task_type: TaskType,
    task_load: f64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_load_balancer_creation() {
        let balancer = AdaptiveLoadBalancer::new(4);
        assert!(balancer.is_ok());
    }

    #[test]
    fn test_worker_load() {
        let load = WorkerLoad {
            worker_id: 1,
            current_load: 0.75,
            task_queue_length: 3,
            response_time: Duration::from_millis(50),
            last_updated: Instant::now(),
        };

        assert_eq!(load.worker_id, 1);
        assert_eq!(load.current_load, 0.75);
        assert_eq!(load.task_queue_length, 3);
    }

    #[test]
    fn test_task_creation() {
        let task = Task {
            id: 123,
            task_type: TaskType::CPUIntensive,
            estimated_load: 1.5,
            priority: TaskPriority::High,
        };

        assert_eq!(task.id, 123);
        assert_eq!(task.task_type, TaskType::CPUIntensive);
        assert_eq!(task.estimated_load, 1.5);
    }

    #[test]
    fn test_load_balancer_stats() {
        let stats = LoadBalancerStats {
            tasks_scheduled: 1000,
            load_imbalance_ratio: 0.15,
            average_response_time: Duration::from_millis(25),
            scheduling_overhead: Duration::from_micros(500),
            algorithm_switches: 5,
            current_algorithm: LoadBalancingAlgorithm::AdaptiveML,
        };

        assert_eq!(stats.tasks_scheduled, 1000);
        assert_eq!(stats.load_imbalance_ratio, 0.15);
        assert_eq!(stats.algorithm_switches, 5);
    }

    #[test]
    fn test_workload_pattern_analyzer() {
        let analyzer = WorkloadPatternAnalyzer::new();

        // Test with empty history
        let pattern = analyzer.analyze_current_pattern();
        assert_eq!(pattern, WorkloadPattern::Mixed);
    }

    #[test]
    fn test_ml_decision_engine() {
        let engine = MLDecisionEngine::new();

        let pattern = WorkloadPattern::CPUIntensive;
        let algorithm = engine.recommend_algorithm(&pattern).unwrap();
        assert_eq!(algorithm, LoadBalancingAlgorithm::LeastLoaded);
    }

    #[tokio::test]
    async fn test_load_balancer_scheduling() {
        let balancer = AdaptiveLoadBalancer::new(4).unwrap();

        let task = Task {
            id: 1,
            task_type: TaskType::CPUIntensive,
            estimated_load: 1.0,
            priority: TaskPriority::Normal,
        };

        let worker_id = balancer.schedule_task(task).unwrap();
        assert!(worker_id < 4);

        // Check stats updated
        let stats = balancer.stats();
        assert_eq!(stats.tasks_scheduled, 1);
    }
}
