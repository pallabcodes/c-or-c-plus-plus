# Recovery and Durability Standards

## Scope
Applies to all recovery and durability code including write ahead logging, checkpointing, and crash recovery. Extends repository root rules.

## Write Ahead Logging (WAL)

### Log Structure
* Sequential append only log file
* Log sequence numbers (LSN) for ordering
* Log records with proper headers
* Checksums for corruption detection
* Reference: ARIES Recovery Algorithm (Mohan et al., 1992)

### Log Record Types
* Update records (before and after images)
* Commit records
* Abort records
* Checkpoint records
* Compensation log records (CLRs) for undo

### Log Management
* Log file rotation and archival
* Log compression where applicable
* Group commit for throughput
* Flush policies (synchronous, asynchronous, ordered)

## Checkpointing

### Checkpoint Types
* Fuzzy checkpoints for reduced downtime
* Sharp checkpoints when possible
* Incremental checkpoints
* Background checkpointing

### Checkpoint Process
* Identify dirty pages
* Write dirty pages to stable storage
* Record checkpoint LSN
* Maintain checkpoint log record
* Flush log up to checkpoint LSN

## ARIES Recovery Algorithm

### Three Passes

#### Analysis Pass
* Identify dirty pages from log
* Identify active transactions
* Determine redo start point
* Determine undo end point

#### Redo Pass
* Redo all operations from redo start point
* Replay log records forward
* Apply CLRs normally
* Update page LSNs

#### Undo Pass
* Undo operations of loser transactions
* Write CLRs for undo operations
* Follow next undo LSN chain
* Handle nested top actions

### Implementation Requirements
* Correct LSN tracking throughout system
* Proper handling of redo only operations
* Correct undo via CLRs
* Recover both system and transaction failures

## Crash Recovery

### Recovery Initialization
* Detect last checkpoint
* Read checkpoint log record
* Initialize recovery state
* Load active transaction table

### Recovery Execution
* Execute ARIES three passes
* Restore database to consistent state
* Clean up incomplete transactions
* Recover to last committed state

### Media Recovery
* Backup and restore procedures
* Point in time recovery
* Incremental backups
* Continuous archiving

## Durability Guarantees

### Synchronous Commit
* fsync on commit
* Full durability guarantee
* Higher latency

### Asynchronous Commit
* No fsync on commit
* Lower latency
* Risk of data loss on crash

### Group Commit
* Batch multiple commits
* Single fsync for batch
* Balance durability and performance

## Implementation Requirements
* All writes must go through WAL
* Proper fsync strategy selection
* Handle log file full conditions
* Log truncation and archival
* Corruption detection and handling
* Recovery testing and validation

## Performance Considerations
* Minimize fsync calls through batching
* Use write combining where possible
* Separate log disk for performance
* Async logging for lower latency
* Profile recovery time

