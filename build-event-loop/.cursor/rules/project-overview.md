# Event Loop Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, and Amazon. The event loop must meet enterprise production standards and serve as a foundation for high performance servers and brokers.

## Purpose
Provide a production grade event loop and reactor framework in C and C plus plus that scales from single core to many core systems while maintaining predictable latency and strong observability.

## Scope
* Applies to all C and C plus plus code in build event loop directory
* Extends repository root rules
* Covers I O models, timers, scheduling, queues and backpressure, threading, observability, tuning, testing, performance, security, and operations

## Components
1. Architecture and Design
2. IO Models
3. Timers and Scheduling
4. Task Queues and Backpressure
5. Concurrency and Threading
6. Networking and OS Tuning
7. Observability
8. Testing and Validation
9. Performance Optimization
10. Security and Safety
11. Operations and Admin
