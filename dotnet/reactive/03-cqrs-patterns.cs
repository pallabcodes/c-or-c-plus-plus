using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.Reactive.CQRSPatterns
{
    /// <summary>
    /// CQRS (Command Query Responsibility Segregation) patterns for enterprise applications.
    /// Used by Bloomberg for trading systems, PayPal for payment processing,
    /// and Google for scalable data operations.
    /// </summary>
    public class CQRSPatterns
    {
        /// <summary>
        /// Bloomberg-style trading command handler
        /// Handles trading commands with validation and event publishing
        /// </summary>
        public class TradingCommandHandler
        {
            private readonly IEventStore _eventStore;
            private readonly IEventPublisher _eventPublisher;

            public TradingCommandHandler(IEventStore eventStore, IEventPublisher eventPublisher)
            {
                _eventStore = eventStore;
                _eventPublisher = eventPublisher;
            }

            public async Task<CommandResult> HandleAsync(PlaceOrderCommand command, CancellationToken cancellationToken = default)
            {
                try
                {
                    // Validate command
                    var validationResult = await ValidateCommandAsync(command, cancellationToken);
                    if (!validationResult.IsValid)
                    {
                        return CommandResult.Failure(validationResult.Errors);
                    }

                    // Create order event
                    var orderEvent = new OrderPlacedEvent
                    {
                        OrderId = command.OrderId,
                        Symbol = command.Symbol,
                        Quantity = command.Quantity,
                        Price = command.Price,
                        OrderType = command.OrderType,
                        TraderId = command.TraderId,
                        Timestamp = DateTime.UtcNow
                    };

                    // Store event
                    await _eventStore.AppendEventAsync(orderEvent, cancellationToken);

                    // Publish event
                    await _eventPublisher.PublishAsync(orderEvent, cancellationToken);

                    return CommandResult.Success(command.OrderId);
                }
                catch (Exception ex)
                {
                    return CommandResult.Failure(new[] { ex.Message });
                }
            }

            public async Task<CommandResult> HandleAsync(CancelOrderCommand command, CancellationToken cancellationToken = default)
            {
                try
                {
                    // Check if order exists and can be cancelled
                    var orderState = await GetOrderStateAsync(command.OrderId, cancellationToken);
                    if (orderState == null)
                    {
                        return CommandResult.Failure(new[] { "Order not found" });
                    }

                    if (orderState.Status != OrderStatus.Pending)
                    {
                        return CommandResult.Failure(new[] { "Order cannot be cancelled" });
                    }

                    // Create cancellation event
                    var cancelEvent = new OrderCancelledEvent
                    {
                        OrderId = command.OrderId,
                        Reason = command.Reason,
                        Timestamp = DateTime.UtcNow
                    };

                    // Store and publish event
                    await _eventStore.AppendEventAsync(cancelEvent, cancellationToken);
                    await _eventPublisher.PublishAsync(cancelEvent, cancellationToken);

                    return CommandResult.Success(command.OrderId);
                }
                catch (Exception ex)
                {
                    return CommandResult.Failure(new[] { ex.Message });
                }
            }

            private async Task<ValidationResult> ValidateCommandAsync(PlaceOrderCommand command, CancellationToken cancellationToken)
            {
                var errors = new List<string>();

                if (string.IsNullOrEmpty(command.Symbol))
                    errors.Add("Symbol is required");

                if (command.Quantity <= 0)
                    errors.Add("Quantity must be positive");

                if (command.Price <= 0)
                    errors.Add("Price must be positive");

                if (string.IsNullOrEmpty(command.TraderId))
                    errors.Add("Trader ID is required");

                // Additional business validation
                if (command.Quantity > 1000000)
                    errors.Add("Quantity exceeds maximum allowed");

                return new ValidationResult
                {
                    IsValid = !errors.Any(),
                    Errors = errors
                };
            }

            private async Task<OrderState> GetOrderStateAsync(string orderId, CancellationToken cancellationToken)
            {
                // This would typically query the read model
                // For simplicity, we'll return a mock state
                await Task.Delay(10, cancellationToken);
                return new OrderState
                {
                    OrderId = orderId,
                    Status = OrderStatus.Pending
                };
            }
        }

        /// <summary>
        /// PayPal-style payment command handler
        /// Handles payment commands with fraud detection and event publishing
        /// </summary>
        public class PaymentCommandHandler
        {
            private readonly IEventStore _eventStore;
            private readonly IEventPublisher _eventPublisher;
            private readonly IFraudDetectionService _fraudDetectionService;

            public PaymentCommandHandler(
                IEventStore eventStore, 
                IEventPublisher eventPublisher,
                IFraudDetectionService fraudDetectionService)
            {
                _eventStore = eventStore;
                _eventPublisher = eventPublisher;
                _fraudDetectionService = fraudDetectionService;
            }

            public async Task<CommandResult> HandleAsync(ProcessPaymentCommand command, CancellationToken cancellationToken = default)
            {
                try
                {
                    // Validate command
                    var validationResult = await ValidatePaymentCommandAsync(command, cancellationToken);
                    if (!validationResult.IsValid)
                    {
                        return CommandResult.Failure(validationResult.Errors);
                    }

                    // Fraud detection
                    var fraudCheck = await _fraudDetectionService.CheckPaymentAsync(command, cancellationToken);
                    if (fraudCheck.IsFraudulent)
                    {
                        var fraudEvent = new PaymentFraudDetectedEvent
                        {
                            PaymentId = command.PaymentId,
                            CustomerId = command.CustomerId,
                            Amount = command.Amount,
                            Reason = fraudCheck.Reason,
                            Timestamp = DateTime.UtcNow
                        };

                        await _eventStore.AppendEventAsync(fraudEvent, cancellationToken);
                        await _eventPublisher.PublishAsync(fraudEvent, cancellationToken);

                        return CommandResult.Failure(new[] { "Payment flagged for fraud" });
                    }

                    // Process payment
                    var paymentEvent = new PaymentProcessedEvent
                    {
                        PaymentId = command.PaymentId,
                        CustomerId = command.CustomerId,
                        Amount = command.Amount,
                        Currency = command.Currency,
                        Status = PaymentStatus.Success,
                        TransactionId = Guid.NewGuid().ToString(),
                        Timestamp = DateTime.UtcNow
                    };

                    await _eventStore.AppendEventAsync(paymentEvent, cancellationToken);
                    await _eventPublisher.PublishAsync(paymentEvent, cancellationToken);

                    return CommandResult.Success(command.PaymentId);
                }
                catch (Exception ex)
                {
                    return CommandResult.Failure(new[] { ex.Message });
                }
            }

            private async Task<ValidationResult> ValidatePaymentCommandAsync(ProcessPaymentCommand command, CancellationToken cancellationToken)
            {
                var errors = new List<string>();

                if (string.IsNullOrEmpty(command.PaymentId))
                    errors.Add("Payment ID is required");

                if (command.Amount <= 0)
                    errors.Add("Amount must be positive");

                if (string.IsNullOrEmpty(command.Currency))
                    errors.Add("Currency is required");

                if (string.IsNullOrEmpty(command.CustomerId))
                    errors.Add("Customer ID is required");

                return new ValidationResult
                {
                    IsValid = !errors.Any(),
                    Errors = errors
                };
            }
        }

        /// <summary>
        /// Google-style search query handler
        /// Handles search queries with caching and analytics
        /// </summary>
        public class SearchQueryHandler
        {
            private readonly ISearchService _searchService;
            private readonly ICacheService _cacheService;
            private readonly IAnalyticsService _analyticsService;

            public SearchQueryHandler(
                ISearchService searchService,
                ICacheService cacheService,
                IAnalyticsService analyticsService)
            {
                _searchService = searchService;
                _cacheService = cacheService;
                _analyticsService = analyticsService;
            }

            public async Task<SearchResult> HandleAsync(SearchQuery query, CancellationToken cancellationToken = default)
            {
                try
                {
                    // Check cache first
                    var cacheKey = $"search:{query.Query}:{query.UserId}";
                    var cachedResult = await _cacheService.GetAsync<SearchResult>(cacheKey, cancellationToken);
                    if (cachedResult != null)
                    {
                        // Track cache hit
                        await _analyticsService.TrackSearchAsync(query, true, cancellationToken);
                        return cachedResult;
                    }

                    // Perform search
                    var searchResult = await _searchService.SearchAsync(query, cancellationToken);

                    // Cache result
                    await _cacheService.SetAsync(cacheKey, searchResult, TimeSpan.FromMinutes(15), cancellationToken);

                    // Track search analytics
                    await _analyticsService.TrackSearchAsync(query, false, cancellationToken);

                    return searchResult;
                }
                catch (Exception ex)
                {
                    // Track search error
                    await _analyticsService.TrackSearchErrorAsync(query, ex.Message, cancellationToken);
                    throw;
                }
            }
        }

        /// <summary>
        /// Amazon-style inventory command handler
        /// Handles inventory commands with reservation and fulfillment
        /// </summary>
        public class InventoryCommandHandler
        {
            private readonly IEventStore _eventStore;
            private readonly IEventPublisher _eventPublisher;
            private readonly IInventoryReadModel _inventoryReadModel;

            public InventoryCommandHandler(
                IEventStore eventStore,
                IEventPublisher eventPublisher,
                IInventoryReadModel inventoryReadModel)
            {
                _eventStore = eventStore;
                _eventPublisher = eventPublisher;
                _inventoryReadModel = inventoryReadModel;
            }

            public async Task<CommandResult> HandleAsync(ReserveInventoryCommand command, CancellationToken cancellationToken = default)
            {
                try
                {
                    // Check inventory availability
                    var inventory = await _inventoryReadModel.GetInventoryAsync(command.ProductId, cancellationToken);
                    if (inventory == null || inventory.AvailableQuantity < command.Quantity)
                    {
                        return CommandResult.Failure(new[] { "Insufficient inventory" });
                    }

                    // Create reservation event
                    var reservationEvent = new InventoryReservedEvent
                    {
                        ProductId = command.ProductId,
                        Quantity = command.Quantity,
                        OrderId = command.OrderId,
                        Timestamp = DateTime.UtcNow
                    };

                    await _eventStore.AppendEventAsync(reservationEvent, cancellationToken);
                    await _eventPublisher.PublishAsync(reservationEvent, cancellationToken);

                    return CommandResult.Success(command.OrderId);
                }
                catch (Exception ex)
                {
                    return CommandResult.Failure(new[] { ex.Message });
                }
            }

            public async Task<CommandResult> HandleAsync(ReleaseInventoryCommand command, CancellationToken cancellationToken = default)
            {
                try
                {
                    // Create release event
                    var releaseEvent = new InventoryReleasedEvent
                    {
                        ProductId = command.ProductId,
                        Quantity = command.Quantity,
                        OrderId = command.OrderId,
                        Reason = command.Reason,
                        Timestamp = DateTime.UtcNow
                    };

                    await _eventStore.AppendEventAsync(releaseEvent, cancellationToken);
                    await _eventPublisher.PublishAsync(releaseEvent, cancellationToken);

                    return CommandResult.Success(command.OrderId);
                }
                catch (Exception ex)
                {
                    return CommandResult.Failure(new[] { ex.Message });
                }
            }
        }

        /// <summary>
        /// Uber-style ride command handler
        /// Handles ride commands with matching and tracking
        /// </summary>
        public class RideCommandHandler
        {
            private readonly IEventStore _eventStore;
            private readonly IEventPublisher _eventPublisher;
            private readonly IDriverMatchingService _driverMatchingService;

            public RideCommandHandler(
                IEventStore eventStore,
                IEventPublisher eventPublisher,
                IDriverMatchingService driverMatchingService)
            {
                _eventStore = eventStore;
                _eventPublisher = eventPublisher;
                _driverMatchingService = driverMatchingService;
            }

            public async Task<CommandResult> HandleAsync(RequestRideCommand command, CancellationToken cancellationToken = default)
            {
                try
                {
                    // Find matching driver
                    var driver = await _driverMatchingService.FindBestDriverAsync(command.PickupLocation, cancellationToken);
                    if (driver == null)
                    {
                        return CommandResult.Failure(new[] { "No available drivers" });
                    }

                    // Create ride request event
                    var rideRequestEvent = new RideRequestedEvent
                    {
                        RideId = command.RideId,
                        PassengerId = command.PassengerId,
                        DriverId = driver.DriverId,
                        PickupLocation = command.PickupLocation,
                        Destination = command.Destination,
                        Timestamp = DateTime.UtcNow
                    };

                    await _eventStore.AppendEventAsync(rideRequestEvent, cancellationToken);
                    await _eventPublisher.PublishAsync(rideRequestEvent, cancellationToken);

                    return CommandResult.Success(command.RideId);
                }
                catch (Exception ex)
                {
                    return CommandResult.Failure(new[] { ex.Message });
                }
            }

            public async Task<CommandResult> HandleAsync(CompleteRideCommand command, CancellationToken cancellationToken = default)
            {
                try
                    {
                        // Create ride completion event
                        var rideCompletedEvent = new RideCompletedEvent
                        {
                            RideId = command.RideId,
                            Fare = command.Fare,
                            Duration = command.Duration,
                            Timestamp = DateTime.UtcNow
                        };

                        await _eventStore.AppendEventAsync(rideCompletedEvent, cancellationToken);
                        await _eventPublisher.PublishAsync(rideCompletedEvent, cancellationToken);

                        return CommandResult.Success(command.RideId);
                    }
                catch (Exception ex)
                {
                    return CommandResult.Failure(new[] { ex.Message });
                }
            }
        }
    }

    #region Supporting Classes

    public class CommandResult
    {
        public bool IsSuccess { get; set; }
        public string EntityId { get; set; }
        public string[] Errors { get; set; }

        public static CommandResult Success(string entityId)
        {
            return new CommandResult { IsSuccess = true, EntityId = entityId };
        }

        public static CommandResult Failure(string[] errors)
        {
            return new CommandResult { IsSuccess = false, Errors = errors };
        }
    }

    public class ValidationResult
    {
        public bool IsValid { get; set; }
        public List<string> Errors { get; set; } = new();
    }

    // Commands
    public class PlaceOrderCommand
    {
        public string OrderId { get; set; }
        public string Symbol { get; set; }
        public int Quantity { get; set; }
        public decimal Price { get; set; }
        public OrderType OrderType { get; set; }
        public string TraderId { get; set; }
    }

    public class CancelOrderCommand
    {
        public string OrderId { get; set; }
        public string Reason { get; set; }
    }

    public class ProcessPaymentCommand
    {
        public string PaymentId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Currency { get; set; }
    }

    public class SearchQuery
    {
        public string Query { get; set; }
        public string UserId { get; set; }
        public int Page { get; set; } = 1;
        public int PageSize { get; set; } = 10;
    }

    public class ReserveInventoryCommand
    {
        public string ProductId { get; set; }
        public int Quantity { get; set; }
        public string OrderId { get; set; }
    }

    public class ReleaseInventoryCommand
    {
        public string ProductId { get; set; }
        public int Quantity { get; set; }
        public string OrderId { get; set; }
        public string Reason { get; set; }
    }

    public class RequestRideCommand
    {
        public string RideId { get; set; }
        public string PassengerId { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
    }

    public class CompleteRideCommand
    {
        public string RideId { get; set; }
        public decimal Fare { get; set; }
        public TimeSpan Duration { get; set; }
    }

    // Events
    public class OrderPlacedEvent
    {
        public string OrderId { get; set; }
        public string Symbol { get; set; }
        public int Quantity { get; set; }
        public decimal Price { get; set; }
        public OrderType OrderType { get; set; }
        public string TraderId { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class OrderCancelledEvent
    {
        public string OrderId { get; set; }
        public string Reason { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class PaymentProcessedEvent
    {
        public string PaymentId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Currency { get; set; }
        public PaymentStatus Status { get; set; }
        public string TransactionId { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class PaymentFraudDetectedEvent
    {
        public string PaymentId { get; set; }
        public string CustomerId { get; set; }
        public decimal Amount { get; set; }
        public string Reason { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class InventoryReservedEvent
    {
        public string ProductId { get; set; }
        public int Quantity { get; set; }
        public string OrderId { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class InventoryReleasedEvent
    {
        public string ProductId { get; set; }
        public int Quantity { get; set; }
        public string OrderId { get; set; }
        public string Reason { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class RideRequestedEvent
    {
        public string RideId { get; set; }
        public string PassengerId { get; set; }
        public string DriverId { get; set; }
        public Location PickupLocation { get; set; }
        public Location Destination { get; set; }
        public DateTime Timestamp { get; set; }
    }

    public class RideCompletedEvent
    {
        public string RideId { get; set; }
        public decimal Fare { get; set; }
        public TimeSpan Duration { get; set; }
        public DateTime Timestamp { get; set; }
    }

    // Read Models
    public class SearchResult
    {
        public List<SearchItem> Items { get; set; } = new();
        public int TotalCount { get; set; }
        public int Page { get; set; }
        public int PageSize { get; set; }
    }

    public class SearchItem
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public string Description { get; set; }
        public string Url { get; set; }
        public double Score { get; set; }
    }

    public class OrderState
    {
        public string OrderId { get; set; }
        public OrderStatus Status { get; set; }
    }

    public class InventoryState
    {
        public string ProductId { get; set; }
        public int AvailableQuantity { get; set; }
        public int ReservedQuantity { get; set; }
    }

    // Enums
    public enum OrderType
    {
        Buy,
        Sell
    }

    public enum OrderStatus
    {
        Pending,
        Filled,
        Cancelled
    }

    public enum PaymentStatus
    {
        Pending,
        Success,
        Failed
    }

    public class Location
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
    }

    // Interfaces
    public interface IEventStore
    {
        Task AppendEventAsync<T>(T eventData, CancellationToken cancellationToken = default) where T : class;
    }

    public interface IEventPublisher
    {
        Task PublishAsync<T>(T eventData, CancellationToken cancellationToken = default) where T : class;
    }

    public interface IFraudDetectionService
    {
        Task<FraudCheckResult> CheckPaymentAsync(ProcessPaymentCommand command, CancellationToken cancellationToken = default);
    }

    public class FraudCheckResult
    {
        public bool IsFraudulent { get; set; }
        public string Reason { get; set; }
    }

    public interface ISearchService
    {
        Task<SearchResult> SearchAsync(SearchQuery query, CancellationToken cancellationToken = default);
    }

    public interface ICacheService
    {
        Task<T> GetAsync<T>(string key, CancellationToken cancellationToken = default);
        Task SetAsync<T>(string key, T value, TimeSpan expiration, CancellationToken cancellationToken = default);
    }

    public interface IAnalyticsService
    {
        Task TrackSearchAsync(SearchQuery query, bool fromCache, CancellationToken cancellationToken = default);
        Task TrackSearchErrorAsync(SearchQuery query, string error, CancellationToken cancellationToken = default);
    }

    public interface IInventoryReadModel
    {
        Task<InventoryState> GetInventoryAsync(string productId, CancellationToken cancellationToken = default);
    }

    public interface IDriverMatchingService
    {
        Task<Driver> FindBestDriverAsync(Location pickupLocation, CancellationToken cancellationToken = default);
    }

    public class Driver
    {
        public string DriverId { get; set; }
        public Location Location { get; set; }
        public bool IsAvailable { get; set; }
    }

    #endregion
}
