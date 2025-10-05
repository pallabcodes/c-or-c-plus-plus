using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Parallel.DataParallelism
{
    /// <summary>
    /// Data parallelism patterns for high-performance data processing.
    /// Used by Bloomberg for market data analysis, Google for search indexing,
    /// and PayPal for transaction processing.
    /// </summary>
    public class DataParallelismPatterns
    {
        /// <summary>
        /// Bloomberg-style market data aggregation using PLINQ
        /// Processes millions of market data points in parallel
        /// </summary>
        public static Dictionary<string, MarketDataSummary> AggregateMarketData(
            IEnumerable<MarketData> marketData,
            CancellationToken cancellationToken = default)
        {
            return marketData
                .AsParallel()
                .WithCancellation(cancellationToken)
                .WithDegreeOfParallelism(Environment.ProcessorCount)
                .GroupBy(data => data.Symbol)
                .ToDictionary(
                    group => group.Key,
                    group => new MarketDataSummary
                    {
                        Symbol = group.Key,
                        Count = group.Count(),
                        AveragePrice = group.Average(d => d.Price),
                        MinPrice = group.Min(d => d.Price),
                        MaxPrice = group.Max(d => d.Price),
                        TotalVolume = group.Sum(d => d.Volume),
                        LastUpdate = group.Max(d => d.Timestamp)
                    });
        }

        /// <summary>
        /// Google-style document processing with parallel aggregation
        /// Processes large document collections for search indexing
        /// </summary>
        public static async Task<SearchIndex> BuildSearchIndexAsync(
            IEnumerable<Document> documents,
            CancellationToken cancellationToken = default)
        {
            var index = new ConcurrentDictionary<string, List<int>>();
            var documentList = documents.ToList();
            
            await Parallel.ForEachAsync(
                documentList.Select((doc, index) => new { Document = doc, Index = index }),
                new ParallelOptions
                {
                    CancellationToken = cancellationToken,
                    MaxDegreeOfParallelism = Environment.ProcessorCount
                },
                async (item, ct) =>
                {
                    var tokens = await ProcessDocumentAsync(item.Document, ct);
                    
                    foreach (var token in tokens)
                    {
                        index.AddOrUpdate(
                            token,
                            new List<int> { item.Index },
                            (key, existing) =>
                            {
                                lock (existing)
                                {
                                    existing.Add(item.Index);
                                }
                                return existing;
                            });
                    }
                });

            return new SearchIndex { TokenToDocuments = index.ToDictionary(kvp => kvp.Key, kvp => kvp.Value) };
        }

        /// <summary>
        /// PayPal/Stripe-style transaction processing with parallel validation
        /// Validates thousands of transactions concurrently
        /// </summary>
        public static async Task<List<TransactionResult>> ProcessTransactionsAsync(
            IEnumerable<Transaction> transactions,
            CancellationToken cancellationToken = default)
        {
            var results = new ConcurrentBag<TransactionResult>();
            
            await Parallel.ForEachAsync(
                transactions,
                new ParallelOptions
                {
                    CancellationToken = cancellationToken,
                    MaxDegreeOfParallelism = Math.Min(Environment.ProcessorCount * 2, 16) // Higher concurrency for I/O
                },
                async (transaction, ct) =>
                {
                    try
                    {
                        var result = await ValidateAndProcessTransactionAsync(transaction, ct);
                        results.Add(result);
                    }
                    catch (Exception ex)
                    {
                        results.Add(new TransactionResult
                        {
                            TransactionId = transaction.Id,
                            Status = TransactionStatus.Failed,
                            ErrorMessage = ex.Message
                        });
                    }
                });

            return results.ToList();
        }

        /// <summary>
        /// Uber-style route optimization with parallel calculations
        /// Calculates optimal routes for multiple requests simultaneously
        /// </summary>
        public static async Task<List<RouteOptimization>> OptimizeRoutesAsync(
            IEnumerable<RouteRequest> requests,
            CancellationToken cancellationToken = default)
        {
            var results = new ConcurrentBag<RouteOptimization>();
            
            await Parallel.ForEachAsync(
                requests,
                new ParallelOptions
                {
                    CancellationToken = cancellationToken,
                    MaxDegreeOfParallelism = Environment.ProcessorCount
                },
                async (request, ct) =>
                {
                    var optimization = await CalculateOptimalRouteAsync(request, ct);
                    results.Add(optimization);
                });

            return results.ToList();
        }

        /// <summary>
        /// Amazon-style recommendation calculation with parallel processing
        /// Generates recommendations for millions of users
        /// </summary>
        public static async Task<Dictionary<string, List<Recommendation>>> GenerateBulkRecommendationsAsync(
            IEnumerable<User> users,
            IEnumerable<Product> products,
            CancellationToken cancellationToken = default)
        {
            var recommendations = new ConcurrentDictionary<string, List<Recommendation>>();
            var productList = products.ToList();
            
            await Parallel.ForEachAsync(
                users,
                new ParallelOptions
                {
                    CancellationToken = cancellationToken,
                    MaxDegreeOfParallelism = Environment.ProcessorCount
                },
                async (user, ct) =>
                {
                    var userRecommendations = await CalculateUserRecommendationsAsync(user, productList, ct);
                    recommendations.TryAdd(user.Id, userRecommendations);
                });

            return recommendations.ToDictionary(kvp => kvp.Key, kvp => kvp.Value);
        }

        /// <summary>
        /// High-performance data transformation using parallel processing
        /// Used for ETL operations in enterprise systems
        /// </summary>
        public static async Task<List<TransformedData>> TransformDataAsync<T>(
            IEnumerable<T> sourceData,
            Func<T, Task<TransformedData>> transformation,
            CancellationToken cancellationToken = default)
        {
            var results = new ConcurrentBag<TransformedData>();
            
            await Parallel.ForEachAsync(
                sourceData,
                new ParallelOptions
                {
                    CancellationToken = cancellationToken,
                    MaxDegreeOfParallelism = Environment.ProcessorCount
                },
                async (item, ct) =>
                {
                    try
                    {
                        var transformed = await transformation(item);
                        results.Add(transformed);
                    }
                    catch (Exception ex)
                    {
                        // Log error but continue processing
                        Console.WriteLine($"Error transforming data: {ex.Message}");
                    }
                });

            return results.ToList();
        }

        /// <summary>
        /// Parallel data validation with custom validation rules
        /// Used for data quality assurance in enterprise systems
        /// </summary>
        public static async Task<ValidationResult> ValidateDataAsync<T>(
            IEnumerable<T> data,
            IEnumerable<Func<T, Task<ValidationError>>> validators,
            CancellationToken cancellationToken = default)
        {
            var errors = new ConcurrentBag<ValidationError>();
            var validatorList = validators.ToList();
            
            await Parallel.ForEachAsync(
                data,
                new ParallelOptions
                {
                    CancellationToken = cancellationToken,
                    MaxDegreeOfParallelism = Environment.ProcessorCount
                },
                async (item, ct) =>
                {
                    foreach (var validator in validatorList)
                    {
                        try
                        {
                            var error = await validator(item);
                            if (error != null)
                                errors.Add(error);
                        }
                        catch (Exception ex)
                        {
                            errors.Add(new ValidationError
                            {
                                Item = item?.ToString(),
                                Rule = validator.Method.Name,
                                Message = ex.Message
                            });
                        }
                    }
                });

            return new ValidationResult
            {
                IsValid = !errors.Any(),
                Errors = errors.ToList(),
                TotalItems = data.Count(),
                ErrorCount = errors.Count
            };
        }

        #region Helper Methods

        private static async Task<List<string>> ProcessDocumentAsync(Document document, CancellationToken cancellationToken)
        {
            await Task.Delay(10, cancellationToken); // Simulate processing
            return document.Content
                .Split(' ', StringSplitOptions.RemoveEmptyEntries)
                .Select(w => w.ToLowerInvariant().Trim('.,!?;:'))
                .Where(w => w.Length > 2)
                .ToList();
        }

        private static async Task<TransactionResult> ValidateAndProcessTransactionAsync(
            Transaction transaction, 
            CancellationToken cancellationToken)
        {
            await Task.Delay(50, cancellationToken); // Simulate validation and processing
            
            // Simulate validation logic
            if (transaction.Amount <= 0)
                throw new InvalidOperationException("Invalid transaction amount");
            
            if (string.IsNullOrEmpty(transaction.CustomerId))
                throw new InvalidOperationException("Missing customer ID");

            return new TransactionResult
            {
                TransactionId = transaction.Id,
                Status = TransactionStatus.Success,
                ProcessedAt = DateTime.UtcNow
            };
        }

        private static async Task<RouteOptimization> CalculateOptimalRouteAsync(
            RouteRequest request, 
            CancellationToken cancellationToken)
        {
            await Task.Delay(100, cancellationToken); // Simulate route calculation
            
            return new RouteOptimization
            {
                RequestId = request.Id,
                OptimalRoute = new List<Location> { request.Start, request.End },
                EstimatedDuration = TimeSpan.FromMinutes(15),
                EstimatedCost = 25.50m,
                Distance = 5.2
            };
        }

        private static async Task<List<Recommendation>> CalculateUserRecommendationsAsync(
            User user, 
            List<Product> products, 
            CancellationToken cancellationToken)
        {
            await Task.Delay(20, cancellationToken); // Simulate ML calculation
            
            return products
                .Where(p => p.Category != user.ExcludedCategory)
                .OrderByDescending(p => p.Rating)
                .Take(10)
                .Select(p => new Recommendation
                {
                    Product = p,
                    Score = p.Rating / 5.0,
                    Reason = "High rating match"
                })
                .ToList();
        }

        #endregion
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
        public DateTime LastUpdate { get; set; }
    }

    public class Document
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public string Content { get; set; }
        public DateTime CreatedAt { get; set; }
    }

    public class SearchIndex
    {
        public Dictionary<string, List<int>> TokenToDocuments { get; set; } = new();
    }

    public class Transaction
    {
        public string Id { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Currency { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class TransactionResult
    {
        public string TransactionId { get; set; }
        public TransactionStatus Status { get; set; }
        public DateTime ProcessedAt { get; set; }
        public string ErrorMessage { get; set; }
    }

    public enum TransactionStatus
    {
        Pending,
        Success,
        Failed,
        Cancelled
    }

    public class RouteRequest
    {
        public string Id { get; set; }
        public Location Start { get; set; }
        public Location End { get; set; }
        public DateTime RequestTime { get; set; }
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    public class RouteOptimization
    {
        public string RequestId { get; set; }
        public List<Location> OptimalRoute { get; set; }
        public TimeSpan EstimatedDuration { get; set; }
        public decimal EstimatedCost { get; set; }
        public double Distance { get; set; }
    }

    public class User
    {
        public string Id { get; set; }
        public string Name { get; set; }
        public string ExcludedCategory { get; set; }
        public List<string> Preferences { get; set; } = new();
    }

    public class Product
    {
        public string Id { get; set; }
        public string Name { get; set; }
        public string Category { get; set; }
        public decimal Price { get; set; }
        public double Rating { get; set; }
    }

    public class Recommendation
    {
        public Product Product { get; set; }
        public double Score { get; set; }
        public string Reason { get; set; }
    }

    public class TransformedData
    {
        public string Id { get; set; }
        public string Data { get; set; }
        public DateTime ProcessedAt { get; set; }
    }

    public class ValidationError
    {
        public string Item { get; set; }
        public string Rule { get; set; }
        public string Message { get; set; }
    }

    public class ValidationResult
    {
        public bool IsValid { get; set; }
        public List<ValidationError> Errors { get; set; } = new();
        public int TotalItems { get; set; }
        public int ErrorCount { get; set; }
    }

    #endregion
}
