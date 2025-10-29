# Security for IPC

## Scope
Applies to all IPC code. Extends repository root rules.

## Permissions
* Use least privilege permissions when creating shared objects and queues
* Start with restrictive permissions and only widen as needed
* Document permission requirements and their rationale
* Avoid world readable or writable shared objects unless explicitly required

## Object Naming
* Do not use predictable names for shared objects without a namespacing strategy
* Include process ID, user ID, or other unique identifiers in object names
* Clean up named objects after use to prevent reuse attacks
* Use abstract namespaces for Unix domain sockets where supported

## Input Sanitization
* Sanitize all external inputs and data passed through IPC channels
* Validate message sizes and types before processing
* Reject malformed or suspicious IPC messages
* Use structured message formats rather than raw strings where possible

## Sensitive Data Protection
* Avoid storing sensitive data like passwords or keys in shared memory without encryption
* Clear sensitive data from memory after use
* Use secure IPC channels for sensitive operations
* Document security assumptions and threat model

