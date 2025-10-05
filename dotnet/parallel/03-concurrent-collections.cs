using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Parallel.ConcurrentCollections
{
    /// <summary>
    /// Thread-safe concurrent collection patterns for enterprise applications.
    /// Used by Google for distributed caching, Bloomberg for real-time data,
    /// and PayPal for transaction state management.
    /// </summary>
    public class ConcurrentCollectionPatterns
    {
        /// <summary>
        /// Google-style distributed cache with concurrent dictionary
        /// Handles high-frequency read/write operations with TTL support
        /// </summary>
        public class DistributedCache<TKey, TValue>
        {
            private readonly ConcurrentDictionary<TKey, CacheEntry<TValue>> _cache;
            private readonly Timer _cleanupTimer;
            private readonly TimeSpan _defaultTtl;

            public DistributedCache(TimeSpan defaultTtl = default)
            {
                _cache = new ConcurrentDictionary<TKey, CacheEntry<TValue>>();
                _defaultTtl = defaultTtl == default ? TimeSpan.FromMinutes(30) : defaultTtl;
                
                // Cleanup expired entries every 5 minutes
                _cleanupTimer = new Timer(CleanupExpiredEntries, null, TimeSpan.FromMinutes(5), TimeSpan.FromMinutes(5));
            }

            public bool TryGet(TKey key, out TValue value)
            {
                if (_cache.TryGetValue(key, out var entry) && !entry.IsExpired)
                {
                    value = entry.Value;
                    return true;
                }

                value = default;
                return false;
            }

            public void Set(TKey key, TValue value, TimeSpan? ttl = null)
            {
                var expiration = DateTime.UtcNow.Add(ttl ?? _defaultTtl);
                var entry = new CacheEntry<TValue>(value, expiration);
                _cache.AddOrUpdate(key, entry, (k, existing) => entry);
            }

            public bool TryRemove(TKey key, out TValue value)
            {
                if (_cache.TryRemove(key, out var entry))
                {
                    value = entry.Value;
                    return true;
                }

                value = default;
                return false;
            }

            public int Count => _cache.Count;

            private void CleanupExpiredEntries(object state)
            {
                var expiredKeys = _cache
                    .Where(kvp => kvp.Value.IsExpired)
                    .Select(kvp => kvp.Key)
                    .ToList();

                foreach (var key in expiredKeys)
                {
                    _cache.TryRemove(key, out _);
                }
            }

            public void Dispose()
            {
                _cleanupTimer?.Dispose();
            }
        }

        /// <summary>
        /// Bloomberg-style real-time data aggregator with concurrent collections
        /// Aggregates market data from multiple sources with thread safety
        /// </summary>
        public class MarketDataAggregator
        {
            private readonly ConcurrentDictionary<string, MarketDataSnapshot> _snapshots;
            private readonly ConcurrentQueue<MarketDataUpdate> _updateQueue;
            private readonly CancellationTokenSource _cancellationTokenSource;
            private readonly Task _processingTask;

            public MarketDataAggregator()
            {
                _snapshots = new ConcurrentDictionary<string, MarketDataSnapshot>();
                _updateQueue = new ConcurrentQueue<MarketDataUpdate>();
                _cancellationTokenSource = new CancellationTokenSource();
                
                _processingTask = Task.Run(ProcessUpdatesAsync);
            }

            public void AddUpdate(MarketDataUpdate update)
            {
                _updateQueue.Enqueue(update);
            }

            public MarketDataSnapshot GetSnapshot(string symbol)
            {
                return _snapshots.TryGetValue(symbol, out var snapshot) ? snapshot : null;
            }

            public Dictionary<string, MarketDataSnapshot> GetAllSnapshots()
            {
                return _snapshots.ToDictionary(kvp => kvp.Key, kvp => kvp.Value);
            }

            private async Task ProcessUpdatesAsync()
            {
                while (!_cancellationTokenSource.Token.IsCancellationRequested)
                {
                    var updates = new List<MarketDataUpdate>();
                    
                    // Batch process updates for better performance
                    while (_updateQueue.TryDequeue(out var update) && updates.Count < 1000)
                    {
                        updates.Add(update);
                    }

                    if (updates.Any())
                    {
                        ProcessBatchUpdates(updates);
                    }

                    await Task.Delay(10, _cancellationTokenSource.Token);
                }
            }

            private void ProcessBatchUpdates(List<MarketDataUpdate> updates)
            {
                foreach (var update in updates)
                {
                    _snapshots.AddOrUpdate(
                        update.Symbol,
                        new MarketDataSnapshot
                        {
                            Symbol = update.Symbol,
                            Price = update.Price,
                            Volume = update.Volume,
                            LastUpdate = update.Timestamp
                        },
                        (key, existing) => new MarketDataSnapshot
                        {
                            Symbol = existing.Symbol,
                            Price = update.Price,
                            Volume = existing.Volume + update.Volume,
                            LastUpdate = update.Timestamp
                        });
                }
            }

            public void Dispose()
            {
                _cancellationTokenSource.Cancel();
                _processingTask.Wait();
                _cancellationTokenSource.Dispose();
            }
        }

        /// <summary>
        /// PayPal-style transaction state manager with concurrent bag
        /// Manages transaction states across multiple threads
        /// </summary>
        public class TransactionStateManager
        {
            private readonly ConcurrentBag<TransactionState> _completedTransactions;
            private readonly ConcurrentDictionary<string, TransactionState> _activeTransactions;
            private readonly ConcurrentQueue<TransactionEvent> _eventQueue;

            public TransactionStateManager()
            {
                _completedTransactions = new ConcurrentBag<TransactionState>();
                _activeTransactions = new ConcurrentDictionary<string, TransactionState>();
                _eventQueue = new ConcurrentQueue<TransactionEvent>();
            }

            public bool StartTransaction(string transactionId, decimal amount, string customerId)
            {
                var state = new TransactionState
                {
                    TransactionId = transactionId,
                    Amount = amount,
                    CustomerId = customerId,
                    Status = TransactionStatus.Pending,
                    CreatedAt = DateTime.UtcNow
                };

                if (_activeTransactions.TryAdd(transactionId, state))
                {
                    _eventQueue.Enqueue(new TransactionEvent
                    {
                        TransactionId = transactionId,
                        EventType = TransactionEventType.Started,
                        Timestamp = DateTime.UtcNow
                    });
                    return true;
                }

                return false;
            }

            public bool UpdateTransactionStatus(string transactionId, TransactionStatus status)
            {
                if (_activeTransactions.TryGetValue(transactionId, out var state))
                {
                    state.Status = status;
                    state.LastUpdated = DateTime.UtcNow;

                    if (status == TransactionStatus.Completed || status == TransactionStatus.Failed)
                    {
                        if (_activeTransactions.TryRemove(transactionId, out var completedState))
                        {
                            _completedTransactions.Add(completedState);
                        }
                    }

                    _eventQueue.Enqueue(new TransactionEvent
                    {
                        TransactionId = transactionId,
                        EventType = GetEventType(status),
                        Timestamp = DateTime.UtcNow
                    });

                    return true;
                }

                return false;
            }

            public TransactionState GetTransactionState(string transactionId)
            {
                if (_activeTransactions.TryGetValue(transactionId, out var activeState))
                    return activeState;

                // Check completed transactions (this is O(n) but acceptable for small sets)
                return _completedTransactions.FirstOrDefault(t => t.TransactionId == transactionId);
            }

            public List<TransactionEvent> GetEvents(string transactionId)
            {
                return _eventQueue
                    .Where(e => e.TransactionId == transactionId)
                    .OrderBy(e => e.Timestamp)
                    .ToList();
            }

            private TransactionEventType GetEventType(TransactionStatus status)
            {
                return status switch
                {
                    TransactionStatus.Pending => TransactionEventType.Started,
                    TransactionStatus.Processing => TransactionEventType.Processing,
                    TransactionStatus.Completed => TransactionEventType.Completed,
                    TransactionStatus.Failed => TransactionEventType.Failed,
                    _ => TransactionEventType.Unknown
                };
            }
        }

        /// <summary>
        /// Uber-style driver location tracker with concurrent collections
        /// Tracks real-time driver locations with thread safety
        /// </summary>
        public class DriverLocationTracker
        {
            private readonly ConcurrentDictionary<string, DriverLocation> _driverLocations;
            private readonly ConcurrentQueue<LocationUpdate> _locationUpdates;
            private readonly Timer _cleanupTimer;

            public DriverLocationTracker()
            {
                _driverLocations = new ConcurrentDictionary<string, DriverLocation>();
                _locationUpdates = new ConcurrentQueue<LocationUpdate>();
                
                // Cleanup stale locations every minute
                _cleanupTimer = new Timer(CleanupStaleLocations, null, TimeSpan.FromMinutes(1), TimeSpan.FromMinutes(1));
            }

            public void UpdateDriverLocation(string driverId, double latitude, double longitude, DateTime timestamp)
            {
                var location = new DriverLocation
                {
                    DriverId = driverId,
                    Latitude = latitude,
                    Longitude = longitude,
                    LastUpdate = timestamp
                };

                _driverLocations.AddOrUpdate(driverId, location, (key, existing) => location);
                
                _locationUpdates.Enqueue(new LocationUpdate
                {
                    DriverId = driverId,
                    Latitude = latitude,
                    Longitude = longitude,
                    Timestamp = timestamp
                });
            }

            public List<DriverLocation> GetNearbyDrivers(double latitude, double longitude, double radiusKm)
            {
                return _driverLocations.Values
                    .Where(driver => CalculateDistance(latitude, longitude, driver.Latitude, driver.Longitude) <= radiusKm)
                    .OrderBy(driver => CalculateDistance(latitude, longitude, driver.Latitude, driver.Longitude))
                    .ToList();
            }

            public DriverLocation GetDriverLocation(string driverId)
            {
                return _driverLocations.TryGetValue(driverId, out var location) ? location : null;
            }

            private void CleanupStaleLocations(object state)
            {
                var cutoff = DateTime.UtcNow.AddMinutes(-5); // Remove locations older than 5 minutes
                var staleDrivers = _driverLocations
                    .Where(kvp => kvp.Value.LastUpdate < cutoff)
                    .Select(kvp => kvp.Key)
                    .ToList();

                foreach (var driverId in staleDrivers)
                {
                    _driverLocations.TryRemove(driverId, out _);
                }
            }

            private double CalculateDistance(double lat1, double lon1, double lat2, double lon2)
            {
                // Simplified distance calculation (in production, use Haversine formula)
                return Math.Sqrt(Math.Pow(lat1 - lat2, 2) + Math.Pow(lon1 - lon2, 2)) * 111; // Rough km conversion
            }

            public void Dispose()
            {
                _cleanupTimer?.Dispose();
            }
        }

        /// <summary>
        /// Amazon-style product inventory manager with concurrent collections
        /// Manages inventory levels across multiple threads
        /// </summary>
        public class InventoryManager
        {
            private readonly ConcurrentDictionary<string, InventoryItem> _inventory;
            private readonly ConcurrentQueue<InventoryUpdate> _updateQueue;
            private readonly object _lockObject = new object();

            public InventoryManager()
            {
                _inventory = new ConcurrentDictionary<string, InventoryItem>();
                _updateQueue = new ConcurrentQueue<InventoryUpdate>();
            }

            public bool ReserveItem(string productId, int quantity, string orderId)
            {
                return _inventory.AddOrUpdate(
                    productId,
                    new InventoryItem
                    {
                        ProductId = productId,
                        AvailableQuantity = 0,
                        ReservedQuantity = quantity,
                        LastUpdated = DateTime.UtcNow
                    },
                    (key, existing) =>
                    {
                        lock (_lockObject)
                        {
                            if (existing.AvailableQuantity >= quantity)
                            {
                                existing.AvailableQuantity -= quantity;
                                existing.ReservedQuantity += quantity;
                                existing.LastUpdated = DateTime.UtcNow;
                                
                                _updateQueue.Enqueue(new InventoryUpdate
                                {
                                    ProductId = productId,
                                    ChangeType = InventoryChangeType.Reserved,
                                    Quantity = quantity,
                                    OrderId = orderId,
                                    Timestamp = DateTime.UtcNow
                                });
                                
                                return existing;
                            }
                        }
                        return existing; // No change if insufficient inventory
                    }).AvailableQuantity >= 0;
            }

            public bool ReleaseReservation(string productId, int quantity, string orderId)
            {
                if (_inventory.TryGetValue(productId, out var item))
                {
                    lock (_lockObject)
                    {
                        if (item.ReservedQuantity >= quantity)
                        {
                            item.ReservedQuantity -= quantity;
                            item.AvailableQuantity += quantity;
                            item.LastUpdated = DateTime.UtcNow;
                            
                            _updateQueue.Enqueue(new InventoryUpdate
                            {
                                ProductId = productId,
                                ChangeType = InventoryChangeType.Released,
                                Quantity = quantity,
                                OrderId = orderId,
                                Timestamp = DateTime.UtcNow
                            });
                            
                            return true;
                        }
                    }
                }
                return false;
            }

            public void AddStock(string productId, int quantity)
            {
                _inventory.AddOrUpdate(
                    productId,
                    new InventoryItem
                    {
                        ProductId = productId,
                        AvailableQuantity = quantity,
                        ReservedQuantity = 0,
                        LastUpdated = DateTime.UtcNow
                    },
                    (key, existing) =>
                    {
                        lock (_lockObject)
                        {
                            existing.AvailableQuantity += quantity;
                            existing.LastUpdated = DateTime.UtcNow;
                        }
                        return existing;
                    });

                _updateQueue.Enqueue(new InventoryUpdate
                {
                    ProductId = productId,
                    ChangeType = InventoryChangeType.Added,
                    Quantity = quantity,
                    Timestamp = DateTime.UtcNow
                });
            }

            public InventoryItem GetInventory(string productId)
            {
                return _inventory.TryGetValue(productId, out var item) ? item : null;
            }

            public List<InventoryUpdate> GetRecentUpdates(TimeSpan timeWindow)
            {
                var cutoff = DateTime.UtcNow.Subtract(timeWindow);
                return _updateQueue
                    .Where(update => update.Timestamp >= cutoff)
                    .OrderBy(update => update.Timestamp)
                    .ToList();
            }
        }
    }

    #region Supporting Classes

    public class CacheEntry<T>
    {
        public T Value { get; }
        public DateTime Expiration { get; }
        public bool IsExpired => DateTime.UtcNow > Expiration;

        public CacheEntry(T value, DateTime expiration)
        {
            Value = value;
            Expiration = expiration;
        }
    }

    public class MarketDataSnapshot
    {
        public string Symbol { get; set; }
        public decimal Price { get; set; }
        public decimal Volume { get; set; }
        public DateTime LastUpdate { get; set; }
    }

    public class MarketDataUpdate
    {
        public string Symbol { get; set; }
        public decimal Price { get; set; }
        public decimal Volume { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class TransactionState
    {
        public string TransactionId { get; set; }
        public decimal Amount { get; set; }
        public string CustomerId { get; set; }
        public TransactionStatus Status { get; set; }
        public DateTime CreatedAt { get; set; }
        public DateTime LastUpdated { get; set; }
    }

    public enum TransactionStatus
    {
        Pending,
        Processing,
        Completed,
        Failed,
        Cancelled
    }

    public class TransactionEvent
    {
        public string TransactionId { get; set; }
        public TransactionEventType EventType { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum TransactionEventType
    {
        Started,
        Processing,
        Completed,
        Failed,
        Unknown
    }

    public class DriverLocation
    {
        public string DriverId { get; set; }
        public double Latitude { get; set; }
        public double Longitude { get; set; }
        public DateTime LastUpdate { get; set; }
    }

    public class LocationUpdate
    {
        public string DriverId { get; set; }
        public double Latitude { get; set; }
        public double Longitude { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class InventoryItem
    {
        public string ProductId { get; set; }
        public int AvailableQuantity { get; set; }
        public int ReservedQuantity { get; set; }
        public DateTime LastUpdated { get; set; }
    }

    public class InventoryUpdate
    {
        public string ProductId { get; set; }
        public InventoryChangeType ChangeType { get; set; }
        public int Quantity { get; set; }
        public string OrderId { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum InventoryChangeType
    {
        Added,
        Reserved,
        Released,
        Sold
    }

    #endregion
}
