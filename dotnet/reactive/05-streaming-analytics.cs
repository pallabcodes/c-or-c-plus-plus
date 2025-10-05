using System;
using System.Collections.Generic;
using System.Linq;
using System.Reactive;
using System.Reactive.Concurrency;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Reactive.StreamingAnalytics
{
    /// <summary>
    /// Advanced streaming analytics patterns used by top-tier companies.
    /// Covers real-time analytics, windowing, aggregation, and machine learning pipelines.
    /// </summary>
    public class StreamingAnalyticsPatterns
    {
        /// <summary>
        /// Google-style real-time search analytics
        /// Processes search queries and generates real-time insights
        /// </summary>
        public class SearchAnalyticsEngine
        {
            private readonly Subject<SearchQuery> _searchQueries;
            private readonly Subject<SearchAnalytics> _analytics;

            public SearchAnalyticsEngine()
            {
                _searchQueries = new Subject<SearchQuery>();
                _analytics = new Subject<SearchAnalytics>();
            }

            public IObservable<SearchQuery> SearchQueries => _searchQueries.AsObservable();
            public IObservable<SearchAnalytics> Analytics => _analytics.AsObservable();

            public void ProcessSearchQuery(SearchQuery query)
            {
                _searchQueries.OnNext(query);
            }

            public IObservable<SearchTrends> GetSearchTrends(TimeSpan windowSize)
            {
                return _searchQueries
                    .Buffer(windowSize, TimeSpan.FromSeconds(1))
                    .Where(buffer => buffer.Any())
                    .Select(buffer => AnalyzeSearchTrends(buffer))
                    .Publish()
                    .RefCount();
            }

            public IObservable<PopularQueries> GetPopularQueries(TimeSpan windowSize, int topCount = 10)
            {
                return _searchQueries
                    .Buffer(windowSize, TimeSpan.FromSeconds(5))
                    .Where(buffer => buffer.Any())
                    .Select(buffer => GetTopQueries(buffer, topCount))
                    .Publish()
                    .RefCount();
            }

            private SearchTrends AnalyzeSearchTrends(IList<SearchQuery> queries)
            {
                var trends = new SearchTrends
                {
                    WindowStart = DateTime.UtcNow - TimeSpan.FromMinutes(5),
                    WindowEnd = DateTime.UtcNow,
                    TotalQueries = queries.Count,
                    UniqueQueries = queries.Select(q => q.Query).Distinct().Count(),
                    AverageQueryLength = queries.Average(q => q.Query.Length),
                    TopCategories = queries
                        .GroupBy(q => q.Category)
                        .OrderByDescending(g => g.Count())
                        .Take(5)
                        .Select(g => new CategoryTrend { Category = g.Key, Count = g.Count() })
                        .ToList()
                };

                _analytics.OnNext(new SearchAnalytics
                {
                    Type = "Trends",
                    Data = trends,
                    Timestamp = DateTime.UtcNow
                });

                return trends;
            }

            private PopularQueries GetTopQueries(IList<SearchQuery> queries, int topCount)
            {
                var popular = queries
                    .GroupBy(q => q.Query)
                    .OrderByDescending(g => g.Count())
                    .Take(topCount)
                    .Select(g => new QueryPopularity { Query = g.Key, Count = g.Count() })
                    .ToList();

                var result = new PopularQueries
                {
                    WindowStart = DateTime.UtcNow - TimeSpan.FromMinutes(5),
                    WindowEnd = DateTime.UtcNow,
                    Queries = popular
                };

                _analytics.OnNext(new SearchAnalytics
                {
                    Type = "PopularQueries",
                    Data = result,
                    Timestamp = DateTime.UtcNow
                });

                return result;
            }
        }

        /// <summary>
        /// Bloomberg-style real-time market data analytics
        /// Processes high-frequency market data and generates trading signals
        /// </summary>
        public class MarketDataAnalytics
        {
            private readonly Subject<MarketData> _marketData;
            private readonly Subject<TradingSignal> _tradingSignals;

            public MarketDataAnalytics()
            {
                _marketData = new Subject<MarketData>();
                _tradingSignals = new Subject<TradingSignal>();
            }

            public IObservable<MarketData> MarketData => _marketData.AsObservable();
            public IObservable<TradingSignal> TradingSignals => _tradingSignals.AsObservable();

            public void ProcessMarketData(MarketData data)
            {
                _marketData.OnNext(data);
            }

            public IObservable<PriceAlert> GetPriceAlerts(string symbol, decimal threshold)
            {
                return _marketData
                    .Where(data => data.Symbol == symbol)
                    .Buffer(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(1))
                    .Where(buffer => buffer.Any())
                    .SelectMany(buffer => AnalyzePriceMovements(symbol, buffer, threshold))
                    .Publish()
                    .RefCount();
            }

            public IObservable<VolumeSpike> GetVolumeSpikes(string symbol, decimal volumeThreshold)
            {
                return _marketData
                    .Where(data => data.Symbol == symbol)
                    .Buffer(TimeSpan.FromMinutes(1), TimeSpan.FromSeconds(10))
                    .Where(buffer => buffer.Any())
                    .SelectMany(buffer => AnalyzeVolumeSpikes(symbol, buffer, volumeThreshold))
                    .Publish()
                    .RefCount();
            }

            public IObservable<TechnicalIndicator> GetTechnicalIndicators(string symbol, TechnicalIndicatorType type)
            {
                return _marketData
                    .Where(data => data.Symbol == symbol)
                    .Buffer(TimeSpan.FromMinutes(5), TimeSpan.FromSeconds(30))
                    .Where(buffer => buffer.Any())
                    .Select(buffer => CalculateTechnicalIndicator(symbol, buffer, type))
                    .Where(indicator => indicator != null)
                    .Publish()
                    .RefCount();
            }

            private IEnumerable<PriceAlert> AnalyzePriceMovements(string symbol, IList<MarketData> data, decimal threshold)
            {
                if (data.Count < 2) yield break;

                var currentPrice = data.Last().Price;
                var previousPrice = data[data.Count - 2].Price;
                var priceChange = (currentPrice - previousPrice) / previousPrice;

                if (Math.Abs(priceChange) >= threshold)
                {
                    yield return new PriceAlert
                    {
                        Symbol = symbol,
                        CurrentPrice = currentPrice,
                        PreviousPrice = previousPrice,
                        PriceChange = priceChange,
                        AlertType = priceChange > 0 ? "Price Increase" : "Price Decrease",
                        Timestamp = DateTime.UtcNow
                    };
                }
            }

            private IEnumerable<VolumeSpike> AnalyzeVolumeSpikes(string symbol, IList<MarketData> data, decimal volumeThreshold)
            {
                if (data.Count < 2) yield break;

                var currentVolume = data.Last().Volume;
                var averageVolume = data.Take(data.Count - 1).Average(d => d.Volume);

                if (currentVolume > averageVolume * volumeThreshold)
                {
                    yield return new VolumeSpike
                    {
                        Symbol = symbol,
                        CurrentVolume = currentVolume,
                        AverageVolume = averageVolume,
                        VolumeRatio = currentVolume / averageVolume,
                        Timestamp = DateTime.UtcNow
                    };
                }
            }

            private TechnicalIndicator CalculateTechnicalIndicator(string symbol, IList<MarketData> data, TechnicalIndicatorType type)
            {
                if (data.Count < 20) return null; // Need minimum data points

                return type switch
                {
                    TechnicalIndicatorType.SMA => CalculateSMA(symbol, data, 20),
                    TechnicalIndicatorType.EMA => CalculateEMA(symbol, data, 20),
                    TechnicalIndicatorType.RSI => CalculateRSI(symbol, data, 14),
                    TechnicalIndicatorType.MACD => CalculateMACD(symbol, data),
                    _ => null
                };
            }

            private TechnicalIndicator CalculateSMA(string symbol, IList<MarketData> data, int period)
            {
                var prices = data.TakeLast(period).Select(d => d.Price).ToList();
                var sma = prices.Average();

                return new TechnicalIndicator
                {
                    Symbol = symbol,
                    Type = TechnicalIndicatorType.SMA,
                    Value = sma,
                    Timestamp = DateTime.UtcNow
                };
            }

            private TechnicalIndicator CalculateEMA(string symbol, IList<MarketData> data, int period)
            {
                var prices = data.TakeLast(period).Select(d => d.Price).ToList();
                var multiplier = 2.0m / (period + 1);
                var ema = prices.First();

                foreach (var price in prices.Skip(1))
                {
                    ema = (price * multiplier) + (ema * (1 - multiplier));
                }

                return new TechnicalIndicator
                {
                    Symbol = symbol,
                    Type = TechnicalIndicatorType.EMA,
                    Value = ema,
                    Timestamp = DateTime.UtcNow
                };
            }

            private TechnicalIndicator CalculateRSI(string symbol, IList<MarketData> data, int period)
            {
                var prices = data.TakeLast(period + 1).Select(d => d.Price).ToList();
                var gains = new List<decimal>();
                var losses = new List<decimal>();

                for (int i = 1; i < prices.Count; i++)
                {
                    var change = prices[i] - prices[i - 1];
                    gains.Add(change > 0 ? change : 0);
                    losses.Add(change < 0 ? -change : 0);
                }

                var avgGain = gains.Average();
                var avgLoss = losses.Average();

                if (avgLoss == 0) return null;

                var rs = avgGain / avgLoss;
                var rsi = 100 - (100 / (1 + rs));

                return new TechnicalIndicator
                {
                    Symbol = symbol,
                    Type = TechnicalIndicatorType.RSI,
                    Value = rsi,
                    Timestamp = DateTime.UtcNow
                };
            }

            private TechnicalIndicator CalculateMACD(string symbol, IList<MarketData> data)
            {
                // Simplified MACD calculation
                var prices = data.TakeLast(26).Select(d => d.Price).ToList();
                var ema12 = CalculateEMA(symbol, data.TakeLast(12).ToList(), 12);
                var ema26 = CalculateEMA(symbol, data.TakeLast(26).ToList(), 26);

                if (ema12 == null || ema26 == null) return null;

                var macd = ema12.Value - ema26.Value;

                return new TechnicalIndicator
                {
                    Symbol = symbol,
                    Type = TechnicalIndicatorType.MACD,
                    Value = macd,
                    Timestamp = DateTime.UtcNow
                };
            }
        }

        /// <summary>
        /// PayPal-style fraud detection analytics
        /// Processes transaction data and detects fraudulent patterns
        /// </summary>
        public class FraudDetectionAnalytics
        {
            private readonly Subject<Transaction> _transactions;
            private readonly Subject<FraudAlert> _fraudAlerts;

            public FraudDetectionAnalytics()
            {
                _transactions = new Subject<Transaction>();
                _fraudAlerts = new Subject<FraudAlert>();
            }

            public IObservable<Transaction> Transactions => _transactions.AsObservable();
            public IObservable<FraudAlert> FraudAlerts => _fraudAlerts.AsObservable();

            public void ProcessTransaction(Transaction transaction)
            {
                _transactions.OnNext(transaction);
            }

            public IObservable<FraudScore> GetFraudScores(string customerId)
            {
                return _transactions
                    .Where(t => t.CustomerId == customerId)
                    .Buffer(TimeSpan.FromMinutes(10), TimeSpan.FromMinutes(1))
                    .Where(buffer => buffer.Any())
                    .Select(buffer => CalculateFraudScore(customerId, buffer))
                    .Publish()
                    .RefCount();
            }

            public IObservable<AnomalyDetection> GetAnomalyDetections()
            {
                return _transactions
                    .Buffer(TimeSpan.FromMinutes(5), TimeSpan.FromSeconds(30))
                    .Where(buffer => buffer.Any())
                    .SelectMany(buffer => DetectAnomalies(buffer))
                    .Publish()
                    .RefCount();
            }

            private FraudScore CalculateFraudScore(string customerId, IList<Transaction> transactions)
            {
                var score = 0m;
                var reasons = new List<string>();

                // High value transaction check
                var highValueTransactions = transactions.Where(t => t.Amount > 10000).Count();
                if (highValueTransactions > 0)
                {
                    score += highValueTransactions * 20;
                    reasons.Add($"High value transactions: {highValueTransactions}");
                }

                // High frequency check
                var transactionCount = transactions.Count;
                if (transactionCount > 20)
                {
                    score += (transactionCount - 20) * 5;
                    reasons.Add($"High frequency: {transactionCount} transactions");
                }

                // Unusual time check
                var nightTransactions = transactions.Where(t => t.Timestamp.Hour < 6 || t.Timestamp.Hour > 22).Count();
                if (nightTransactions > 5)
                {
                    score += nightTransactions * 10;
                    reasons.Add($"Unusual time transactions: {nightTransactions}");
                }

                // Geographic anomaly check
                var uniqueLocations = transactions.Select(t => t.Location).Distinct().Count();
                if (uniqueLocations > 3)
                {
                    score += uniqueLocations * 15;
                    reasons.Add($"Multiple locations: {uniqueLocations}");
                }

                var fraudScore = new FraudScore
                {
                    CustomerId = customerId,
                    Score = Math.Min(score, 100), // Cap at 100
                    Reasons = reasons,
                    Timestamp = DateTime.UtcNow
                };

                if (fraudScore.Score > 50)
                {
                    _fraudAlerts.OnNext(new FraudAlert
                    {
                        CustomerId = customerId,
                        Score = fraudScore.Score,
                        Reasons = reasons,
                        Timestamp = DateTime.UtcNow
                    });
                }

                return fraudScore;
            }

            private IEnumerable<AnomalyDetection> DetectAnomalies(IList<Transaction> transactions)
            {
                if (transactions.Count < 10) yield break;

                var amounts = transactions.Select(t => (double)t.Amount).ToList();
                var mean = amounts.Average();
                var stdDev = Math.Sqrt(amounts.Sum(x => Math.Pow(x - mean, 2)) / amounts.Count);

                foreach (var transaction in transactions)
                {
                    var zScore = Math.Abs((double)transaction.Amount - mean) / stdDev;
                    if (zScore > 3) // 3 standard deviations
                    {
                        yield return new AnomalyDetection
                        {
                            TransactionId = transaction.Id,
                            CustomerId = transaction.CustomerId,
                            Amount = transaction.Amount,
                            ZScore = zScore,
                            AnomalyType = "Statistical Outlier",
                            Timestamp = DateTime.UtcNow
                        };
                    }
                }
            }
        }

        /// <summary>
        /// Uber-style real-time demand analytics
        /// Analyzes ride requests and driver availability for dynamic pricing
        /// </summary>
        public class DemandAnalytics
        {
            private readonly Subject<RideRequest> _rideRequests;
            private readonly Subject<DriverLocation> _driverLocations;
            private readonly Subject<DemandMetrics> _demandMetrics;

            public DemandAnalytics()
            {
                _rideRequests = new Subject<RideRequest>();
                _driverLocations = new Subject<DriverLocation>();
                _demandMetrics = new Subject<DemandMetrics>();
            }

            public IObservable<RideRequest> RideRequests => _rideRequests.AsObservable();
            public IObservable<DriverLocation> DriverLocations => _driverLocations.AsObservable();
            public IObservable<DemandMetrics> DemandMetrics => _demandMetrics.AsObservable();

            public void ProcessRideRequest(RideRequest request)
            {
                _rideRequests.OnNext(request);
            }

            public void UpdateDriverLocation(DriverLocation location)
            {
                _driverLocations.OnNext(location);
            }

            public IObservable<DynamicPricing> GetDynamicPricing(string city)
            {
                return Observable.CombineLatest(
                    _rideRequests.Where(r => r.City == city),
                    _driverLocations.Where(d => d.City == city),
                    (requests, drivers) => new { Requests = requests, Drivers = drivers })
                    .Buffer(TimeSpan.FromMinutes(5), TimeSpan.FromSeconds(30))
                    .Where(buffer => buffer.Any())
                    .Select(buffer => CalculateDynamicPricing(city, buffer))
                    .Publish()
                    .RefCount();
            }

            public IObservable<SurgePricing> GetSurgePricing(string city)
            {
                return _rideRequests
                    .Where(r => r.City == city)
                    .Buffer(TimeSpan.FromMinutes(2), TimeSpan.FromSeconds(10))
                    .Where(buffer => buffer.Any())
                    .Select(buffer => CalculateSurgePricing(city, buffer))
                    .Publish()
                    .RefCount();
            }

            private DynamicPricing CalculateDynamicPricing(string city, IList<dynamic> data)
            {
                var requests = data.SelectMany(d => d.Requests).ToList();
                var drivers = data.SelectMany(d => d.Drivers).ToList();

                var demandRatio = requests.Count / Math.Max(drivers.Count, 1);
                var basePrice = 10.0m; // Base price
                var multiplier = Math.Max(1.0m, demandRatio);

                return new DynamicPricing
                {
                    City = city,
                    BasePrice = basePrice,
                    Multiplier = multiplier,
                    FinalPrice = basePrice * multiplier,
                    DemandRatio = demandRatio,
                    RequestCount = requests.Count,
                    DriverCount = drivers.Count,
                    Timestamp = DateTime.UtcNow
                };
            }

            private SurgePricing CalculateSurgePricing(string city, IList<RideRequest> requests)
            {
                var requestCount = requests.Count;
                var surgeMultiplier = 1.0m;

                if (requestCount > 100)
                {
                    surgeMultiplier = 2.0m;
                }
                else if (requestCount > 50)
                {
                    surgeMultiplier = 1.5m;
                }
                else if (requestCount > 20)
                {
                    surgeMultiplier = 1.2m;
                }

                return new SurgePricing
                {
                    City = city,
                    SurgeMultiplier = surgeMultiplier,
                    RequestCount = requestCount,
                    IsSurgeActive = surgeMultiplier > 1.0m,
                    Timestamp = DateTime.UtcNow
                };
            }
        }
    }

    #region Supporting Classes

    public class SearchQuery
    {
        public string Query { get; set; }
        public string Category { get; set; }
        public string UserId { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class SearchAnalytics
    {
        public string Type { get; set; }
        public object Data { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class SearchTrends
    {
        public DateTime WindowStart { get; set; }
        public DateTime WindowEnd { get; set; }
        public int TotalQueries { get; set; }
        public int UniqueQueries { get; set; }
        public double AverageQueryLength { get; set; }
        public List<CategoryTrend> TopCategories { get; set; } = new();
    }

    public class CategoryTrend
    {
        public string Category { get; set; }
        public int Count { get; set; }
    }

    public class PopularQueries
    {
        public DateTime WindowStart { get; set; }
        public DateTime WindowEnd { get; set; }
        public List<QueryPopularity> Queries { get; set; } = new();
    }

    public class QueryPopularity
    {
        public string Query { get; set; }
        public int Count { get; set; }
    }

    public class MarketData
    {
        public string Symbol { get; set; }
        public decimal Price { get; set; }
        public decimal Volume { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class TradingSignal
    {
        public string Symbol { get; set; }
        public string SignalType { get; set; }
        public decimal Price { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class PriceAlert
    {
        public string Symbol { get; set; }
        public decimal CurrentPrice { get; set; }
        public decimal PreviousPrice { get; set; }
        public decimal PriceChange { get; set; }
        public string AlertType { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class VolumeSpike
    {
        public string Symbol { get; set; }
        public decimal CurrentVolume { get; set; }
        public decimal AverageVolume { get; set; }
        public decimal VolumeRatio { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class TechnicalIndicator
    {
        public string Symbol { get; set; }
        public TechnicalIndicatorType Type { get; set; }
        public decimal Value { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum TechnicalIndicatorType
    {
        SMA,    // Simple Moving Average
        EMA,    // Exponential Moving Average
        RSI,    // Relative Strength Index
        MACD    // Moving Average Convergence Divergence
    }

    public class Transaction
    {
        public string Id { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Location { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class FraudScore
    {
        public string CustomerId { get; set; }
        public decimal Score { get; set; }
        public List<string> Reasons { get; set; } = new();
        public DateTime Timestamp { get; set; }
    }

    public class FraudAlert
    {
        public string CustomerId { get; set; }
        public decimal Score { get; set; }
        public List<string> Reasons { get; set; } = new();
        public DateTime Timestamp { get; set; }
    }

    public class AnomalyDetection
    {
        public string TransactionId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public double ZScore { get; set; }
        public string AnomalyType { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class RideRequest
    {
        public string Id { get; set; }
        public string PassengerId { get; set; }
        public string City { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
        public DateTime RequestTime { get; set; }
    }

    public class DriverLocation
    {
        public string DriverId { get; set; }
        public string City { get; set; }
        public Location Location { get; set; }
        public bool IsAvailable { get; set; }
        public DateTime LastUpdate { get; set; }
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    public class DemandMetrics
    {
        public string City { get; set; }
        public int RequestCount { get; set; }
        public int DriverCount { get; set; }
        public decimal DemandRatio { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class DynamicPricing
    {
        public string City { get; set; }
        public decimal BasePrice { get; set; }
        public decimal Multiplier { get; set; }
        public decimal FinalPrice { get; set; }
        public decimal DemandRatio { get; set; }
        public int RequestCount { get; set; }
        public int DriverCount { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class SurgePricing
    {
        public string City { get; set; }
        public decimal SurgeMultiplier { get; set; }
        public int RequestCount { get; set; }
        public bool IsSurgeActive { get; set; }
        public DateTime Timestamp { get; set; }
    }

    #endregion
}
