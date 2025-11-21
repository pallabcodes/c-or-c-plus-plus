"""
AuroraDB Python Driver

High-performance async Python driver for AuroraDB with native async/await support,
vector search capabilities, and advanced analytics features.
"""

import asyncio
import json
from typing import List, Dict, Any, Optional, Union, Iterator, AsyncIterator
from dataclasses import dataclass
from datetime import datetime
import logging

# Import the Rust core (would be via PyO3 bindings)
try:
    from aurora._native import AuroraClient as _NativeClient
    from aurora._native import AuroraError as _NativeError
except ImportError:
    # Fallback for development
    class _NativeClient:
        pass
    class _NativeError(Exception):
        pass

logger = logging.getLogger(__name__)


@dataclass
class AuroraConfig:
    """AuroraDB connection configuration"""
    host: str = "localhost"
    port: int = 5433
    database: str = "aurora"
    user: str = "aurora"
    password: Optional[str] = None
    ssl_mode: str = "require"
    ssl_cert: Optional[str] = None
    ssl_key: Optional[str] = None
    ssl_ca: Optional[str] = None
    connection_timeout: float = 30.0
    max_connections: int = 20
    min_connections: int = 5
    max_idle_time: float = 300.0
    retry_attempts: int = 3
    retry_delay: float = 1.0

    @classmethod
    def from_url(cls, url: str) -> 'AuroraConfig':
        """Parse AuroraDB URL into configuration"""
        # Parse aurora://user:pass@host:port/database?param=value
        import urllib.parse
        parsed = urllib.parse.urlparse(url)

        if parsed.scheme != 'aurora':
            raise ValueError(f"Invalid URL scheme: {parsed.scheme}")

        # Extract auth
        user = parsed.username
        password = parsed.password

        # Extract host and port
        host = parsed.hostname or 'localhost'
        port = parsed.port or 5433

        # Extract database
        path = parsed.path.lstrip('/')
        database = path if path else 'aurora'

        # Extract query parameters
        query_params = urllib.parse.parse_qs(parsed.query)

        return cls(
            host=host,
            port=port,
            database=database,
            user=user,
            password=password,
            ssl_mode=query_params.get('sslmode', ['require'])[0],
            ssl_cert=query_params.get('sslcert', [None])[0],
            ssl_key=query_params.get('sslkey', [None])[0],
            ssl_ca=query_params.get('sslca', [None])[0],
            connection_timeout=float(query_params.get('connect_timeout', ['30.0'])[0]),
            max_connections=int(query_params.get('max_connections', ['20'])[0]),
        )


@dataclass
class QueryResult:
    """Query execution result"""
    rows: List[Dict[str, Any]]
    columns: List[str]
    row_count: int
    execution_time_ms: float
    query_id: str


@dataclass
class VectorSearchResult:
    """Vector search result"""
    results: List[Dict[str, Any]]
    search_time_ms: float
    total_candidates: int
    query_vector: List[float]


@dataclass
class AnalyticsResult:
    """Analytics query result"""
    data: List[Dict[str, Any]]
    metadata: Dict[str, Any]
    execution_time_ms: float
    processed_rows: int


class AuroraClient:
    """AuroraDB async client"""

    def __init__(self, config: AuroraConfig):
        self.config = config
        self._native_client = None
        self._closed = False

    @classmethod
    async def connect(cls, url_or_config: Union[str, AuroraConfig]) -> 'AuroraClient':
        """Connect to AuroraDB"""
        if isinstance(url_or_config, str):
            config = AuroraConfig.from_url(url_or_config)
        else:
            config = url_or_config

        client = cls(config)
        await client._connect()
        return client

    async def _connect(self):
        """Establish connection to AuroraDB"""
        try:
            # Convert config to native format
            native_config = {
                "host": self.config.host,
                "port": self.config.port,
                "database": self.config.database,
                "user": self.config.user,
                "password": self.config.password,
                "ssl_mode": self.config.ssl_mode,
                "ssl_cert": self.config.ssl_cert,
                "ssl_key": self.config.ssl_key,
                "ssl_ca": self.config.ssl_ca,
                "connection_timeout": self.config.connection_timeout,
                "max_connections": self.config.max_connections,
                "min_connections": self.config.min_connections,
                "max_idle_time": self.config.max_idle_time,
                "retry_attempts": self.config.retry_attempts,
                "retry_delay": self.config.retry_delay,
            }

            self._native_client = _NativeClient(native_config)
            await self._native_client.connect()

            logger.info(f"Connected to AuroraDB at {self.config.host}:{self.config.port}")

        except _NativeError as e:
            raise AuroraError(f"Connection failed: {e}")

    async def query(self, sql: str, params: Optional[List[Any]] = None) -> QueryResult:
        """Execute a query"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            if params:
                result = await self._native_client.query_with_params(sql, params)
            else:
                result = await self._native_client.query(sql)

            return QueryResult(
                rows=result.rows,
                columns=result.columns,
                row_count=result.row_count,
                execution_time_ms=result.execution_time_ms,
                query_id=result.query_id,
            )

        except _NativeError as e:
            raise AuroraError(f"Query failed: {e}")

    async def execute(self, sql: str, params: Optional[List[Any]] = None) -> Dict[str, Any]:
        """Execute a statement (INSERT, UPDATE, DELETE)"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            if params:
                result = await self._native_client.execute_with_params(sql, params)
            else:
                result = await self._native_client.execute(sql)

            return {
                "rows_affected": result.rows_affected,
                "last_insert_id": result.last_insert_id,
                "execution_time_ms": result.execution_time_ms,
            }

        except _NativeError as e:
            raise AuroraError(f"Execute failed: {e}")

    async def vector_search(
        self,
        collection: str,
        query_vector: List[float],
        limit: int = 10,
        filters: Optional[Dict[str, Any]] = None,
        rerank: bool = False,
        explain: bool = False,
    ) -> VectorSearchResult:
        """Perform vector similarity search"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            request = {
                "collection": collection,
                "query_vector": query_vector,
                "limit": limit,
                "filters": filters or {},
                "rerank": rerank,
                "explain": explain,
            }

            result = await self._native_client.vector_search_advanced(request)

            return VectorSearchResult(
                results=result.results,
                search_time_ms=result.search_time_ms,
                total_candidates=result.total_candidates,
                query_vector=query_vector,
            )

        except _NativeError as e:
            raise AuroraError(f"Vector search failed: {e}")

    async def analytics_query(self, sql: str) -> AnalyticsResult:
        """Execute analytics query"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            result = await self._native_client.analytics_query(sql)

            return AnalyticsResult(
                data=result.data,
                metadata=result.metadata,
                execution_time_ms=result.execution_time_ms,
                processed_rows=result.processed_rows,
            )

        except _NativeError as e:
            raise AuroraError(f"Analytics query failed: {e}")

    async def stream_analytics(self, sql: str) -> AsyncIterator[Dict[str, Any]]:
        """Stream analytics results"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            stream = await self._native_client.stream_analytics(sql)

            async for row in stream:
                yield row

        except _NativeError as e:
            raise AuroraError(f"Analytics streaming failed: {e}")

    async def transaction(self) -> 'AuroraTransaction':
        """Start a transaction"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            native_txn = await self._native_client.transaction()
            return AuroraTransaction(native_txn)
        except _NativeError as e:
            raise AuroraError(f"Transaction start failed: {e}")

    async def batch_execute(self, statements: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Execute multiple statements in batch"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            results = await self._native_client.batch_execute(statements)
            return results
        except _NativeError as e:
            raise AuroraError(f"Batch execute failed: {e}")

    async def health_check(self) -> Dict[str, Any]:
        """Check database health"""
        if self._closed:
            raise AuroraError("Client is closed")

        try:
            health = await self._native_client.health_check()
            return {
                "status": health.status,
                "message": health.message,
                "details": health.details,
            }
        except _NativeError as e:
            raise AuroraError(f"Health check failed: {e}")

    async def close(self):
        """Close the client connection"""
        if not self._closed and self._native_client:
            await self._native_client.close()
            self._closed = True
            logger.info("AuroraDB client closed")

    async def __aenter__(self):
        """Async context manager entry"""
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """Async context manager exit"""
        await self.close()


class AuroraTransaction:
    """Database transaction"""

    def __init__(self, native_transaction):
        self._native_txn = native_transaction
        self._committed = False
        self._rolled_back = False

    async def query(self, sql: str, params: Optional[List[Any]] = None) -> QueryResult:
        """Execute query in transaction"""
        self._check_active()

        try:
            if params:
                result = await self._native_txn.query_with_params(sql, params)
            else:
                result = await self._native_txn.query(sql)

            return QueryResult(
                rows=result.rows,
                columns=result.columns,
                row_count=result.row_count,
                execution_time_ms=result.execution_time_ms,
                query_id=result.query_id,
            )

        except _NativeError as e:
            raise AuroraError(f"Transaction query failed: {e}")

    async def execute(self, sql: str, params: Optional[List[Any]] = None) -> Dict[str, Any]:
        """Execute statement in transaction"""
        self._check_active()

        try:
            if params:
                result = await self._native_txn.execute_with_params(sql, params)
            else:
                result = await self._native_txn.execute(sql)

            return {
                "rows_affected": result.rows_affected,
                "last_insert_id": result.last_insert_id,
                "execution_time_ms": result.execution_time_ms,
            }

        except _NativeError as e:
            raise AuroraError(f"Transaction execute failed: {e}")

    async def commit(self):
        """Commit transaction"""
        self._check_active()

        try:
            await self._native_txn.commit()
            self._committed = True
        except _NativeError as e:
            raise AuroraError(f"Transaction commit failed: {e}")

    async def rollback(self):
        """Rollback transaction"""
        if not self._committed:
            try:
                await self._native_txn.rollback()
            except _NativeError as e:
                raise AuroraError(f"Transaction rollback failed: {e}")
        self._rolled_back = True

    def _check_active(self):
        """Check if transaction is still active"""
        if self._committed:
            raise AuroraError("Transaction already committed")
        if self._rolled_back:
            raise AuroraError("Transaction already rolled back")


class AuroraError(Exception):
    """AuroraDB error"""
    pass


# Convenience functions

async def connect(url: str) -> AuroraClient:
    """Connect to AuroraDB using URL"""
    return await AuroraClient.connect(url)


async def connect_with_config(config: AuroraConfig) -> AuroraClient:
    """Connect to AuroraDB using configuration object"""
    return await AuroraClient.connect(config)


# Example usage
async def main():
    """Example usage of AuroraDB Python driver"""
    # Connect to database
    async with AuroraClient.connect("aurora://localhost:5433/mydb") as client:

        # Simple query
        result = await client.query("SELECT id, name FROM users LIMIT 10")
        print(f"Found {result.row_count} users")

        # Vector search
        search_results = await client.vector_search(
            collection="products",
            query_vector=[0.1, 0.2, 0.3, 0.4, 0.5],
            limit=5,
            filters={"category": "electronics", "price": {"$lt": 100}}
        )
        print(f"Found {len(search_results.results)} similar products")

        # Analytics query
        analytics = await client.analytics_query("""
            SELECT category, COUNT(*) as count, AVG(price) as avg_price
            FROM products
            WHERE created_date >= NOW() - INTERVAL '30 days'
            GROUP BY category
            ORDER BY count DESC
        """)
        print(f"Analytics processed {analytics.processed_rows} rows")

        # Transaction
        async with client.transaction() as txn:
            await txn.execute("INSERT INTO users (name, email) VALUES (?, ?)",
                            ["John Doe", "john@example.com"])
            await txn.execute("INSERT INTO user_profiles (user_id, bio) VALUES (LAST_INSERT_ID(), ?)",
                            ["Software engineer"])
            await txn.commit()

        print("Transaction completed successfully")


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    asyncio.run(main())
