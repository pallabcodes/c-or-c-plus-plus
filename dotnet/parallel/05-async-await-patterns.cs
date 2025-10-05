using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Parallel.AsyncAwait
{
    /// <summary>
    /// Advanced async/await patterns used by top-tier companies.
    /// Covers cancellation, timeout, retry, circuit breaker, and backpressure patterns.
    /// </summary>
    public class AsyncAwaitPatterns
    {
        private readonly HttpClient _httpClient;
        private readonly SemaphoreSlim _semaphore;

        public AsyncAwaitPatterns()
        {
            _httpClient = new HttpClient();
            _semaphore = new SemaphoreSlim(10, 10); // Limit to 10 concurrent requests
        }

        /// <summary>
        /// Google-style async data fetching with cancellation and timeout
        /// Used in Google Search, Maps, and Cloud services
        /// </summary>
        public async Task<SearchResult> FetchSearchResultAsync(
            string query, 
            CancellationToken cancellationToken = default)
        {
            using var timeoutCts = new CancellationTokenSource(TimeSpan.FromSeconds(5));
            using var combinedCts = CancellationTokenSource.CreateLinkedTokenSource(
                cancellationToken, timeoutCts.Token);

            try
            {
                // Simulate API call with proper cancellation
                await Task.Delay(2000, combinedCts.Token);
                
                return new SearchResult
                {
                    Query = query,
                    Results = new List<string> { $"Result 1 for {query}", $"Result 2 for {query}" },
                    Timestamp = DateTime.UtcNow
                };
            }
            catch (OperationCanceledException) when (timeoutCts.Token.IsCancellationRequested)
            {
                throw new TimeoutException($"Search request timed out after 5 seconds");
            }
            catch (OperationCanceledException)
            {
                throw; // Re-throw if cancelled by user
            }
        }

        /// <summary>
        /// Bloomberg-style market data fetching with retry and circuit breaker
        /// Used in Bloomberg Terminal and market data APIs
        /// </summary>
        public async Task<MarketData> FetchMarketDataWithRetryAsync(
            string symbol,
            int maxRetries = 3,
            CancellationToken cancellationToken = default)
        {
            var retryPolicy = new RetryPolicy(maxRetries, TimeSpan.FromMilliseconds(100));
            var circuitBreaker = new CircuitBreaker(5, TimeSpan.FromMinutes(1));

            for (int attempt = 0; attempt <= maxRetries; attempt++)
            {
                try
                {
                    if (!circuitBreaker.CanExecute())
                    {
                        throw new CircuitBreakerOpenException("Circuit breaker is open");
                    }

                    var result = await FetchMarketDataInternalAsync(symbol, cancellationToken);
                    circuitBreaker.RecordSuccess();
                    return result;
                }
                catch (Exception ex) when (attempt < maxRetries)
                {
                    circuitBreaker.RecordFailure();
                    var delay = retryPolicy.GetDelay(attempt);
                    await Task.Delay(delay, cancellationToken);
                }
            }

            throw new InvalidOperationException($"Failed to fetch market data for {symbol} after {maxRetries} retries");
        }

        private async Task<MarketData> FetchMarketDataInternalAsync(string symbol, CancellationToken cancellationToken)
        {
            // Simulate API call
            await Task.Delay(1000, cancellationToken);
            
            // Simulate occasional failures
            if (Random.Shared.NextDouble() < 0.3)
            {
                throw new HttpRequestException("Market data API temporarily unavailable");
            }

            return new MarketData
            {
                Symbol = symbol,
                Price = 100 + Random.Shared.Next(-10, 10),
                Volume = Random.Shared.Next(1000, 10000),
                Timestamp = DateTime.UtcNow
            };
        }

        /// <summary>
        /// PayPal-style payment processing with backpressure control
        /// Used in PayPal's payment processing systems
        /// </summary>
        public async Task<PaymentResult> ProcessPaymentWithBackpressureAsync(
            PaymentRequest request,
            CancellationToken cancellationToken = default)
        {
            // Acquire semaphore to control concurrency
            await _semaphore.WaitAsync(cancellationToken);
            
            try
            {
                // Simulate payment processing
                await Task.Delay(500, cancellationToken);
                
                // Simulate validation
                if (request.Amount <= 0)
                {
                    return new PaymentResult
                    {
                        Success = false,
                        ErrorMessage = "Invalid amount"
                    };
                }

                return new PaymentResult
                {
                    Success = true,
                    TransactionId = Guid.NewGuid().ToString(),
                    ProcessedAt = DateTime.UtcNow
                };
            }
            finally
            {
                _semaphore.Release();
            }
        }

        /// <summary>
        /// Uber-style batch processing with async enumeration
        /// Used in Uber's ride matching and pricing systems
        /// </summary>
        public async IAsyncEnumerable<RideMatch> ProcessRideRequestsAsync(
            IAsyncEnumerable<RideRequest> requests,
            [System.Runtime.CompilerServices.EnumeratorCancellation] CancellationToken cancellationToken = default)
        {
            await foreach (var request in requests.WithCancellation(cancellationToken))
            {
                var match = await FindBestDriverAsync(request, cancellationToken);
                if (match != null)
                {
                    yield return match;
                }
            }
        }

        private async Task<RideMatch> FindBestDriverAsync(RideRequest request, CancellationToken cancellationToken)
        {
            await Task.Delay(100, cancellationToken); // Simulate driver search
            
            // Simulate finding a driver
            if (Random.Shared.NextDouble() < 0.8) // 80% success rate
            {
                return new RideMatch
                {
                    RequestId = request.Id,
                    DriverId = Guid.NewGuid().ToString(),
                    EstimatedArrival = TimeSpan.FromMinutes(Random.Shared.Next(5, 15)),
                    Timestamp = DateTime.UtcNow
                };
            }

            return null;
        }

        /// <summary>
        /// Amazon-style parallel processing with Task.WhenAll
        /// Used in Amazon's recommendation and search systems
        /// </summary>
        public async Task<RecommendationResult> GetRecommendationsAsync(
            string userId,
            CancellationToken cancellationToken = default)
        {
            var tasks = new[]
            {
                GetPurchaseHistoryAsync(userId, cancellationToken),
                GetBrowsingHistoryAsync(userId, cancellationToken),
                GetSimilarUsersAsync(userId, cancellationToken),
                GetTrendingItemsAsync(cancellationToken)
            };

            try
            {
                var results = await Task.WhenAll(tasks);
                
                return new RecommendationResult
                {
                    UserId = userId,
                    PurchaseHistory = results[0],
                    BrowsingHistory = results[1],
                    SimilarUsers = results[2],
                    TrendingItems = results[3],
                    GeneratedAt = DateTime.UtcNow
                };
            }
            catch (Exception ex)
            {
                // Log error and return partial results
                return new RecommendationResult
                {
                    UserId = userId,
                    Error = ex.Message,
                    GeneratedAt = DateTime.UtcNow
                };
            }
        }

        private async Task<List<string>> GetPurchaseHistoryAsync(string userId, CancellationToken cancellationToken)
        {
            await Task.Delay(200, cancellationToken);
            return new List<string> { "Item1", "Item2", "Item3" };
        }

        private async Task<List<string>> GetBrowsingHistoryAsync(string userId, CancellationToken cancellationToken)
        {
            await Task.Delay(150, cancellationToken);
            return new List<string> { "Item4", "Item5" };
        }

        private async Task<List<string>> GetSimilarUsersAsync(string userId, CancellationToken cancellationToken)
        {
            await Task.Delay(300, cancellationToken);
            return new List<string> { "User1", "User2" };
        }

        private async Task<List<string>> GetTrendingItemsAsync(CancellationToken cancellationToken)
        {
            await Task.Delay(100, cancellationToken);
            return new List<string> { "Trending1", "Trending2", "Trending3" };
        }

        /// <summary>
        /// Stripe-style webhook processing with async batching
        /// Used in Stripe's webhook processing system
        /// </summary>
        public async Task ProcessWebhooksAsync(
            IAsyncEnumerable<WebhookEvent> events,
            CancellationToken cancellationToken = default)
        {
            const int batchSize = 10;
            var batch = new List<WebhookEvent>(batchSize);

            await foreach (var webhook in events.WithCancellation(cancellationToken))
            {
                batch.Add(webhook);

                if (batch.Count >= batchSize)
                {
                    await ProcessBatchAsync(batch, cancellationToken);
                    batch.Clear();
                }
            }

            // Process remaining items
            if (batch.Count > 0)
            {
                await ProcessBatchAsync(batch, cancellationToken);
            }
        }

        private async Task ProcessBatchAsync(List<WebhookEvent> batch, CancellationToken cancellationToken)
        {
            var tasks = batch.Select(webhook => ProcessWebhookAsync(webhook, cancellationToken));
            await Task.WhenAll(tasks);
        }

        private async Task ProcessWebhookAsync(WebhookEvent webhook, CancellationToken cancellationToken)
        {
            await Task.Delay(50, cancellationToken); // Simulate processing
            Console.WriteLine($"Processed webhook: {webhook.EventType} for {webhook.ObjectId}");
        }

        /// <summary>
        /// Atlassian-style async file processing with progress reporting
        /// Used in Atlassian's file upload and processing systems
        /// </summary>
        public async Task<FileProcessingResult> ProcessFileAsync(
            string filePath,
            IProgress<FileProcessingProgress> progress,
            CancellationToken cancellationToken = default)
        {
            var result = new FileProcessingResult { FilePath = filePath };

            try
            {
                // Step 1: Validate file
                progress?.Report(new FileProcessingProgress { Step = "Validating", Percentage = 10 });
                await ValidateFileAsync(filePath, cancellationToken);

                // Step 2: Upload file
                progress?.Report(new FileProcessingProgress { Step = "Uploading", Percentage = 30 });
                var uploadResult = await UploadFileAsync(filePath, cancellationToken);
                result.UploadId = uploadResult.UploadId;

                // Step 3: Process file
                progress?.Report(new FileProcessingProgress { Step = "Processing", Percentage = 50 });
                var processResult = await ProcessFileContentAsync(uploadResult.UploadId, cancellationToken);
                result.ProcessedContent = processResult;

                // Step 4: Generate thumbnails
                progress?.Report(new FileProcessingProgress { Step = "Generating thumbnails", Percentage = 80 });
                await GenerateThumbnailsAsync(uploadResult.UploadId, cancellationToken);

                // Step 5: Complete
                progress?.Report(new FileProcessingProgress { Step = "Complete", Percentage = 100 });
                result.Success = true;
            }
            catch (Exception ex)
            {
                result.Success = false;
                result.Error = ex.Message;
            }

            return result;
        }

        private async Task ValidateFileAsync(string filePath, CancellationToken cancellationToken)
        {
            await Task.Delay(200, cancellationToken);
            // Simulate validation
        }

        private async Task<UploadResult> UploadFileAsync(string filePath, CancellationToken cancellationToken)
        {
            await Task.Delay(500, cancellationToken);
            return new UploadResult { UploadId = Guid.NewGuid().ToString() };
        }

        private async Task<string> ProcessFileContentAsync(string uploadId, CancellationToken cancellationToken)
        {
            await Task.Delay(1000, cancellationToken);
            return $"Processed content for {uploadId}";
        }

        private async Task GenerateThumbnailsAsync(string uploadId, CancellationToken cancellationToken)
        {
            await Task.Delay(300, cancellationToken);
            // Simulate thumbnail generation
        }

        public void Dispose()
        {
            _httpClient?.Dispose();
            _semaphore?.Dispose();
        }
    }

    #region Supporting Classes

    public class SearchResult
    {
        public string Query { get; set; }
        public List<string> Results { get; set; } = new();
        public DateTime Timestamp { get; set; }
    }

    public class MarketData
    {
        public string Symbol { get; set; }
        public decimal Price { get; set; }
        public decimal Volume { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class PaymentRequest
    {
        public string Id { get; set; }
        public decimal Amount { get; set; }
        public string Currency { get; set; }
        public string CustomerId { get; set; }
    }

    public class PaymentResult
    {
        public bool Success { get; set; }
        public string TransactionId { get; set; }
        public string ErrorMessage { get; set; }
        public DateTime ProcessedAt { get; set; }
    }

    public class RideRequest
    {
        public string Id { get; set; }
        public string PassengerId { get; set; }
        public DateTime RequestTime { get; set; }
    }

    public class RideMatch
    {
        public string RequestId { get; set; }
        public string DriverId { get; set; }
        public TimeSpan EstimatedArrival { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class RecommendationResult
    {
        public string UserId { get; set; }
        public List<string> PurchaseHistory { get; set; } = new();
        public List<string> BrowsingHistory { get; set; } = new();
        public List<string> SimilarUsers { get; set; } = new();
        public List<string> TrendingItems { get; set; } = new();
        public string Error { get; set; }
        public DateTime GeneratedAt { get; set; }
    }

    public class WebhookEvent
    {
        public string Id { get; set; }
        public string EventType { get; set; }
        public string ObjectId { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class FileProcessingResult
    {
        public string FilePath { get; set; }
        public bool Success { get; set; }
        public string UploadId { get; set; }
        public string ProcessedContent { get; set; }
        public string Error { get; set; }
    }

    public class FileProcessingProgress
    {
        public string Step { get; set; }
        public int Percentage { get; set; }
    }

    public class UploadResult
    {
        public string UploadId { get; set; }
    }

    public class RetryPolicy
    {
        private readonly int _maxRetries;
        private readonly TimeSpan _baseDelay;

        public RetryPolicy(int maxRetries, TimeSpan baseDelay)
        {
            _maxRetries = maxRetries;
            _baseDelay = baseDelay;
        }

        public TimeSpan GetDelay(int attempt)
        {
            // Exponential backoff with jitter
            var delay = TimeSpan.FromMilliseconds(_baseDelay.TotalMilliseconds * Math.Pow(2, attempt));
            var jitter = TimeSpan.FromMilliseconds(Random.Shared.Next(0, 100));
            return delay.Add(jitter);
        }
    }

    public class CircuitBreaker
    {
        private readonly int _failureThreshold;
        private readonly TimeSpan _timeout;
        private int _failureCount;
        private DateTime _lastFailureTime;
        private CircuitBreakerState _state = CircuitBreakerState.Closed;

        public CircuitBreaker(int failureThreshold, TimeSpan timeout)
        {
            _failureThreshold = failureThreshold;
            _timeout = timeout;
        }

        public bool CanExecute()
        {
            if (_state == CircuitBreakerState.Open)
            {
                if (DateTime.UtcNow - _lastFailureTime > _timeout)
                {
                    _state = CircuitBreakerState.HalfOpen;
                    return true;
                }
                return false;
            }
            return true;
        }

        public void RecordSuccess()
        {
            _failureCount = 0;
            _state = CircuitBreakerState.Closed;
        }

        public void RecordFailure()
        {
            _failureCount++;
            _lastFailureTime = DateTime.UtcNow;
            
            if (_failureCount >= _failureThreshold)
            {
                _state = CircuitBreakerState.Open;
            }
        }
    }

    public enum CircuitBreakerState
    {
        Closed,
        Open,
        HalfOpen
    }

    public class CircuitBreakerOpenException : Exception
    {
        public CircuitBreakerOpenException(string message) : base(message) { }
    }

    #endregion
}
