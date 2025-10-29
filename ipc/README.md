IPC module

Validation checklist for new IPC samples
1. Resource management
   - All system calls checked for failure
   - All resources released on all code paths
   - No zombies and no leaked descriptors or mappings
2. Error handling
   - Clear messages without internal details
   - errno inspected and appropriate codes returned
3. Synchronization
   - Ordering and ownership documented
   - Timeouts handled where blocking is possible
4. Memory safety
   - Bounds checked and mmap return validated
   - Shared structs aligned and initialized
5. Security
   - Least privilege permissions for shared objects
   - Namespaced identifiers to avoid collisions
6. Testing
   - Producer and consumer exercised
   - Failure and cleanup paths validated

Template
See _template.cpp for a minimal program skeleton with error handling and cleanup stages.


