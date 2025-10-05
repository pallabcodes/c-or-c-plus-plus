using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Reactive;
using System.Reactive.Concurrency;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Reactive.Backpressure
{
    /// <summary>
    /// Backpressure patterns for handling high-volume data streams.
    /// Used by Google for search indexing, Bloomberg for market data,
    /// and PayPal for transaction processing.
    /// </summary>
    public class BackpressurePatterns
    {
        /// <summary>
        /// Google-style search indexing with backpressure control
        /// Handles millions of documents with controlled memory usage
        /// </summary>
        public IObservable<IndexedDocument> CreateSearchIndexingWithBackpressure(
            IObservable<Document> documents,
            int maxConcurrency = 10,
            int bufferSize = 1000)
        {
            var semaphore = new SemaphoreSlim(maxConcurrency, maxConcurrency);
            var documentQueue = new BlockingCollection<Document>(bufferSize);

            // Producer task
            var producerTask = Task.Run(async () =>
            {
                try
                {
                    await documents.ForEachAsync(async doc =>
                    {
                        if (!documentQueue.IsAddingCompleted)
                        {
                            documentQueue.Add(doc);
                        }
                    });
                }
                finally
                {
                    documentQueue.CompleteAdding();
                }
            });

            // Consumer observable
            return Observable.Create<IndexedDocument>(observer =>
            {
                var cancellationTokenSource = new CancellationTokenSource();
                var consumerTask = Task.Run(async () =>
                {
                    try
                    {
                        foreach (var document in documentQueue.GetConsumingEnumerable(cancellationTokenSource.Token))
                        {
                            await semaphore.WaitAsync(cancellationTokenSource.Token);
                            
                            try
                            {
                                var indexedDoc = await IndexDocumentAsync(document);
                                observer.OnNext(indexedDoc);
                            }
                            finally
                            {
                                semaphore.Release();
                            }
                        }
                        observer.OnCompleted();
                    }
                    catch (OperationCanceledException)
                    {
                        observer.OnCompleted();
                    }
                    catch (Exception ex)
                    {
                        observer.OnError(ex);
                    }
                });

                return new CompositeDisposable(
                    cancellationTokenSource,
                    Disposable.Create(() => cancellationTokenSource.Cancel()),
                    Disposable.Create(() => documentQueue.CompleteAdding())
                );
            });
        }

        private async Task<IndexedDocument> IndexDocumentAsync(Document document)
        {
            await Task.Delay(100); // Simulate indexing
            return new IndexedDocument
            {
                DocumentId = document.Id,
                Title = document.Title,
                Content = document.Content,
                Tokens = ExtractTokens(document.Content),
                IndexedAt = DateTime.UtcNow
            };
        }

        private List<string> ExtractTokens(string content)
        {
            return content.Split(' ', StringSplitOptions.RemoveEmptyEntries)
                .Select(w => w.ToLowerInvariant().Trim('.,!?;:'))
                .Where(w => w.Length > 2)
                .ToList();
        }

        /// <summary>
        /// Bloomberg-style market data processing with sliding window backpressure
        /// Handles high-frequency market data with memory control
        /// </summary>
        public IObservable<MarketDataSummary> CreateMarketDataWithBackpressure(
            IObservable<MarketData> marketData,
            TimeSpan windowSize = default,
            int maxItemsPerWindow = 1000)
        {
            if (windowSize == default)
                windowSize = TimeSpan.FromSeconds(5);

            return marketData
                .GroupBy(data => data.Symbol)
                .SelectMany(group => group
                    .Buffer(windowSize, TimeSpan.FromSeconds(1))
                    .Where(buffer => buffer.Any())
                    .Select(buffer => ProcessMarketDataWindow(group.Key, buffer, maxItemsPerWindow))
                    .Where(summary => summary != null))
                .Publish()
                .RefCount();
        }

        private MarketDataSummary ProcessMarketDataWindow(string symbol, IList<MarketData> data, int maxItems)
        {
            // Apply backpressure by limiting items per window
            var limitedData = data.Take(maxItems).ToList();
            
            if (!limitedData.Any())
                return null;

            return new MarketDataSummary
            {
                Symbol = symbol,
                AveragePrice = limitedData.Average(d => d.Price),
                MaxPrice = limitedData.Max(d => d.Price),
                MinPrice = limitedData.Min(d => d.Price),
                TotalVolume = limitedData.Sum(d => d.Volume),
                ItemCount = limitedData.Count,
                ProcessedAt = DateTime.UtcNow
            };
        }

        /// <summary>
        /// PayPal-style transaction processing with rate limiting
        /// Handles payment transactions with controlled processing rate
        /// </summary>
        public IObservable<ProcessedTransaction> CreateTransactionProcessingWithBackpressure(
            IObservable<Transaction> transactions,
            int maxTransactionsPerSecond = 100)
        {
            var rateLimiter = new RateLimiter(maxTransactionsPerSecond);
            var transactionQueue = new BlockingCollection<Transaction>(10000);

            // Producer task
            var producerTask = Task.Run(async () =>
            {
                try
                {
                    await transactions.ForEachAsync(async transaction =>
                    {
                        if (!transactionQueue.IsAddingCompleted)
                        {
                            transactionQueue.Add(transaction);
                        }
                    });
                }
                finally
                {
                    transactionQueue.CompleteAdding();
                }
            });

            // Consumer observable with rate limiting
            return Observable.Create<ProcessedTransaction>(observer =>
            {
                var cancellationTokenSource = new CancellationTokenSource();
                var consumerTask = Task.Run(async () =>
                {
                    try
                    {
                        foreach (var transaction in transactionQueue.GetConsumingEnumerable(cancellationTokenSource.Token))
                        {
                            await rateLimiter.WaitAsync();
                            
                            try
                            {
                                var processedTransaction = await ProcessTransactionAsync(transaction);
                                observer.OnNext(processedTransaction);
                            }
                            catch (Exception ex)
                            {
                                observer.OnError(ex);
                            }
                        }
                        observer.OnCompleted();
                    }
                    catch (OperationCanceledException)
                    {
                        observer.OnCompleted();
                    }
                    catch (Exception ex)
                    {
                        observer.OnError(ex);
                    }
                });

                return new CompositeDisposable(
                    cancellationTokenSource,
                    Disposable.Create(() => cancellationTokenSource.Cancel()),
                    Disposable.Create(() => transactionQueue.CompleteAdding())
                );
            });
        }

        private async Task<ProcessedTransaction> ProcessTransactionAsync(Transaction transaction)
        {
            await Task.Delay(50); // Simulate processing
            return new ProcessedTransaction
            {
                TransactionId = transaction.Id,
                CustomerId = transaction.CustomerId,
                Amount = transaction.Amount,
                Status = TransactionStatus.Success,
                ProcessedAt = DateTime.UtcNow
            };
        }

        /// <summary>
        /// Uber-style ride matching with backpressure and priority queuing
        /// Handles ride requests with priority-based processing
        /// </summary>
        public IObservable<RideMatch> CreateRideMatchingWithBackpressure(
            IObservable<RideRequest> requests,
            IObservable<DriverLocation> driverUpdates,
            int maxConcurrentMatches = 50)
        {
            var semaphore = new SemaphoreSlim(maxConcurrentMatches, maxConcurrentMatches);
            var priorityQueue = new ConcurrentPriorityQueue<RideRequest>();
            var driverLocations = new ConcurrentDictionary<string, DriverLocation>();

            // Update driver locations
            driverUpdates
                .Subscribe(location => driverLocations[location.DriverId] = location);

            // Add requests to priority queue
            requests
                .Subscribe(request => priorityQueue.Enqueue(request, GetRequestPriority(request)));

            // Process requests with backpressure
            return Observable.Create<RideMatch>(observer =>
            {
                var cancellationTokenSource = new CancellationTokenSource();
                var processingTask = Task.Run(async () =>
                {
                    try
                    {
                        while (!cancellationTokenSource.Token.IsCancellationRequested)
                        {
                            if (priorityQueue.TryDequeue(out var request, out var priority))
                            {
                                await semaphore.WaitAsync(cancellationTokenSource.Token);
                                
                                try
                                {
                                    var match = await FindBestDriverAsync(request, driverLocations);
                                    if (match != null)
                                    {
                                        observer.OnNext(match);
                                    }
                                }
                                finally
                                {
                                    semaphore.Release();
                                }
                            }
                            else
                            {
                                await Task.Delay(100, cancellationTokenSource.Token);
                            }
                        }
                        observer.OnCompleted();
                    }
                    catch (OperationCanceledException)
                    {
                        observer.OnCompleted();
                    }
                    catch (Exception ex)
                    {
                        observer.OnError(ex);
                    }
                });

                return new CompositeDisposable(
                    cancellationTokenSource,
                    Disposable.Create(() => cancellationTokenSource.Cancel())
                );
            });
        }

        private int GetRequestPriority(RideRequest request)
        {
            // Higher priority for older requests
            var age = DateTime.UtcNow - request.RequestTime;
            return (int)age.TotalSeconds;
        }

        private async Task<RideMatch> FindBestDriverAsync(RideRequest request, ConcurrentDictionary<string, DriverLocation> driverLocations)
        {
            await Task.Delay(10); // Simulate processing

            var availableDrivers = driverLocations.Values
                .Where(driver => driver.IsAvailable)
                .Where(driver => CalculateDistance(request.PickupLocation, driver.Location) <= 5.0)
                .ToList();

            if (!availableDrivers.Any())
                return null;

            var bestDriver = availableDrivers
                .OrderBy(driver => CalculateDistance(request.PickupLocation, driver.Location))
                .First();

            return new RideMatch
            {
                RequestId = request.Id,
                DriverId = bestDriver.DriverId,
                Distance = CalculateDistance(request.PickupLocation, bestDriver.Location),
                EstimatedArrival = TimeSpan.FromMinutes(5),
                Timestamp = DateTime.UtcNow
            };
        }

        private double CalculateDistance(Location loc1, Location loc2)
        {
            return Math.Sqrt(Math.Pow(loc1.Latitude - loc2.Latitude, 2) + 
                           Math.Pow(loc1.Longitude - loc2.Longitude, 2));
        }

        /// <summary>
        /// Amazon-style recommendation processing with adaptive backpressure
        /// Handles recommendation requests with dynamic rate limiting
        /// </summary>
        public IObservable<Recommendation> CreateRecommendationWithBackpressure(
            IObservable<RecommendationRequest> requests,
            int initialRate = 50,
            int maxRate = 200)
        {
            var adaptiveRateLimiter = new AdaptiveRateLimiter(initialRate, maxRate);
            var requestQueue = new BlockingCollection<RecommendationRequest>(5000);

            // Producer task
            var producerTask = Task.Run(async () =>
            {
                try
                {
                    await requests.ForEachAsync(async request =>
                    {
                        if (!requestQueue.IsAddingCompleted)
                        {
                            requestQueue.Add(request);
                        }
                    });
                }
                finally
                {
                    requestQueue.CompleteAdding();
                }
            });

            // Consumer observable with adaptive rate limiting
            return Observable.Create<Recommendation>(observer =>
            {
                var cancellationTokenSource = new CancellationTokenSource();
                var consumerTask = Task.Run(async () =>
                {
                    try
                    {
                        foreach (var request in requestQueue.GetConsumingEnumerable(cancellationTokenSource.Token))
                        {
                            await adaptiveRateLimiter.WaitAsync();
                            
                            try
                            {
                                var recommendation = await GenerateRecommendationAsync(request);
                                observer.OnNext(recommendation);
                                adaptiveRateLimiter.RecordSuccess();
                            }
                            catch (Exception ex)
                            {
                                adaptiveRateLimiter.RecordFailure();
                                observer.OnError(ex);
                            }
                        }
                        observer.OnCompleted();
                    }
                    catch (OperationCanceledException)
                    {
                        observer.OnCompleted();
                    }
                    catch (Exception ex)
                    {
                        observer.OnError(ex);
                    }
                });

                return new CompositeDisposable(
                    cancellationTokenSource,
                    Disposable.Create(() => cancellationTokenSource.Cancel()),
                    Disposable.Create(() => requestQueue.CompleteAdding())
                );
            });
        }

        private async Task<Recommendation> GenerateRecommendationAsync(RecommendationRequest request)
        {
            await Task.Delay(100); // Simulate processing
            return new Recommendation
            {
                UserId = request.UserId,
                ProductId = Guid.NewGuid().ToString(),
                ProductName = $"Recommended Product for {request.UserId}",
                Score = Random.Shared.NextDouble(),
                Timestamp = DateTime.UtcNow
            };
        }
    }

    #region Supporting Classes

    public class Document
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public string Content { get; set; }
    }

    public class IndexedDocument
    {
        public string DocumentId { get; set; }
        public string Title { get; set; }
        public string Content { get; set; }
        public List<string> Tokens { get; set; } = new();
        public DateTime IndexedAt { get; set; }
    }

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
        public decimal AveragePrice { get; set; }
        public decimal MaxPrice { get; set; }
        public decimal MinPrice { get; set; }
        public decimal TotalVolume { get; set; }
        public int ItemCount { get; set; }
        public DateTime ProcessedAt { get; set; }
    }

    public class Transaction
    {
        public string Id { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class ProcessedTransaction
    {
        public string TransactionId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public TransactionStatus Status { get; set; }
        public DateTime ProcessedAt { get; set; }
    }

    public enum TransactionStatus
    {
        Pending,
        Success,
        Failed,
        Cancelled
    }

    public class RideRequest
    {
        public string Id { get; set; }
        public string PassengerId { get; set; }
        public Location PickupLocation { get; set; }
        public DateTime RequestTime { get; set; }
    }

    public class DriverLocation
    {
        public string DriverId { get; set; }
        public Location Location { get; set; }
        public bool IsAvailable { get; set; }
        public DateTime LastUpdate { get; set; }
    }

    public class RideMatch
    {
        public string RequestId { get; set; }
        public string DriverId { get; set; }
        public double Distance { get; set; }
        public TimeSpan EstimatedArrival { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    public class RecommendationRequest
    {
        public string UserId { get; set; }
        public string Category { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class Recommendation
    {
        public string UserId { get; set; }
        public string ProductId { get; set; }
        public string ProductName { get; set; }
        public double Score { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class RateLimiter
    {
        private readonly SemaphoreSlim _semaphore;
        private readonly Timer _timer;

        public RateLimiter(int maxRequestsPerSecond)
        {
            _semaphore = new SemaphoreSlim(maxRequestsPerSecond, maxRequestsPerSecond);
            _timer = new Timer(ReleaseTokens, null, TimeSpan.Zero, TimeSpan.FromSeconds(1));
        }

        private void ReleaseTokens(object state)
        {
            _semaphore.Release();
        }

        public async Task WaitAsync()
        {
            await _semaphore.WaitAsync();
        }

        public void Dispose()
        {
            _timer?.Dispose();
            _semaphore?.Dispose();
        }
    }

    public class AdaptiveRateLimiter
    {
        private readonly SemaphoreSlim _semaphore;
        private readonly int _maxRate;
        private readonly Timer _timer;
        private int _currentRate;
        private int _successCount;
        private int _failureCount;
        private readonly object _lock = new object();

        public AdaptiveRateLimiter(int initialRate, int maxRate)
        {
            _currentRate = initialRate;
            _maxRate = maxRate;
            _semaphore = new SemaphoreSlim(initialRate, maxRate);
            _timer = new Timer(AdjustRate, null, TimeSpan.FromSeconds(1), TimeSpan.FromSeconds(1));
        }

        private void AdjustRate(object state)
        {
            lock (_lock)
            {
                var totalRequests = _successCount + _failureCount;
                if (totalRequests > 0)
                {
                    var successRate = (double)_successCount / totalRequests;
                    
                    if (successRate > 0.95 && _currentRate < _maxRate)
                    {
                        _currentRate = Math.Min(_currentRate + 10, _maxRate);
                    }
                    else if (successRate < 0.8 && _currentRate > 10)
                    {
                        _currentRate = Math.Max(_currentRate - 10, 10);
                    }
                }

                _successCount = 0;
                _failureCount = 0;
            }
        }

        public async Task WaitAsync()
        {
            await _semaphore.WaitAsync();
        }

        public void RecordSuccess()
        {
            lock (_lock)
            {
                _successCount++;
            }
        }

        public void RecordFailure()
        {
            lock (_lock)
            {
                _failureCount++;
            }
        }

        public void Dispose()
        {
            _timer?.Dispose();
            _semaphore?.Dispose();
        }
    }

    public class ConcurrentPriorityQueue<T>
    {
        private readonly SortedDictionary<int, Queue<T>> _queues = new();
        private readonly object _lock = new object();

        public void Enqueue(T item, int priority)
        {
            lock (_lock)
            {
                if (!_queues.ContainsKey(priority))
                {
                    _queues[priority] = new Queue<T>();
                }
                _queues[priority].Enqueue(item);
            }
        }

        public bool TryDequeue(out T item, out int priority)
        {
            lock (_lock)
            {
                if (_queues.Any())
                {
                    var highestPriority = _queues.Keys.Max();
                    var queue = _queues[highestPriority];
                    
                    if (queue.Count > 0)
                    {
                        item = queue.Dequeue();
                        priority = highestPriority;
                        
                        if (queue.Count == 0)
                        {
                            _queues.Remove(highestPriority);
                        }
                        
                        return true;
                    }
                }
                
                item = default;
                priority = 0;
                return false;
            }
        }
    }

    #endregion
}
