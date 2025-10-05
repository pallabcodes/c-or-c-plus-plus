using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Parallel.ProducerConsumer
{
    /// <summary>
    /// Producer-Consumer patterns for high-throughput data processing.
    /// Used by Google for search indexing pipelines, Bloomberg for market data processing,
    /// and PayPal for transaction processing queues.
    /// </summary>
    public class ProducerConsumerPatterns
    {
        /// <summary>
        /// Google-style search indexing pipeline with producer-consumer pattern
        /// Processes millions of documents with controlled concurrency
        /// </summary>
        public class SearchIndexingPipeline
        {
            private readonly BlockingCollection<Document> _documentQueue;
            private readonly BlockingCollection<IndexedDocument> _indexedQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task[] _producerTasks;
            private readonly Task[] _consumerTasks;

            public SearchIndexingPipeline(int producerCount = 2, int consumerCount = Environment.ProcessorCount)
            {
                _documentQueue = new BlockingCollection<Document>(1000); // Bounded queue
                _indexedQueue = new BlockingCollection<IndexedDocument>(1000);
                _cancellationTokenSource = new CancellationTokenSource();

                // Start producers
                _producerTasks = new Task[producerCount];
                for (int i = 0; i < producerCount; i++)
                {
                    _producerTasks[i] = Task.Run(() => ProducerLoop(_cancellationTokenSource.Token));
                }

                // Start consumers
                _consumerTasks = new Task[consumerCount];
                for (int i = 0; i < consumerCount; i++)
                {
                    _consumerTasks[i] = Task.Run(() => ConsumerLoop(_cancellationTokenSource.Token));
                }
            }

            public void AddDocument(Document document)
            {
                if (!_documentQueue.IsAddingCompleted)
                {
                    _documentQueue.Add(document);
                }
            }

            public IndexedDocument GetIndexedDocument()
            {
                return _indexedQueue.Take();
            }

            public void Complete()
            {
                _documentQueue.CompleteAdding();
                Task.WaitAll(_producerTasks);
                _indexedQueue.CompleteAdding();
                Task.WaitAll(_consumerTasks);
            }

            private async Task ProducerLoop(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var document in _documentQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        // Simulate document processing (fetching, parsing, etc.)
                        await ProcessDocumentAsync(document);
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
            }

            private async Task ConsumerLoop(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var document in _documentQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        var indexedDocument = await IndexDocumentAsync(document);
                        _indexedQueue.Add(indexedDocument);
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
            }

            private async Task ProcessDocumentAsync(Document document)
            {
                await Task.Delay(50); // Simulate processing
                document.ProcessedAt = DateTime.UtcNow;
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

            public void Dispose()
            {
                _cancellationTokenSource.Cancel();
                Complete();
                _cancellationTokenSource.Dispose();
                _documentQueue.Dispose();
                _indexedQueue.Dispose();
            }
        }

        /// <summary>
        /// Bloomberg-style market data processing pipeline
        /// Processes high-frequency market data with backpressure control
        /// </summary>
        public class MarketDataPipeline
        {
            private readonly BlockingCollection<MarketData> _rawDataQueue;
            private readonly BlockingCollection<ProcessedMarketData> _processedDataQueue;
            private readonly BlockingCollection<MarketDataAlert> _alertQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task[] _processingTasks;

            public MarketDataPipeline(int processingTaskCount = Environment.ProcessorCount)
            {
                _rawDataQueue = new BlockingCollection<MarketData>(5000);
                _processedDataQueue = new BlockingCollection<ProcessedMarketData>(2000);
                _alertQueue = new BlockingCollection<MarketDataAlert>(1000);
                _cancellationTokenSource = new CancellationTokenSource();

                // Start processing tasks
                _processingTasks = new Task[processingTaskCount];
                for (int i = 0; i < processingTaskCount; i++)
                {
                    _processingTasks[i] = Task.Run(() => ProcessingLoop(_cancellationTokenSource.Token));
                }
            }

            public void AddMarketData(MarketData data)
            {
                if (!_rawDataQueue.IsAddingCompleted)
                {
                    _rawDataQueue.Add(data);
                }
            }

            public ProcessedMarketData GetProcessedData()
            {
                return _processedDataQueue.Take();
            }

            public MarketDataAlert GetAlert()
            {
                return _alertQueue.Take();
            }

            private async Task ProcessingLoop(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var data in _rawDataQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        var processedData = await ProcessMarketDataAsync(data);
                        _processedDataQueue.Add(processedData);

                        // Check for alerts
                        var alert = CheckForAlerts(processedData);
                        if (alert != null)
                        {
                            _alertQueue.Add(alert);
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
            }

            private async Task<ProcessedMarketData> ProcessMarketDataAsync(MarketData data)
            {
                await Task.Delay(10); // Simulate processing

                return new ProcessedMarketData
                {
                    Symbol = data.Symbol,
                    Price = data.Price,
                    Volume = data.Volume,
                    Timestamp = data.Timestamp,
                    PriceChange = CalculatePriceChange(data),
                    VolumeChange = CalculateVolumeChange(data),
                    ProcessedAt = DateTime.UtcNow
                };
            }

            private decimal CalculatePriceChange(MarketData data)
            {
                // Simplified price change calculation
                return data.Price * 0.01m; // 1% change simulation
            }

            private decimal CalculateVolumeChange(MarketData data)
            {
                // Simplified volume change calculation
                return data.Volume * 0.05m; // 5% change simulation
            }

            private MarketDataAlert CheckForAlerts(ProcessedMarketData data)
            {
                // Check for significant price movements
                if (Math.Abs(data.PriceChange) > data.Price * 0.05m) // 5% threshold
                {
                    return new MarketDataAlert
                    {
                        Symbol = data.Symbol,
                        AlertType = AlertType.PriceMovement,
                        Message = $"Significant price movement: {data.PriceChange:C}",
                        Timestamp = DateTime.UtcNow
                    };
                }

                // Check for high volume
                if (data.Volume > 1000000) // 1M volume threshold
                {
                    return new MarketDataAlert
                    {
                        Symbol = data.Symbol,
                        AlertType = AlertType.HighVolume,
                        Message = $"High volume detected: {data.Volume:N0}",
                        Timestamp = DateTime.UtcNow
                    };
                }

                return null;
            }

            public void Complete()
            {
                _rawDataQueue.CompleteAdding();
                Task.WaitAll(_processingTasks);
                _processedDataQueue.CompleteAdding();
                _alertQueue.CompleteAdding();
            }

            public void Dispose()
            {
                _cancellationTokenSource.Cancel();
                Complete();
                _cancellationTokenSource.Dispose();
                _rawDataQueue.Dispose();
                _processedDataQueue.Dispose();
                _alertQueue.Dispose();
            }
        }

        /// <summary>
        /// PayPal-style transaction processing pipeline
        /// Processes payment transactions with fraud detection
        /// </summary>
        public class TransactionProcessingPipeline
        {
            private readonly BlockingCollection<Transaction> _transactionQueue;
            private readonly BlockingCollection<ProcessedTransaction> _processedQueue;
            private readonly BlockingCollection<FraudAlert> _fraudQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task[] _processingTasks;

            public TransactionProcessingPipeline(int processingTaskCount = Environment.ProcessorCount)
            {
                _transactionQueue = new BlockingCollection<Transaction>(2000);
                _processedQueue = new BlockingCollection<ProcessedTransaction>(1000);
                _fraudQueue = new BlockingCollection<FraudAlert>(500);
                _cancellationTokenSource = new CancellationTokenSource();

                _processingTasks = new Task[processingTaskCount];
                for (int i = 0; i < processingTaskCount; i++)
                {
                    _processingTasks[i] = Task.Run(() => ProcessingLoop(_cancellationTokenSource.Token));
                }
            }

            public void AddTransaction(Transaction transaction)
            {
                if (!_transactionQueue.IsAddingCompleted)
                {
                    _transactionQueue.Add(transaction);
                }
            }

            public ProcessedTransaction GetProcessedTransaction()
            {
                return _processedQueue.Take();
            }

            public FraudAlert GetFraudAlert()
            {
                return _fraudQueue.Take();
            }

            private async Task ProcessingLoop(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var transaction in _transactionQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        // Fraud detection
                        var fraudCheck = await PerformFraudCheckAsync(transaction);
                        if (fraudCheck.IsFraudulent)
                        {
                            _fraudQueue.Add(new FraudAlert
                            {
                                TransactionId = transaction.Id,
                                CustomerId = transaction.CustomerId,
                                Amount = transaction.Amount,
                                Reason = fraudCheck.Reason,
                                Timestamp = DateTime.UtcNow
                            });
                            continue;
                        }

                        // Process transaction
                        var processedTransaction = await ProcessTransactionAsync(transaction);
                        _processedQueue.Add(processedTransaction);
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
            }

            private async Task<FraudCheckResult> PerformFraudCheckAsync(Transaction transaction)
            {
                await Task.Delay(20); // Simulate fraud check

                // Simple fraud detection logic
                if (transaction.Amount > 10000)
                {
                    return new FraudCheckResult
                    {
                        IsFraudulent = true,
                        Reason = "High value transaction"
                    };
                }

                if (transaction.Amount < 0)
                {
                    return new FraudCheckResult
                    {
                        IsFraudulent = true,
                        Reason = "Negative amount"
                    };
                }

                return new FraudCheckResult { IsFraudulent = false };
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
                    ProcessedAt = DateTime.UtcNow,
                    TransactionFee = CalculateTransactionFee(transaction.Amount)
                };
            }

            private decimal CalculateTransactionFee(decimal amount)
            {
                return amount * 0.029m + 0.30m; // 2.9% + $0.30
            }

            public void Complete()
            {
                _transactionQueue.CompleteAdding();
                Task.WaitAll(_processingTasks);
                _processedQueue.CompleteAdding();
                _fraudQueue.CompleteAdding();
            }

            public void Dispose()
            {
                _cancellationTokenSource.Cancel();
                Complete();
                _cancellationTokenSource.Dispose();
                _transactionQueue.Dispose();
                _processedQueue.Dispose();
                _fraudQueue.Dispose();
            }
        }

        /// <summary>
        /// Uber-style ride matching pipeline
        /// Matches passengers with drivers in real-time
        /// </summary>
        public class RideMatchingPipeline
        {
            private readonly BlockingCollection<RideRequest> _requestQueue;
            private readonly BlockingCollection<DriverLocation> _driverQueue;
            private readonly BlockingCollection<RideMatch> _matchQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task _matchingTask;

            public RideMatchingPipeline()
            {
                _requestQueue = new BlockingCollection<RideRequest>(500);
                _driverQueue = new BlockingCollection<DriverLocation>(1000);
                _matchQueue = new BlockingCollection<RideMatch>(200);
                _cancellationTokenSource = new CancellationTokenSource();

                _matchingTask = Task.Run(() => MatchingLoop(_cancellationTokenSource.Token));
            }

            public void AddRideRequest(RideRequest request)
            {
                if (!_requestQueue.IsAddingCompleted)
                {
                    _requestQueue.Add(request);
                }
            }

            public void UpdateDriverLocation(DriverLocation location)
            {
                if (!_driverQueue.IsAddingCompleted)
                {
                    _driverQueue.Add(location);
                }
            }

            public RideMatch GetMatch()
            {
                return _matchQueue.Take();
            }

            private async Task MatchingLoop(CancellationToken cancellationToken)
            {
                var driverLocations = new List<DriverLocation>();

                try
                {
                    // Process driver location updates
                    var driverTask = Task.Run(async () =>
                    {
                        foreach (var location in _driverQueue.GetConsumingEnumerable(cancellationToken))
                        {
                            lock (driverLocations)
                            {
                                var existing = driverLocations.FirstOrDefault(d => d.DriverId == location.DriverId);
                                if (existing != null)
                                {
                                    driverLocations.Remove(existing);
                                }
                                driverLocations.Add(location);
                            }
                        }
                    }, cancellationToken);

                    // Process ride requests
                    foreach (var request in _requestQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        await ProcessRideRequest(request, driverLocations);
                    }

                    await driverTask;
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
            }

            private async Task ProcessRideRequest(RideRequest request, List<DriverLocation> driverLocations)
            {
                await Task.Delay(10); // Simulate processing

                DriverLocation bestDriver = null;
                double bestDistance = double.MaxValue;

                lock (driverLocations)
                {
                    foreach (var driver in driverLocations.Where(d => d.IsAvailable))
                    {
                        var distance = CalculateDistance(request.PickupLocation, driver.Location);
                        if (distance < bestDistance && distance <= 5.0) // Within 5km
                        {
                            bestDistance = distance;
                            bestDriver = driver;
                        }
                    }
                }

                if (bestDriver != null)
                {
                    var match = new RideMatch
                    {
                        RequestId = request.Id,
                        DriverId = bestDriver.DriverId,
                        PassengerId = request.PassengerId,
                        Distance = bestDistance,
                        EstimatedArrival = TimeSpan.FromMinutes(bestDistance * 2),
                        Timestamp = DateTime.UtcNow
                    };

                    _matchQueue.Add(match);
                }
            }

            private double CalculateDistance(Location loc1, Location loc2)
            {
                return Math.Sqrt(Math.Pow(loc1.Latitude - loc2.Latitude, 2) + Math.Pow(loc1.Longitude - loc2.Longitude, 2));
            }

            public void Complete()
            {
                _requestQueue.CompleteAdding();
                _driverQueue.CompleteAdding();
                _matchQueue.CompleteAdding();
                _matchingTask.Wait();
            }

            public void Dispose()
            {
                _cancellationTokenSource.Cancel();
                Complete();
                _cancellationTokenSource.Dispose();
                _requestQueue.Dispose();
                _driverQueue.Dispose();
                _matchQueue.Dispose();
            }
        }
    }

    #region Supporting Classes

    public class Document
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public string Content { get; set; }
        public DateTime ProcessedAt { get; set; }
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

    public class ProcessedMarketData
    {
        public string Symbol { get; set; }
        public decimal Price { get; set; }
        public decimal Volume { get; set; }
        public DateTime Timestamp { get; set; }
        public decimal PriceChange { get; set; }
        public decimal VolumeChange { get; set; }
        public DateTime ProcessedAt { get; set; }
    }

    public class MarketDataAlert
    {
        public string Symbol { get; set; }
        public AlertType AlertType { get; set; }
        public string Message { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum AlertType
    {
        PriceMovement,
        HighVolume,
        LowVolume,
        MarketOpen,
        MarketClose
    }

    public class Transaction
    {
        public string Id { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Currency { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class ProcessedTransaction
    {
        public string TransactionId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public TransactionStatus Status { get; set; }
        public DateTime ProcessedAt { get; set; }
        public decimal TransactionFee { get; set; }
    }

    public enum TransactionStatus
    {
        Pending,
        Success,
        Failed,
        Cancelled
    }

    public class FraudAlert
    {
        public string TransactionId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Reason { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class FraudCheckResult
    {
        public bool IsFraudulent { get; set; }
        public string Reason { get; set; }
    }

    public class RideRequest
    {
        public string Id { get; set; }
        public string PassengerId { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
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
        public string PassengerId { get; set; }
        public double Distance { get; set; }
        public TimeSpan EstimatedArrival { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    #endregion
}
