# MQTT Protocol and Specification Compliance

## Scope
MQTT 3.1.1 and MQTT 5.0 compliance across parser, encoder, state machines, and error handling.

## MQTT 3.1.1 Essentials
* CONNECT flags and Clean Session semantics
* Keep Alive handling (PINGREQ/PINGRESP)
* SUBSCRIBE/UNSUBSCRIBE with wildcards `+` and `#`
* PUBLISH with DUP/QoS/RETAIN flags semantics
* QoS pipelines: 0 (fire-and-forget), 1 (PUBACK), 2 (PUBREC→PUBREL→PUBCOMP)
* Will message semantics

## MQTT 5.0 Additions
* Reason Codes on ACKs
* User Properties
* Flow Control: Receive Maximum
* Topic Aliases (client and server)
* Session Expiry and Server Keep Alive
* Subscription Identifiers and Shared Subscriptions `$share/<group>/...`
* Request/Response correlation data

## Validation and Error Handling
* Strict Remaining Length varint parsing with overflow checks
* Malformed packet detection and DISCONNECT with Reason Code (v5)
* Throttling and ban policies for abuse detection

## Interoperability
* Conformance matrix against Paho clients (3.1.1 and 5.0)
* Fuzz corpus for all control packets

## Documentation
* Each handler documents legal transitions and timers
* Tables of Reason Codes and their use
