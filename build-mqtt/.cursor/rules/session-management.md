# Session Management

## Clean Start / Session Present
* Implement Clean Start (v5) and 3.1.1 cleanSession semantics
* Persist session state when not clean; remove on expiry

## Expiry and Timers
* Session Expiry Interval (v5); wheel timers for scale
* Will delay interval handling

## Inflight State
* Track QoS1/2 inflight maps; persist for crash recovery
* Enforce per-client inflight windows

## Will Messages
* Store will properties; publish on ungraceful disconnect
* Validate topic and ACL at publish time

## Reauthentication
* Support server initiated reauth (v5) if implemented; otherwise disconnect

## Testing
* Expiry correctness under reconnects and crashes
* Will publish semantics for all disconnect reasons
