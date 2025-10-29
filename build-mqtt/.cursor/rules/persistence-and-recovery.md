# Persistence and Recovery

## WAL and Snapshots
* Append-only log for publishes and session mutations
* Periodic snapshots to bound recovery time

## Crash Recovery
* Rebuild session store and retained store on restart
* Deduplicate inflight QoS messages

## Retained Store
* Per-topic last-value store with delete via empty payload
* Persist retained entries with compaction

## Storage Engine Choices
* Embedded RocksDB/LMDB or custom append log
* Consider sync policies (fdatasync) and batching

## Testing
* Power-cut tests; forced crashes; disk full/slow scenarios
* Recovery correctness for QoS1/2 and retained
