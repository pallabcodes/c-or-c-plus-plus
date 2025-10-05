using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Reactive.EventSourcing
{
    /// <summary>
    /// Event Sourcing patterns for enterprise applications.
    /// Used by Bloomberg for audit trails, PayPal for transaction history,
    /// and Google for state reconstruction and analytics.
    /// </summary>
    public class EventSourcingPatterns
    {
        /// <summary>
        /// Bloomberg-style audit event store with event sourcing
        /// Maintains complete audit trail for compliance and debugging
        /// </summary>
        public class AuditEventStore
        {
            private readonly List<AuditEvent> _events;
            private readonly object _lockObject = new object();

            public AuditEventStore()
            {
                _events = new List<AuditEvent>();
            }

            public void AppendEvent(AuditEvent auditEvent)
            {
                lock (_lockObject)
                {
                    auditEvent.SequenceNumber = _events.Count + 1;
                    auditEvent.Timestamp = DateTime.UtcNow;
                    _events.Add(auditEvent);
                }
            }

            public List<AuditEvent> GetEvents(string entityId, DateTime? from = null, DateTime? to = null)
            {
                lock (_lockObject)
                {
                    var query = _events.Where(e => e.EntityId == entityId);
                    
                    if (from.HasValue)
                        query = query.Where(e => e.Timestamp >= from.Value);
                    
                    if (to.HasValue)
                        query = query.Where(e => e.Timestamp <= to.Value);
                    
                    return query.OrderBy(e => e.SequenceNumber).ToList();
                }
            }

            public List<AuditEvent> GetEventsByType(string eventType, DateTime? from = null, DateTime? to = null)
            {
                lock (_lockObject)
                {
                    var query = _events.Where(e => e.EventType == eventType);
                    
                    if (from.HasValue)
                        query = query.Where(e => e.Timestamp >= from.Value);
                    
                    if (to.HasValue)
                        query = query.Where(e => e.Timestamp <= to.Value);
                    
                    return query.OrderBy(e => e.SequenceNumber).ToList();
                }
            }

            public AuditEvent GetEvent(long sequenceNumber)
            {
                lock (_lockObject)
                {
                    return _events.FirstOrDefault(e => e.SequenceNumber == sequenceNumber);
                }
            }

            public int GetEventCount()
            {
                lock (_lockObject)
                {
                    return _events.Count;
                }
            }
        }

        /// <summary>
        /// PayPal-style transaction event store with state reconstruction
        /// Rebuilds transaction state from events for consistency
        /// </summary>
        public class TransactionEventStore
        {
            private readonly List<TransactionEvent> _events;
            private readonly object _lockObject = new object();

            public TransactionEventStore()
            {
                _events = new List<TransactionEvent>();
            }

            public void AppendEvent(TransactionEvent transactionEvent)
            {
                lock (_lockObject)
                {
                    transactionEvent.EventId = Guid.NewGuid();
                    transactionEvent.Timestamp = DateTime.UtcNow;
                    _events.Add(transactionEvent);
                }
            }

            public TransactionState RebuildState(string transactionId)
            {
                lock (_lockObject)
                {
                    var transactionEvents = _events
                        .Where(e => e.TransactionId == transactionId)
                        .OrderBy(e => e.Timestamp)
                        .ToList();

                    var state = new TransactionState { TransactionId = transactionId };

                    foreach (var evt in transactionEvents)
                    {
                        ApplyEvent(state, evt);
                    }

                    return state;
                }
            }

            public List<TransactionEvent> GetTransactionEvents(string transactionId)
            {
                lock (_lockObject)
                {
                    return _events
                        .Where(e => e.TransactionId == transactionId)
                        .OrderBy(e => e.Timestamp)
                        .ToList();
                }
            }

            private void ApplyEvent(TransactionState state, TransactionEvent evt)
            {
                switch (evt.EventType)
                {
                    case TransactionEventType.Created:
                        state.Amount = evt.Amount;
                        state.CustomerId = evt.CustomerId;
                        state.Currency = evt.Currency;
                        state.Status = TransactionStatus.Pending;
                        break;
                    case TransactionEventType.Processing:
                        state.Status = TransactionStatus.Processing;
                        break;
                    case TransactionEventType.Completed:
                        state.Status = TransactionStatus.Completed;
                        state.ProcessedAt = evt.Timestamp;
                        break;
                    case TransactionEventType.Failed:
                        state.Status = TransactionStatus.Failed;
                        state.FailureReason = evt.FailureReason;
                        break;
                    case TransactionEventType.Cancelled:
                        state.Status = TransactionStatus.Cancelled;
                        state.CancelledAt = evt.Timestamp;
                        break;
                }
            }
        }

        /// <summary>
        /// Google-style user behavior event store for analytics
        /// Tracks user interactions for recommendation and analytics
        /// </summary>
        public class UserBehaviorEventStore
        {
            private readonly List<UserBehaviorEvent> _events;
            private readonly object _lockObject = new object();

            public UserBehaviorEventStore()
            {
                _events = new List<UserBehaviorEvent>();
            }

            public void TrackBehavior(UserBehaviorEvent behaviorEvent)
            {
                lock (_lockObject)
                {
                    behaviorEvent.EventId = Guid.NewGuid();
                    behaviorEvent.Timestamp = DateTime.UtcNow;
                    _events.Add(behaviorEvent);
                }
            }

            public UserProfile RebuildUserProfile(string userId)
            {
                lock (_lockObject)
                {
                    var userEvents = _events
                        .Where(e => e.UserId == userId)
                        .OrderBy(e => e.Timestamp)
                        .ToList();

                    var profile = new UserProfile { UserId = userId };

                    foreach (var evt in userEvents)
                    {
                        ApplyBehaviorEvent(profile, evt);
                    }

                    return profile;
                }
            }

            public List<UserBehaviorEvent> GetUserEvents(string userId, DateTime? from = null, DateTime? to = null)
            {
                lock (_lockObject)
                {
                    var query = _events.Where(e => e.UserId == userId);
                    
                    if (from.HasValue)
                        query = query.Where(e => e.Timestamp >= from.Value);
                    
                    if (to.HasValue)
                        query = query.Where(e => e.Timestamp <= to.Value);
                    
                    return query.OrderBy(e => e.Timestamp).ToList();
                }
            }

            public List<string> GetPopularProducts(DateTime? from = null, DateTime? to = null)
            {
                lock (_lockObject)
                {
                    var query = _events.Where(e => e.EventType == BehaviorEventType.View || e.EventType == BehaviorEventType.Purchase);
                    
                    if (from.HasValue)
                        query = query.Where(e => e.Timestamp >= from.Value);
                    
                    if (to.HasValue)
                        query = query.Where(e => e.Timestamp <= to.Value);
                    
                    return query
                        .GroupBy(e => e.ProductId)
                        .OrderByDescending(g => g.Count())
                        .Take(10)
                        .Select(g => g.Key)
                        .ToList();
                }
            }

            private void ApplyBehaviorEvent(UserProfile profile, UserBehaviorEvent evt)
            {
                switch (evt.EventType)
                {
                    case BehaviorEventType.View:
                        profile.ViewedProducts.Add(evt.ProductId);
                        break;
                    case BehaviorEventType.Purchase:
                        profile.PurchasedProducts.Add(evt.ProductId);
                        profile.TotalSpent += evt.Amount ?? 0;
                        break;
                    case BehaviorEventType.Search:
                        profile.SearchQueries.Add(evt.SearchQuery);
                        break;
                }
            }
        }

        /// <summary>
        /// Amazon-style inventory event store with state reconstruction
        /// Tracks inventory changes for accurate stock management
        /// </summary>
        public class InventoryEventStore
        {
            private readonly List<InventoryEvent> _events;
            private readonly object _lockObject = new object();

            public InventoryEventStore()
            {
                _events = new List<InventoryEvent>();
            }

            public void RecordEvent(InventoryEvent inventoryEvent)
            {
                lock (_lockObject)
                {
                    inventoryEvent.EventId = Guid.NewGuid();
                    inventoryEvent.Timestamp = DateTime.UtcNow;
                    _events.Add(inventoryEvent);
                }
            }

            public InventoryState RebuildInventoryState(string productId)
            {
                lock (_lockObject)
                {
                    var productEvents = _events
                        .Where(e => e.ProductId == productId)
                        .OrderBy(e => e.Timestamp)
                        .ToList();

                    var state = new InventoryState { ProductId = productId };

                    foreach (var evt in productEvents)
                    {
                        ApplyInventoryEvent(state, evt);
                    }

                    return state;
                }
            }

            public List<InventoryEvent> GetInventoryEvents(string productId, DateTime? from = null, DateTime? to = null)
            {
                lock (_lockObject)
                {
                    var query = _events.Where(e => e.ProductId == productId);
                    
                    if (from.HasValue)
                        query = query.Where(e => e.Timestamp >= from.Value);
                    
                    if (to.HasValue)
                        query = query.Where(e => e.Timestamp <= to.Value);
                    
                    return query.OrderBy(e => e.Timestamp).ToList();
                }
            }

            public List<InventoryEvent> GetLowStockEvents(int threshold = 10)
            {
                lock (_lockObject)
                {
                    return _events
                        .Where(e => e.EventType == InventoryEventType.StockUpdate && e.Quantity <= threshold)
                        .OrderBy(e => e.Timestamp)
                        .ToList();
                }
            }

            private void ApplyInventoryEvent(InventoryState state, InventoryEvent evt)
            {
                switch (evt.EventType)
                {
                    case InventoryEventType.StockUpdate:
                        state.Quantity = evt.Quantity;
                        break;
                    case InventoryEventType.Reserved:
                        state.ReservedQuantity += evt.Quantity;
                        break;
                    case InventoryEventType.Released:
                        state.ReservedQuantity -= evt.Quantity;
                        break;
                    case InventoryEventType.Sold:
                        state.Quantity -= evt.Quantity;
                        state.SoldQuantity += evt.Quantity;
                        break;
                }
            }
        }

        /// <summary>
        /// Uber-style ride event store with state reconstruction
        /// Tracks ride lifecycle for analytics and customer service
        /// </summary>
        public class RideEventStore
        {
            private readonly List<RideEvent> _events;
            private readonly object _lockObject = new object();

            public RideEventStore()
            {
                _events = new List<RideEvent>();
            }

            public void RecordEvent(RideEvent rideEvent)
            {
                lock (_lockObject)
                {
                    rideEvent.EventId = Guid.NewGuid();
                    rideEvent.Timestamp = DateTime.UtcNow;
                    _events.Add(rideEvent);
                }
            }

            public RideState RebuildRideState(string rideId)
            {
                lock (_lockObject)
                {
                    var rideEvents = _events
                        .Where(e => e.RideId == rideId)
                        .OrderBy(e => e.Timestamp)
                        .ToList();

                    var state = new RideState { RideId = rideId };

                    foreach (var evt in rideEvents)
                    {
                        ApplyRideEvent(state, evt);
                    }

                    return state;
                }
            }

            public List<RideEvent> GetRideEvents(string rideId)
            {
                lock (_lockObject)
                {
                    return _events
                        .Where(e => e.RideId == rideId)
                        .OrderBy(e => e.Timestamp)
                        .ToList();
                }
            }

            public List<RideEvent> GetDriverEvents(string driverId, DateTime? from = null, DateTime? to = null)
            {
                lock (_lockObject)
                {
                    var query = _events.Where(e => e.DriverId == driverId);
                    
                    if (from.HasValue)
                        query = query.Where(e => e.Timestamp >= from.Value);
                    
                    if (to.HasValue)
                        query = query.Where(e => e.Timestamp <= to.Value);
                    
                    return query.OrderBy(e => e.Timestamp).ToList();
                }
            }

            private void ApplyRideEvent(RideState state, RideEvent evt)
            {
                switch (evt.EventType)
                {
                    case RideEventType.Requested:
                        state.PassengerId = evt.PassengerId;
                        state.PickupLocation = evt.PickupLocation;
                        state.Destination = evt.Destination;
                        state.Status = RideStatus.Requested;
                        break;
                    case RideEventType.Accepted:
                        state.DriverId = evt.DriverId;
                        state.Status = RideStatus.Accepted;
                        break;
                    case RideEventType.Started:
                        state.Status = RideStatus.InProgress;
                        state.StartTime = evt.Timestamp;
                        break;
                    case RideEventType.Completed:
                        state.Status = RideStatus.Completed;
                        state.EndTime = evt.Timestamp;
                        state.Fare = evt.Fare;
                        break;
                    case RideEventType.Cancelled:
                        state.Status = RideStatus.Cancelled;
                        state.CancellationReason = evt.CancellationReason;
                        break;
                }
            }
        }
    }

    #region Supporting Classes

    public class AuditEvent
    {
        public long SequenceNumber { get; set; }
        public string EntityId { get; set; }
        public string EventType { get; set; }
        public string Description { get; set; }
        public string UserId { get; set; }
        public string Data { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class TransactionEvent
    {
        public Guid EventId { get; set; }
        public string TransactionId { get; set; }
        public TransactionEventType EventType { get; set; }
        public decimal Amount { get; set; }
        public string CustomerId { get; set; }
        public string Currency { get; set; }
        public string FailureReason { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum TransactionEventType
    {
        Created,
        Processing,
        Completed,
        Failed,
        Cancelled
    }

    public class TransactionState
    {
        public string TransactionId { get; set; }
        public decimal Amount { get; set; }
        public string CustomerId { get; set; }
        public string Currency { get; set; }
        public TransactionStatus Status { get; set; }
        public DateTime? ProcessedAt { get; set; }
        public string FailureReason { get; set; }
        public DateTime? CancelledAt { get; set; }
    }

    public enum TransactionStatus
    {
        Pending,
        Processing,
        Completed,
        Failed,
        Cancelled
    }

    public class UserBehaviorEvent
    {
        public Guid EventId { get; set; }
        public string UserId { get; set; }
        public string ProductId { get; set; }
        public BehaviorEventType EventType { get; set; }
        public decimal? Amount { get; set; }
        public string SearchQuery { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum BehaviorEventType
    {
        View,
        Purchase,
        Search,
        AddToCart
    }

    public class UserProfile
    {
        public string UserId { get; set; }
        public List<string> ViewedProducts { get; set; } = new();
        public List<string> PurchasedProducts { get; set; } = new();
        public List<string> SearchQueries { get; set; } = new();
        public decimal TotalSpent { get; set; }
    }

    public class InventoryEvent
    {
        public Guid EventId { get; set; }
        public string ProductId { get; set; }
        public InventoryEventType EventType { get; set; }
        public int Quantity { get; set; }
        public string OrderId { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum InventoryEventType
    {
        StockUpdate,
        Reserved,
        Released,
        Sold
    }

    public class InventoryState
    {
        public string ProductId { get; set; }
        public int Quantity { get; set; }
        public int ReservedQuantity { get; set; }
        public int SoldQuantity { get; set; }
    }

    public class RideEvent
    {
        public Guid EventId { get; set; }
        public string RideId { get; set; }
        public string PassengerId { get; set; }
        public string DriverId { get; set; }
        public RideEventType EventType { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
        public decimal? Fare { get; set; }
        public string CancellationReason { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public enum RideEventType
    {
        Requested,
        Accepted,
        Started,
        Completed,
        Cancelled
    }

    public class RideState
    {
        public string RideId { get; set; }
        public string PassengerId { get; set; }
        public string DriverId { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
        public RideStatus Status { get; set; }
        public DateTime? StartTime { get; set; }
        public DateTime? EndTime { get; set; }
        public decimal? Fare { get; set; }
        public string CancellationReason { get; set; }
    }

    public enum RideStatus
    {
        Requested,
        Accepted,
        InProgress,
        Completed,
        Cancelled
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    #endregion
}
