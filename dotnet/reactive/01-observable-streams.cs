using System;
using System.Collections.Generic;
using System.Reactive;
using System.Reactive.Concurrency;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Reactive.ObservableStreams
{
    /// <summary>
    /// Production-grade reactive programming patterns using Rx.NET.
    /// Used by Bloomberg for real-time market data, PayPal for payment streams,
    /// and Google for real-time analytics processing.
    /// </summary>
    public class ObservableStreamPatterns
    {
        /// <summary>
        /// Bloomberg-style real-time market data stream processor
        /// Handles high-frequency market data with backpressure control
        /// </summary>
        public class MarketDataStreamProcessor : IDisposable
        {
            private readonly Subject<MarketData> _marketDataSubject;
            private readonly Subject<MarketData> _filteredDataSubject;
            private readonly CompositeDisposable _disposables;
            private readonly IScheduler _scheduler;

            public IObservable<MarketData> MarketDataStream => _filteredDataSubject.AsObservable();
            public IObservable<MarketDataSummary> SummaryStream { get; private set; }

            public MarketDataStreamProcessor(IScheduler scheduler = null)
            {
                _scheduler = scheduler ?? Scheduler.Default;
                _marketDataSubject = new Subject<MarketData>();
                _filteredDataSubject = new Subject<MarketData>();
                _disposables = new CompositeDisposable();

                SetupDataProcessing();
            }

            public void OnMarketData(MarketData data)
            {
                _marketDataSubject.OnNext(data);
            }

            public void OnError(Exception error)
            {
                _marketDataSubject.OnError(error);
            }

            public void OnCompleted()
            {
                _marketDataSubject.OnCompleted();
            }

            private void SetupDataProcessing()
            {
                // Filter valid market data
                var validDataStream = _marketDataSubject
                    .Where(data => data.Price > 0 && data.Volume >= 0)
                    .ObserveOn(_scheduler);

                // Subscribe to filtered data
                validDataStream.Subscribe(_filteredDataSubject);

                // Create summary stream with windowing
                SummaryStream = validDataStream
                    .Buffer(TimeSpan.FromSeconds(5), _scheduler)
                    .Where(buffer => buffer.Count > 0)
                    .Select(buffer => new MarketDataSummary
                    {
                        Symbol = buffer[0].Symbol,
                        Count = buffer.Count,
                        AveragePrice = buffer.Average(d => d.Price),
                        MinPrice = buffer.Min(d => d.Price),
                        MaxPrice = buffer.Max(d => d.Price),
                        TotalVolume = buffer.Sum(d => d.Volume),
                        WindowStart = buffer.Min(d => d.Timestamp),
                        WindowEnd = buffer.Max(d => d.Timestamp)
                    })
                    .Publish()
                    .RefCount();

                _disposables.Add(SummaryStream.Subscribe());
            }

            public void Dispose()
            {
                _disposables?.Dispose();
                _marketDataSubject?.Dispose();
                _filteredDataSubject?.Dispose();
            }
        }

        /// <summary>
        /// PayPal-style payment processing stream with fraud detection
        /// Processes payment events with real-time fraud analysis
        /// </summary>
        public class PaymentStreamProcessor : IDisposable
        {
            private readonly Subject<PaymentEvent> _paymentSubject;
            private readonly Subject<FraudAlert> _fraudAlertSubject;
            private readonly CompositeDisposable _disposables;
            private readonly IScheduler _scheduler;

            public IObservable<PaymentEvent> PaymentStream => _paymentSubject.AsObservable();
            public IObservable<FraudAlert> FraudAlerts => _fraudAlertSubject.AsObservable();
            public IObservable<PaymentSummary> PaymentSummary => _paymentSubject
                .Buffer(TimeSpan.FromMinutes(1), _scheduler)
                .Where(buffer => buffer.Count > 0)
                .Select(buffer => new PaymentSummary
                {
                    TotalTransactions = buffer.Count,
                    TotalAmount = buffer.Sum(p => p.Amount),
                    SuccessfulTransactions = buffer.Count(p => p.Status == PaymentStatus.Success),
                    FailedTransactions = buffer.Count(p => p.Status == PaymentStatus.Failed),
                    FraudAlerts = buffer.Count(p => p.IsFraudulent),
                    TimeWindow = new TimeWindow
                    {
                        Start = buffer.Min(p => p.Timestamp),
                        End = buffer.Max(p => p.Timestamp)
                    }
                });

            public PaymentStreamProcessor(IScheduler scheduler = null)
            {
                _scheduler = scheduler ?? Scheduler.Default;
                _paymentSubject = new Subject<PaymentEvent>();
                _fraudAlertSubject = new Subject<FraudAlert>();
                _disposables = new CompositeDisposable();

                SetupFraudDetection();
            }

            public void ProcessPayment(PaymentEvent payment)
            {
                _paymentSubject.OnNext(payment);
            }

            private void SetupFraudDetection()
            {
                // High-value transaction monitoring
                var highValueStream = _paymentSubject
                    .Where(p => p.Amount > 10000)
                    .ObserveOn(_scheduler);

                // Velocity checking (multiple transactions in short time)
                var velocityStream = _paymentSubject
                    .GroupBy(p => p.CustomerId)
                    .SelectMany(group => group
                        .Buffer(TimeSpan.FromMinutes(5), _scheduler)
                        .Where(buffer => buffer.Count > 10)
                        .SelectMany(buffer => buffer));

                // Geographic anomaly detection
                var geoAnomalyStream = _paymentSubject
                    .GroupBy(p => p.CustomerId)
                    .SelectMany(group => group
                        .Buffer(2, 1)
                        .Where(buffer => buffer.Count == 2)
                        .Where(buffer => CalculateDistance(buffer[0].Location, buffer[1].Location) > 1000) // 1000km
                        .SelectMany(buffer => buffer));

                // Combine all fraud indicators
                var fraudStream = highValueStream
                    .Merge(velocityStream)
                    .Merge(geoAnomalyStream)
                    .Distinct(p => p.TransactionId)
                    .Select(payment => new FraudAlert
                    {
                        TransactionId = payment.TransactionId,
                        CustomerId = payment.CustomerId,
                        Amount = payment.Amount,
                        Reason = GetFraudReason(payment),
                        Timestamp = DateTime.UtcNow,
                        Severity = CalculateSeverity(payment)
                    });

                _disposables.Add(fraudStream.Subscribe(_fraudAlertSubject));
            }

            private string GetFraudReason(PaymentEvent payment)
            {
                if (payment.Amount > 10000) return "High value transaction";
                if (payment.Amount > 5000) return "Velocity anomaly";
                return "Geographic anomaly";
            }

            private FraudSeverity CalculateSeverity(PaymentEvent payment)
            {
                if (payment.Amount > 50000) return FraudSeverity.Critical;
                if (payment.Amount > 10000) return FraudSeverity.High;
                return FraudSeverity.Medium;
            }

            private double CalculateDistance(Location loc1, Location loc2)
            {
                // Simplified distance calculation
                return Math.Sqrt(Math.Pow(loc1.Latitude - loc2.Latitude, 2) + Math.Pow(loc1.Longitude - loc2.Longitude, 2));
            }

            public void Dispose()
            {
                _disposables?.Dispose();
                _paymentSubject?.Dispose();
                _fraudAlertSubject?.Dispose();
            }
        }

        /// <summary>
        /// Google-style search analytics stream processor
        /// Processes search queries and generates real-time analytics
        /// </summary>
        public class SearchAnalyticsProcessor : IDisposable
        {
            private readonly Subject<SearchQuery> _searchSubject;
            private readonly Subject<TrendingTopic> _trendingSubject;
            private readonly CompositeDisposable _disposables;
            private readonly IScheduler _scheduler;

            public IObservable<SearchQuery> SearchStream => _searchSubject.AsObservable();
            public IObservable<TrendingTopic> TrendingTopics => _trendingSubject.AsObservable();
            public IObservable<SearchMetrics> SearchMetrics => _searchSubject
                .Buffer(TimeSpan.FromMinutes(1), _scheduler)
                .Where(buffer => buffer.Count > 0)
                .Select(buffer => new SearchMetrics
                {
                    TotalQueries = buffer.Count,
                    UniqueUsers = buffer.Select(q => q.UserId).Distinct().Count(),
                    AverageResponseTime = buffer.Average(q => q.ResponseTime),
                    TopQueries = buffer
                        .GroupBy(q => q.Query)
                        .OrderByDescending(g => g.Count())
                        .Take(10)
                        .Select(g => new QueryCount { Query = g.Key, Count = g.Count() })
                        .ToList(),
                    TimeWindow = new TimeWindow
                    {
                        Start = buffer.Min(q => q.Timestamp),
                        End = buffer.Max(q => q.Timestamp)
                    }
                });

            public SearchAnalyticsProcessor(IScheduler scheduler = null)
            {
                _scheduler = scheduler ?? Scheduler.Default;
                _searchSubject = new Subject<SearchQuery>();
                _trendingSubject = new Subject<TrendingTopic>();
                _disposables = new CompositeDisposable();

                SetupTrendingAnalysis();
            }

            public void ProcessSearch(SearchQuery query)
            {
                _searchSubject.OnNext(query);
            }

            private void SetupTrendingAnalysis()
            {
                // Analyze trending topics by query frequency
                var trendingStream = _searchSubject
                    .GroupBy(q => q.Query)
                    .SelectMany(group => group
                        .Buffer(TimeSpan.FromMinutes(5), _scheduler)
                        .Where(buffer => buffer.Count > 0)
                        .Select(buffer => new
                        {
                            Query = group.Key,
                            Count = buffer.Count,
                            Timestamp = buffer.Max(q => q.Timestamp)
                        }))
                    .GroupBy(t => t.Query)
                    .SelectMany(group => group
                        .Scan((prev, current) => new
                        {
                            Query = current.Query,
                            Count = current.Count,
                            Timestamp = current.Timestamp,
                            GrowthRate = prev != null ? (current.Count - prev.Count) / (double)prev.Count : 0
                        })
                        .Where(trend => trend.GrowthRate > 0.5) // 50% growth
                        .Select(trend => new TrendingTopic
                        {
                            Query = trend.Query,
                            Count = trend.Count,
                            GrowthRate = trend.GrowthRate,
                            Timestamp = trend.Timestamp
                        }));

                _disposables.Add(trendingStream.Subscribe(_trendingSubject));
            }

            public void Dispose()
            {
                _disposables?.Dispose();
                _searchSubject?.Dispose();
                _trendingSubject?.Dispose();
            }
        }

        /// <summary>
        /// Uber-style real-time location tracking with reactive streams
        /// Tracks driver locations and calculates optimal matches
        /// </summary>
        public class LocationTrackingProcessor : IDisposable
        {
            private readonly Subject<LocationUpdate> _locationSubject;
            private readonly Subject<MatchSuggestion> _matchSubject;
            private readonly CompositeDisposable _disposables;
            private readonly IScheduler _scheduler;

            public IObservable<LocationUpdate> LocationStream => _locationSubject.AsObservable();
            public IObservable<MatchSuggestion> MatchSuggestions => _matchSubject.AsObservable();

            public LocationTrackingProcessor(IScheduler scheduler = null)
            {
                _scheduler = scheduler ?? Scheduler.Default;
                _locationSubject = new Subject<LocationUpdate>();
                _matchSubject = new Subject<MatchSuggestion>();
                _disposables = new CompositeDisposable();

                SetupMatching();
            }

            public void UpdateLocation(LocationUpdate update)
            {
                _locationSubject.OnNext(update);
            }

            private void SetupMatching()
            {
                // Group by driver type (passenger vs driver)
                var driverLocations = _locationSubject
                    .Where(update => update.Type == LocationType.Driver)
                    .GroupBy(update => update.Id);

                var passengerLocations = _locationSubject
                    .Where(update => update.Type == LocationType.Passenger)
                    .GroupBy(update => update.Id);

                // Create match suggestions when passenger requests a ride
                var matchStream = passengerLocations
                    .SelectMany(passengerGroup => passengerGroup
                        .Where(update => update.IsRequestingRide)
                        .SelectMany(passengerUpdate =>
                            driverLocations
                                .SelectMany(driverGroup => driverGroup
                                    .Where(driverUpdate => driverUpdate.IsAvailable)
                                    .Select(driverUpdate => new
                                    {
                                        Passenger = passengerUpdate,
                                        Driver = driverUpdate,
                                        Distance = CalculateDistance(passengerUpdate.Location, driverUpdate.Location)
                                    }))
                                .Where(match => match.Distance <= 5.0) // Within 5km
                                .OrderBy(match => match.Distance)
                                .Take(3)
                                .Select(match => new MatchSuggestion
                                {
                                    PassengerId = match.Passenger.Id,
                                    DriverId = match.Driver.Id,
                                    Distance = match.Distance,
                                    EstimatedArrival = TimeSpan.FromMinutes(match.Distance * 2), // 2 min per km
                                    Timestamp = DateTime.UtcNow
                                })));

                _disposables.Add(matchStream.Subscribe(_matchSubject));
            }

            private double CalculateDistance(Location loc1, Location loc2)
            {
                // Simplified distance calculation
                return Math.Sqrt(Math.Pow(loc1.Latitude - loc2.Latitude, 2) + Math.Pow(loc1.Longitude - loc2.Longitude, 2));
            }

            public void Dispose()
            {
                _disposables?.Dispose();
                _locationSubject?.Dispose();
                _matchSubject?.Dispose();
            }
        }

        /// <summary>
        /// Amazon-style recommendation engine with reactive processing
        /// Generates real-time recommendations based on user behavior
        /// </summary>
        public class RecommendationEngine : IDisposable
        {
            private readonly Subject<UserBehavior> _behaviorSubject;
            private readonly Subject<Recommendation> _recommendationSubject;
            private readonly CompositeDisposable _disposables;
            private readonly IScheduler _scheduler;

            public IObservable<UserBehavior> BehaviorStream => _behaviorSubject.AsObservable();
            public IObservable<Recommendation> Recommendations => _recommendationSubject.AsObservable();

            public RecommendationEngine(IScheduler scheduler = null)
            {
                _scheduler = scheduler ?? Scheduler.Default;
                _behaviorSubject = new Subject<UserBehavior>();
                _recommendationSubject = new Subject<Recommendation>();
                _disposables = new CompositeDisposable();

                SetupRecommendationGeneration();
            }

            public void TrackBehavior(UserBehavior behavior)
            {
                _behaviorSubject.OnNext(behavior);
            }

            private void SetupRecommendationGeneration()
            {
                // Generate recommendations based on user behavior patterns
                var recommendationStream = _behaviorSubject
                    .GroupBy(behavior => behavior.UserId)
                    .SelectMany(userGroup => userGroup
                        .Buffer(TimeSpan.FromMinutes(10), _scheduler)
                        .Where(buffer => buffer.Count > 0)
                        .SelectMany(behaviors => GenerateRecommendationsForUser(behaviors)));

                _disposables.Add(recommendationStream.Subscribe(_recommendationSubject));
            }

            private IEnumerable<Recommendation> GenerateRecommendationsForUser(List<UserBehavior> behaviors)
            {
                // Simplified recommendation logic
                var recentProducts = behaviors
                    .Where(b => b.Type == BehaviorType.View || b.Type == BehaviorType.Purchase)
                    .Select(b => b.ProductId)
                    .Distinct()
                    .ToList();

                if (recentProducts.Any())
                {
                    // Generate recommendations based on viewed/purchased products
                    foreach (var productId in recentProducts.Take(5))
                    {
                        yield return new Recommendation
                        {
                            UserId = behaviors[0].UserId,
                            ProductId = productId,
                            Score = Random.Shared.NextDouble(),
                            Reason = "Based on your recent activity",
                            GeneratedAt = DateTime.UtcNow
                        };
                    }
                }
            }

            public void Dispose()
            {
                _disposables?.Dispose();
                _behaviorSubject?.Dispose();
                _recommendationSubject?.Dispose();
            }
        }
    }

    #region Supporting Classes

    public class MarketData
    {
        public string Symbol { get; set; }
        public decimal Price { get; set; }
        public decimal Volume { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class MarketDataSummary
    {
        public string Symbol { get; set; }
        public int Count { get; set; }
        public decimal AveragePrice { get; set; }
        public decimal MinPrice { get; set; }
        public decimal MaxPrice { get; set; }
        public decimal TotalVolume { get; set; }
        public DateTime WindowStart { get; set; }
        public DateTime WindowEnd { get; set; }
    }

    public class PaymentEvent
    {
        public string TransactionId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Currency { get; set; }
        public PaymentStatus Status { get; set; }
        public Location Location { get; set; }
        public DateTime Timestamp { get; set; }
        public bool IsFraudulent { get; set; }
    }

    public enum PaymentStatus
    {
        Pending,
        Success,
        Failed,
        Cancelled
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    public class FraudAlert
    {
        public string TransactionId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Reason { get; set; }
        public DateTime Timestamp { get; set; }
        public FraudSeverity Severity { get; set; }
    }

    public enum FraudSeverity
    {
        Low,
        Medium,
        High,
        Critical
    }

    public class PaymentSummary
    {
        public int TotalTransactions { get; set; }
        public decimal TotalAmount { get; set; }
        public int SuccessfulTransactions { get; set; }
        public int FailedTransactions { get; set; }
        public int FraudAlerts { get; set; }
        public TimeWindow TimeWindow { get; set; }
    }

    public class TimeWindow
    {
        public DateTime Start { get; set; }
        public DateTime End { get; set; }
    }

    public class SearchQuery
    {
        public string Query { get; set; }
        public string UserId { get; set; }
        public TimeSpan ResponseTime { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class TrendingTopic
    {
        public string Query { get; set; }
        public int Count { get; set; }
        public double GrowthRate { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class SearchMetrics
    {
        public int TotalQueries { get; set; }
        public int UniqueUsers { get; set; }
        public double AverageResponseTime { get; set; }
        public List<QueryCount> TopQueries { get; set; } = new();
        public TimeWindow TimeWindow { get; set; }
    }

    public class QueryCount
    {
        public string Query { get; set; }
        public int Count { get; set; }
    }

    public class LocationUpdate
    {
        public string Id { get; set; }
        public LocationType Type { get; set; }
        public Location Location { get; set; }
        public bool IsAvailable { get; set; }
        public bool IsRequestingRide { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum LocationType
    {
        Driver,
        Passenger
    }

    public class MatchSuggestion
    {
        public string PassengerId { get; set; }
        public string DriverId { get; set; }
        public double Distance { get; set; }
        public TimeSpan EstimatedArrival { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class UserBehavior
    {
        public string UserId { get; set; }
        public string ProductId { get; set; }
        public BehaviorType Type { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum BehaviorType
    {
        View,
        AddToCart,
        Purchase,
        Search
    }

    public class Recommendation
    {
        public string UserId { get; set; }
        public string ProductId { get; set; }
        public double Score { get; set; }
        public string Reason { get; set; }
        public DateTime GeneratedAt { get; set; }
    }

    #endregion
}
