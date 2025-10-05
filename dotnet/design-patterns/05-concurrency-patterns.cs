using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.DesignPatterns.Concurrency
{
    /// <summary>
    /// Advanced concurrency patterns used by top-tier companies.
    /// Covers producer-consumer, reader-writer locks, actors, pipelines, and work stealing.
    /// </summary>
    public class ConcurrencyPatterns
    {
        /// <summary>
        /// Google-style producer-consumer pattern for search indexing
        /// Handles millions of documents with controlled concurrency
        /// </summary>
        public class SearchIndexingProducerConsumer
        {
            private readonly BlockingCollection<Document> _documentQueue;
            private readonly BlockingCollection<IndexedDocument> _indexedQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task[] _producerTasks;
            private readonly Task[] _consumerTasks;

            public SearchIndexingProducerConsumer(int producerCount = 2, int consumerCount = Environment.ProcessorCount)
            {
                _documentQueue = new BlockingCollection<Document>(1000);
                _indexedQueue = new BlockingCollection<IndexedDocument>(1000);
                _cancellationTokenSource = new CancellationTokenSource();

                _producerTasks = new Task[producerCount];
                for (int i = 0; i < producerCount; i++)
                {
                    _producerTasks[i] = Task.Run(() => ProducerLoop(_cancellationTokenSource.Token));
                }

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

            private async Task ProducerLoop(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var document in _documentQueue.GetConsumingEnumerable(cancellationToken))
                    {
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

            public void Complete()
            {
                _documentQueue.CompleteAdding();
                Task.WaitAll(_producerTasks);
                _indexedQueue.CompleteAdding();
                Task.WaitAll(_consumerTasks);
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
        /// Bloomberg-style reader-writer lock pattern for market data
        /// Allows multiple readers or single writer for high-frequency data
        /// </summary>
        public class MarketDataCache
        {
            private readonly ReaderWriterLockSlim _lock = new ReaderWriterLockSlim();
            private readonly Dictionary<string, MarketData> _cache = new Dictionary<string, MarketData>();
            private readonly Dictionary<string, DateTime> _lastUpdated = new Dictionary<string, DateTime>();

            public MarketData GetMarketData(string symbol)
            {
                _lock.EnterReadLock();
                try
                {
                    return _cache.TryGetValue(symbol, out var data) ? data : null;
                }
                finally
                {
                    _lock.ExitReadLock();
                }
            }

            public void UpdateMarketData(string symbol, MarketData data)
            {
                _lock.EnterWriteLock();
                try
                {
                    _cache[symbol] = data;
                    _lastUpdated[symbol] = DateTime.UtcNow;
                }
                finally
                {
                    _lock.ExitWriteLock();
                }
            }

            public Dictionary<string, MarketData> GetAllMarketData()
            {
                _lock.EnterReadLock();
                try
                {
                    return new Dictionary<string, MarketData>(_cache);
                }
                finally
                {
                    _lock.ExitReadLock();
                }
            }

            public void ClearStaleData(TimeSpan maxAge)
            {
                _lock.EnterWriteLock();
                try
                {
                    var cutoff = DateTime.UtcNow - maxAge;
                    var staleKeys = _lastUpdated
                        .Where(kvp => kvp.Value < cutoff)
                        .Select(kvp => kvp.Key)
                        .ToList();

                    foreach (var key in staleKeys)
                    {
                        _cache.Remove(key);
                        _lastUpdated.Remove(key);
                    }
                }
                finally
                {
                    _lock.ExitWriteLock();
                }
            }

            public void Dispose()
            {
                _lock?.Dispose();
            }
        }

        /// <summary>
        /// PayPal-style actor pattern for transaction processing
        /// Processes transactions asynchronously with message passing
        /// </summary>
        public class TransactionActor
        {
            private readonly BlockingCollection<TransactionMessage> _messageQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task _processingTask;
            private readonly Dictionary<string, Transaction> _transactions = new Dictionary<string, Transaction>();

            public TransactionActor()
            {
                _messageQueue = new BlockingCollection<TransactionMessage>();
                _cancellationTokenSource = new CancellationTokenSource();
                _processingTask = Task.Run(() => ProcessMessages(_cancellationTokenSource.Token));
            }

            public void SendMessage(TransactionMessage message)
            {
                if (!_messageQueue.IsAddingCompleted)
                {
                    _messageQueue.Add(message);
                }
            }

            private async Task ProcessMessages(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var message in _messageQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        await ProcessMessage(message);
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
            }

            private async Task ProcessMessage(TransactionMessage message)
            {
                switch (message.Type)
                {
                    case MessageType.CreateTransaction:
                        await HandleCreateTransaction(message);
                        break;
                    case MessageType.ProcessTransaction:
                        await HandleProcessTransaction(message);
                        break;
                    case MessageType.CancelTransaction:
                        await HandleCancelTransaction(message);
                        break;
                    case MessageType.GetTransaction:
                        await HandleGetTransaction(message);
                        break;
                }
            }

            private async Task HandleCreateTransaction(TransactionMessage message)
            {
                var transaction = new Transaction
                {
                    Id = message.TransactionId,
                    CustomerId = message.CustomerId,
                    Amount = message.Amount,
                    Status = TransactionStatus.Pending,
                    CreatedAt = DateTime.UtcNow
                };

                _transactions[transaction.Id] = transaction;
                await Task.Delay(10); // Simulate processing
            }

            private async Task HandleProcessTransaction(TransactionMessage message)
            {
                if (_transactions.TryGetValue(message.TransactionId, out var transaction))
                {
                    await Task.Delay(50); // Simulate processing
                    transaction.Status = TransactionStatus.Success;
                    transaction.ProcessedAt = DateTime.UtcNow;
                }
            }

            private async Task HandleCancelTransaction(TransactionMessage message)
            {
                if (_transactions.TryGetValue(message.TransactionId, out var transaction))
                {
                    await Task.Delay(10); // Simulate processing
                    transaction.Status = TransactionStatus.Cancelled;
                    transaction.CancelledAt = DateTime.UtcNow;
                }
            }

            private async Task HandleGetTransaction(TransactionMessage message)
            {
                if (_transactions.TryGetValue(message.TransactionId, out var transaction))
                {
                    // In a real implementation, this would send the transaction back
                    await Task.Delay(5); // Simulate processing
                }
            }

            public void Complete()
            {
                _messageQueue.CompleteAdding();
                _processingTask.Wait();
            }

            public void Dispose()
            {
                _cancellationTokenSource.Cancel();
                Complete();
                _cancellationTokenSource.Dispose();
                _messageQueue.Dispose();
            }
        }

        /// <summary>
        /// Uber-style pipeline pattern for ride matching
        /// Processes ride requests through multiple stages
        /// </summary>
        public class RideMatchingPipeline
        {
            private readonly BlockingCollection<RideRequest> _inputQueue;
            private readonly BlockingCollection<ValidatedRequest> _validatedQueue;
            private readonly BlockingCollection<MatchedRide> _matchedQueue;
            private readonly BlockingCollection<ConfirmedRide> _confirmedQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task[] _stageTasks;

            public RideMatchingPipeline()
            {
                _inputQueue = new BlockingCollection<RideRequest>(1000);
                _validatedQueue = new BlockingCollection<ValidatedRequest>(1000);
                _matchedQueue = new BlockingCollection<MatchedRide>(1000);
                _confirmedQueue = new BlockingCollection<ConfirmedRide>(1000);
                _cancellationTokenSource = new CancellationTokenSource();

                _stageTasks = new Task[4];
                _stageTasks[0] = Task.Run(() => ValidationStage(_cancellationTokenSource.Token));
                _stageTasks[1] = Task.Run(() => MatchingStage(_cancellationTokenSource.Token));
                _stageTasks[2] = Task.Run(() => ConfirmationStage(_cancellationTokenSource.Token));
                _stageTasks[3] = Task.Run(() => NotificationStage(_cancellationTokenSource.Token));
            }

            public void AddRideRequest(RideRequest request)
            {
                if (!_inputQueue.IsAddingCompleted)
                {
                    _inputQueue.Add(request);
                }
            }

            public ConfirmedRide GetConfirmedRide()
            {
                return _confirmedQueue.Take();
            }

            private async Task ValidationStage(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var request in _inputQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        var validatedRequest = await ValidateRequest(request);
                        if (validatedRequest != null)
                        {
                            _validatedQueue.Add(validatedRequest);
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
                finally
                {
                    _validatedQueue.CompleteAdding();
                }
            }

            private async Task MatchingStage(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var request in _validatedQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        var matchedRide = await FindDriver(request);
                        if (matchedRide != null)
                        {
                            _matchedQueue.Add(matchedRide);
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
                finally
                {
                    _matchedQueue.CompleteAdding();
                }
            }

            private async Task ConfirmationStage(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var match in _matchedQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        var confirmedRide = await ConfirmRide(match);
                        if (confirmedRide != null)
                        {
                            _confirmedQueue.Add(confirmedRide);
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
                finally
                {
                    _confirmedQueue.CompleteAdding();
                }
            }

            private async Task NotificationStage(CancellationToken cancellationToken)
            {
                try
                {
                    foreach (var ride in _confirmedQueue.GetConsumingEnumerable(cancellationToken))
                    {
                        await SendNotification(ride);
                    }
                }
                catch (OperationCanceledException)
                {
                    // Graceful shutdown
                }
            }

            private async Task<ValidatedRequest> ValidateRequest(RideRequest request)
            {
                await Task.Delay(10); // Simulate validation
                
                // Simple validation logic
                if (string.IsNullOrEmpty(request.PassengerId) || request.PickupLocation == null)
                {
                    return null;
                }

                return new ValidatedRequest
                {
                    RequestId = request.Id,
                    PassengerId = request.PassengerId,
                    PickupLocation = request.PickupLocation,
                    Destination = request.Destination,
                    ValidatedAt = DateTime.UtcNow
                };
            }

            private async Task<MatchedRide> FindDriver(ValidatedRequest request)
            {
                await Task.Delay(50); // Simulate driver search
                
                // Simple matching logic
                return new MatchedRide
                {
                    RequestId = request.RequestId,
                    DriverId = Guid.NewGuid().ToString(),
                    PassengerId = request.PassengerId,
                    EstimatedArrival = TimeSpan.FromMinutes(5),
                    MatchedAt = DateTime.UtcNow
                };
            }

            private async Task<ConfirmedRide> ConfirmRide(MatchedRide match)
            {
                await Task.Delay(20); // Simulate confirmation
                
                return new ConfirmedRide
                {
                    RequestId = match.RequestId,
                    DriverId = match.DriverId,
                    PassengerId = match.PassengerId,
                    EstimatedArrival = match.EstimatedArrival,
                    ConfirmedAt = DateTime.UtcNow
                };
            }

            private async Task SendNotification(ConfirmedRide ride)
            {
                await Task.Delay(30); // Simulate notification
                Console.WriteLine($"Ride confirmed for passenger {ride.PassengerId} with driver {ride.DriverId}");
            }

            public void Complete()
            {
                _inputQueue.CompleteAdding();
                Task.WaitAll(_stageTasks);
            }

            public void Dispose()
            {
                _cancellationTokenSource.Cancel();
                Complete();
                _cancellationTokenSource.Dispose();
                _inputQueue.Dispose();
                _validatedQueue.Dispose();
                _matchedQueue.Dispose();
                _confirmedQueue.Dispose();
            }
        }

        /// <summary>
        /// Amazon-style work stealing pattern for order processing
        /// Distributes work across multiple threads efficiently
        /// </summary>
        public class WorkStealingOrderProcessor
        {
            private readonly ConcurrentQueue<Order> _globalQueue;
            private readonly ThreadLocal<Queue<Order>> _localQueues;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task[] _workerTasks;

            public WorkStealingOrderProcessor(int workerCount = Environment.ProcessorCount)
            {
                _globalQueue = new ConcurrentQueue<Order>();
                _localQueues = new ThreadLocal<Queue<Order>>(() => new Queue<Order>());
                _cancellationTokenSource = new CancellationTokenSource();

                _workerTasks = new Task[workerCount];
                for (int i = 0; i < workerCount; i++)
                {
                    _workerTasks[i] = Task.Run(() => WorkerLoop(_cancellationTokenSource.Token));
                }
            }

            public void AddOrder(Order order)
            {
                _globalQueue.Enqueue(order);
            }

            private async Task WorkerLoop(CancellationToken cancellationToken)
            {
                while (!cancellationToken.IsCancellationRequested)
                {
                    var order = GetNextOrder();
                    if (order != null)
                    {
                        await ProcessOrder(order);
                    }
                    else
                    {
                        await Task.Delay(10, cancellationToken); // Brief pause if no work
                    }
                }
            }

            private Order GetNextOrder()
            {
                var localQueue = _localQueues.Value;

                // Try to get work from local queue first
                if (localQueue.Count > 0)
                {
                    return localQueue.Dequeue();
                }

                // Try to steal work from global queue
                if (_globalQueue.TryDequeue(out var order))
                {
                    return order;
                }

                // Try to steal work from other threads' local queues
                return StealWorkFromOtherThreads();
            }

            private Order StealWorkFromOtherThreads()
            {
                // In a real implementation, this would access other threads' local queues
                // For simplicity, we'll just return null here
                return null;
            }

            private async Task ProcessOrder(Order order)
            {
                await Task.Delay(100); // Simulate processing
                order.Status = OrderStatus.Processed;
                order.ProcessedAt = DateTime.UtcNow;
            }

            public void Complete()
            {
                _cancellationTokenSource.Cancel();
                Task.WaitAll(_workerTasks);
            }

            public void Dispose()
            {
                _cancellationTokenSource.Dispose();
                _localQueues.Dispose();
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

    public class Transaction
    {
        public string Id { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public TransactionStatus Status { get; set; }
        public DateTime CreatedAt { get; set; }
        public DateTime ProcessedAt { get; set; }
        public DateTime CancelledAt { get; set; }
    }

    public enum TransactionStatus
    {
        Pending,
        Success,
        Failed,
        Cancelled
    }

    public class TransactionMessage
    {
        public MessageType Type { get; set; }
        public string TransactionId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
    }

    public enum MessageType
    {
        CreateTransaction,
        ProcessTransaction,
        CancelTransaction,
        GetTransaction
    }

    public class RideRequest
    {
        public string Id { get; set; }
        public string PassengerId { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
        public DateTime RequestTime { get; set; }
    }

    public class ValidatedRequest
    {
        public string RequestId { get; set; }
        public string PassengerId { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
        public DateTime ValidatedAt { get; set; }
    }

    public class MatchedRide
    {
        public string RequestId { get; set; }
        public string DriverId { get; set; }
        public string PassengerId { get; set; }
        public TimeSpan EstimatedArrival { get; set; }
        public DateTime MatchedAt { get; set; }
    }

    public class ConfirmedRide
    {
        public string RequestId { get; set; }
        public string DriverId { get; set; }
        public string PassengerId { get; set; }
        public TimeSpan EstimatedArrival { get; set; }
        public DateTime ConfirmedAt { get; set; }
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    public class Order
    {
        public string Id { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public OrderStatus Status { get; set; }
        public DateTime CreatedAt { get; set; }
        public DateTime ProcessedAt { get; set; }
    }

    public enum OrderStatus
    {
        Pending,
        Processed,
        Shipped,
        Delivered,
        Cancelled
    }

    #endregion
}
