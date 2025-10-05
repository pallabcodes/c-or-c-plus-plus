using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace DotNetDesignPatternDemos.DesignPatterns.Microservices
{
    /// <summary>
    /// Microservices patterns used by top-tier companies.
    /// Covers API Gateway, Service Discovery, Circuit Breaker, Saga, and Event Sourcing.
    /// </summary>
    public class MicroservicesPatterns
    {
        /// <summary>
        /// Google-style API Gateway pattern for request routing
        /// Routes requests to appropriate microservices with load balancing
        /// </summary>
        public class ApiGateway
        {
            private readonly HttpClient _httpClient;
            private readonly Dictionary<string, List<string>> _serviceEndpoints;
            private readonly Random _random = new Random();

            public ApiGateway()
            {
                _httpClient = new HttpClient();
                _serviceEndpoints = new Dictionary<string, List<string>>
                {
                    ["user-service"] = new List<string> { "http://user-service-1:8080", "http://user-service-2:8080" },
                    ["order-service"] = new List<string> { "http://order-service-1:8080", "http://order-service-2:8080" },
                    ["payment-service"] = new List<string> { "http://payment-service-1:8080", "http://payment-service-2:8080" },
                    ["notification-service"] = new List<string> { "http://notification-service-1:8080", "http://notification-service-2:8080" }
                };
            }

            public async Task<ApiResponse> RouteRequest(string serviceName, string endpoint, object data = null)
            {
                if (!_serviceEndpoints.TryGetValue(serviceName, out var endpoints) || !endpoints.Any())
                {
                    return new ApiResponse
                    {
                        Success = false,
                        ErrorMessage = $"Service {serviceName} not found"
                    };
                }

                // Load balancing - round robin
                var selectedEndpoint = endpoints[_random.Next(endpoints.Count)];
                var url = $"{selectedEndpoint}/{endpoint}";

                try
                {
                    var response = await _httpClient.GetAsync(url);
                    return new ApiResponse
                    {
                        Success = response.IsSuccessStatusCode,
                        StatusCode = (int)response.StatusCode,
                        Content = await response.Content.ReadAsStringAsync()
                    };
                }
                catch (Exception ex)
                {
                    return new ApiResponse
                    {
                        Success = false,
                        ErrorMessage = ex.Message
                    };
                }
            }

            public void Dispose()
            {
                _httpClient?.Dispose();
            }
        }

        /// <summary>
        /// Bloomberg-style Service Discovery pattern for dynamic service location
        /// Discovers and maintains service endpoints dynamically
        /// </summary>
        public class ServiceDiscovery
        {
            private readonly ConcurrentDictionary<string, List<ServiceEndpoint>> _services;
            private readonly Timer _healthCheckTimer;

            public ServiceDiscovery()
            {
                _services = new ConcurrentDictionary<string, List<ServiceEndpoint>>();
                _healthCheckTimer = new Timer(PerformHealthChecks, null, TimeSpan.Zero, TimeSpan.FromSeconds(30));
            }

            public void RegisterService(string serviceName, string endpoint, int port)
            {
                var serviceEndpoint = new ServiceEndpoint
                {
                    Endpoint = endpoint,
                    Port = port,
                    LastHealthCheck = DateTime.UtcNow,
                    IsHealthy = true
                };

                _services.AddOrUpdate(serviceName,
                    new List<ServiceEndpoint> { serviceEndpoint },
                    (key, existing) =>
                    {
                        existing.Add(serviceEndpoint);
                        return existing;
                    });
            }

            public List<ServiceEndpoint> GetHealthyEndpoints(string serviceName)
            {
                if (_services.TryGetValue(serviceName, out var endpoints))
                {
                    return endpoints.Where(e => e.IsHealthy).ToList();
                }
                return new List<ServiceEndpoint>();
            }

            public ServiceEndpoint GetRandomHealthyEndpoint(string serviceName)
            {
                var healthyEndpoints = GetHealthyEndpoints(serviceName);
                if (healthyEndpoints.Any())
                {
                    return healthyEndpoints[new Random().Next(healthyEndpoints.Count)];
                }
                return null;
            }

            private async void PerformHealthChecks(object state)
            {
                foreach (var service in _services)
                {
                    foreach (var endpoint in service.Value)
                    {
                        try
                        {
                            using var httpClient = new HttpClient();
                            httpClient.Timeout = TimeSpan.FromSeconds(5);
                            var response = await httpClient.GetAsync($"http://{endpoint.Endpoint}:{endpoint.Port}/health");
                            endpoint.IsHealthy = response.IsSuccessStatusCode;
                            endpoint.LastHealthCheck = DateTime.UtcNow;
                        }
                        catch
                        {
                            endpoint.IsHealthy = false;
                            endpoint.LastHealthCheck = DateTime.UtcNow;
                        }
                    }
                }
            }

            public void Dispose()
            {
                _healthCheckTimer?.Dispose();
            }
        }

        /// <summary>
        /// PayPal-style Circuit Breaker pattern for fault tolerance
        /// Prevents cascading failures in distributed systems
        /// </summary>
        public class CircuitBreaker
        {
            private readonly int _failureThreshold;
            private readonly TimeSpan _timeout;
            private readonly TimeSpan _retryTimeout;
            private int _failureCount;
            private DateTime _lastFailureTime;
            private CircuitBreakerState _state = CircuitBreakerState.Closed;

            public CircuitBreaker(int failureThreshold = 5, TimeSpan timeout = default, TimeSpan retryTimeout = default)
            {
                _failureThreshold = failureThreshold;
                _timeout = timeout == default ? TimeSpan.FromMinutes(1) : timeout;
                _retryTimeout = retryTimeout == default ? TimeSpan.FromSeconds(30) : retryTimeout;
            }

            public async Task<T> ExecuteAsync<T>(Func<Task<T>> operation)
            {
                if (_state == CircuitBreakerState.Open)
                {
                    if (DateTime.UtcNow - _lastFailureTime > _retryTimeout)
                    {
                        _state = CircuitBreakerState.HalfOpen;
                    }
                    else
                    {
                        throw new CircuitBreakerOpenException("Circuit breaker is open");
                    }
                }

                try
                {
                    var result = await operation();
                    OnSuccess();
                    return result;
                }
                catch (Exception ex)
                {
                    OnFailure();
                    throw;
                }
            }

            private void OnSuccess()
            {
                _failureCount = 0;
                _state = CircuitBreakerState.Closed;
            }

            private void OnFailure()
            {
                _failureCount++;
                _lastFailureTime = DateTime.UtcNow;

                if (_failureCount >= _failureThreshold)
                {
                    _state = CircuitBreakerState.Open;
                }
            }

            public CircuitBreakerState State => _state;
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

        /// <summary>
        /// Uber-style Saga pattern for distributed transactions
        /// Manages distributed transactions across multiple services
        /// </summary>
        public class SagaOrchestrator
        {
            private readonly Dictionary<string, Saga> _sagas;
            private readonly object _lock = new object();

            public SagaOrchestrator()
            {
                _sagas = new Dictionary<string, Saga>();
            }

            public string StartSaga(List<SagaStep> steps)
            {
                var sagaId = Guid.NewGuid().ToString();
                var saga = new Saga
                {
                    Id = sagaId,
                    Steps = steps,
                    CurrentStep = 0,
                    Status = SagaStatus.Running,
                    CreatedAt = DateTime.UtcNow
                };

                lock (_lock)
                {
                    _sagas[sagaId] = saga;
                }

                _ = Task.Run(() => ExecuteSaga(saga));
                return sagaId;
            }

            private async Task ExecuteSaga(Saga saga)
            {
                try
                {
                    for (int i = 0; i < saga.Steps.Count; i++)
                    {
                        saga.CurrentStep = i;
                        var step = saga.Steps[i];

                        try
                        {
                            await step.Execute();
                            step.Status = SagaStepStatus.Completed;
                        }
                        catch (Exception ex)
                        {
                            step.Status = SagaStepStatus.Failed;
                            step.Error = ex.Message;
                            await CompensateSaga(saga, i);
                            return;
                        }
                    }

                    saga.Status = SagaStatus.Completed;
                }
                catch (Exception ex)
                {
                    saga.Status = SagaStatus.Failed;
                    saga.Error = ex.Message;
                }
            }

            private async Task CompensateSaga(Saga saga, int failedStepIndex)
            {
                saga.Status = SagaStatus.Compensating;

                for (int i = failedStepIndex - 1; i >= 0; i--)
                {
                    var step = saga.Steps[i];
                    if (step.Status == SagaStepStatus.Completed)
                    {
                        try
                        {
                            await step.Compensate();
                            step.Status = SagaStepStatus.Compensated;
                        }
                        catch (Exception ex)
                        {
                            step.CompensationError = ex.Message;
                        }
                    }
                }

                saga.Status = SagaStatus.Failed;
            }

            public Saga GetSaga(string sagaId)
            {
                lock (_lock)
                {
                    return _sagas.TryGetValue(sagaId, out var saga) ? saga : null;
                }
            }
        }

        /// <summary>
        /// Amazon-style Event Sourcing pattern for audit trails
        /// Stores events instead of current state for complete audit trail
        /// </summary>
        public class EventStore
        {
            private readonly List<DomainEvent> _events;
            private readonly object _lock = new object();

            public EventStore()
            {
                _events = new List<DomainEvent>();
            }

            public void AppendEvent(DomainEvent domainEvent)
            {
                lock (_lock)
                {
                    domainEvent.Version = _events.Count + 1;
                    domainEvent.Timestamp = DateTime.UtcNow;
                    _events.Add(domainEvent);
                }
            }

            public List<DomainEvent> GetEvents(string aggregateId)
            {
                lock (_lock)
                {
                    return _events.Where(e => e.AggregateId == aggregateId).ToList();
                }
            }

            public List<DomainEvent> GetEvents(string aggregateId, int fromVersion)
            {
                lock (_lock)
                {
                    return _events.Where(e => e.AggregateId == aggregateId && e.Version > fromVersion).ToList();
                }
            }

            public List<DomainEvent> GetAllEvents()
            {
                lock (_lock)
                {
                    return new List<DomainEvent>(_events);
                }
            }
        }

        /// <summary>
        /// Stripe-style CQRS pattern for read/write separation
        /// Separates command and query responsibilities for better scalability
        /// </summary>
        public class CqrsHandler
        {
            private readonly EventStore _eventStore;
            private readonly Dictionary<Type, Func<object, Task>> _commandHandlers;
            private readonly Dictionary<Type, Func<object, Task<object>>> _queryHandlers;

            public CqrsHandler(EventStore eventStore)
            {
                _eventStore = eventStore;
                _commandHandlers = new Dictionary<Type, Func<object, Task>>();
                _queryHandlers = new Dictionary<Type, Func<object, Task<object>>>();
            }

            public void RegisterCommandHandler<T>(Func<T, Task> handler)
            {
                _commandHandlers[typeof(T)] = async (command) => await handler((T)command);
            }

            public void RegisterQueryHandler<T, TResult>(Func<T, Task<TResult>> handler)
            {
                _queryHandlers[typeof(T)] = async (query) => await handler((T)query);
            }

            public async Task HandleCommand<T>(T command)
            {
                if (_commandHandlers.TryGetValue(typeof(T), out var handler))
                {
                    await handler(command);
                }
                else
                {
                    throw new InvalidOperationException($"No handler registered for command type {typeof(T)}");
                }
            }

            public async Task<TResult> HandleQuery<T, TResult>(T query)
            {
                if (_queryHandlers.TryGetValue(typeof(T), out var handler))
                {
                    var result = await handler(query);
                    return (TResult)result;
                }
                else
                {
                    throw new InvalidOperationException($"No handler registered for query type {typeof(T)}");
                }
            }
        }

        /// <summary>
        /// Atlassian-style Bulkhead pattern for resource isolation
        /// Isolates resources to prevent cascading failures
        /// </summary>
        public class BulkheadPattern
        {
            private readonly SemaphoreSlim _criticalOperations;
            private readonly SemaphoreSlim _normalOperations;
            private readonly SemaphoreSlim _backgroundOperations;

            public BulkheadPattern(int criticalLimit = 5, int normalLimit = 20, int backgroundLimit = 50)
            {
                _criticalOperations = new SemaphoreSlim(criticalLimit, criticalLimit);
                _normalOperations = new SemaphoreSlim(normalLimit, normalLimit);
                _backgroundOperations = new SemaphoreSlim(backgroundLimit, backgroundLimit);
            }

            public async Task<T> ExecuteCriticalOperation<T>(Func<Task<T>> operation)
            {
                await _criticalOperations.WaitAsync();
                try
                {
                    return await operation();
                }
                finally
                {
                    _criticalOperations.Release();
                }
            }

            public async Task<T> ExecuteNormalOperation<T>(Func<Task<T>> operation)
            {
                await _normalOperations.WaitAsync();
                try
                {
                    return await operation();
                }
                finally
                {
                    _normalOperations.Release();
                }
            }

            public async Task<T> ExecuteBackgroundOperation<T>(Func<Task<T>> operation)
            {
                await _backgroundOperations.WaitAsync();
                try
                {
                    return await operation();
                }
                finally
                {
                    _backgroundOperations.Release();
                }
            }

            public void Dispose()
            {
                _criticalOperations?.Dispose();
                _normalOperations?.Dispose();
                _backgroundOperations?.Dispose();
            }
        }
    }

    #region Supporting Classes

    public class ApiResponse
    {
        public bool Success { get; set; }
        public int StatusCode { get; set; }
        public string Content { get; set; }
        public string ErrorMessage { get; set; }
    }

    public class ServiceEndpoint
    {
        public string Endpoint { get; set; }
        public int Port { get; set; }
        public DateTime LastHealthCheck { get; set; }
        public bool IsHealthy { get; set; }
    }

    public class Saga
    {
        public string Id { get; set; }
        public List<SagaStep> Steps { get; set; } = new();
        public int CurrentStep { get; set; }
        public SagaStatus Status { get; set; }
        public string Error { get; set; }
        public DateTime CreatedAt { get; set; }
    }

    public class SagaStep
    {
        public string Name { get; set; }
        public Func<Task> Execute { get; set; }
        public Func<Task> Compensate { get; set; }
        public SagaStepStatus Status { get; set; }
        public string Error { get; set; }
        public string CompensationError { get; set; }
    }

    public enum SagaStatus
    {
        Running,
        Completed,
        Failed,
        Compensating
    }

    public enum SagaStepStatus
    {
        Pending,
        Completed,
        Failed,
        Compensated
    }

    public class DomainEvent
    {
        public string Id { get; set; }
        public string AggregateId { get; set; }
        public string EventType { get; set; }
        public object Data { get; set; }
        public int Version { get; set; }
        public DateTime Timestamp { get; set; }
    }

    #endregion
}
