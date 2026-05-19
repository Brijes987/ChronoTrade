# ChronoTrade API Reference

This document describes the gRPC API for interacting with the ChronoTrade trading engine.

## Table of Contents

- [Overview](#overview)
- [Service Definition](#service-definition)
- [Message Types](#message-types)
- [API Methods](#api-methods)
- [Usage Examples](#usage-examples)
- [Error Handling](#error-handling)

## Overview

ChronoTrade exposes a gRPC API for:
- Submitting and managing orders
- Retrieving market data
- Streaming real-time updates

**Base URL**: `localhost:50051` (default)  
**Protocol**: gRPC over HTTP/2  
**Authentication**: Not implemented (add JWT/mTLS for production)

## Service Definition

```protobuf
service TradingService {
    rpc SubmitOrder(OrderRequest) returns (OrderResponse);
    rpc CancelOrder(CancelRequest) returns (CancelResponse);
    rpc GetMarketData(MarketDataRequest) returns (MarketDataResponse);
    rpc StreamMarketData(MarketDataRequest) returns (stream MarketDataResponse);
}
```

## Message Types

### Enums

#### Side
```protobuf
enum Side {
    BUY = 0;
    SELL = 1;
}
```

#### OrderType
```protobuf
enum OrderType {
    MARKET = 0;
    LIMIT = 1;
    STOP = 2;
    STOP_LIMIT = 3;
}
```

#### OrderStatus
```protobuf
enum OrderStatus {
    PENDING = 0;
    ACCEPTED = 1;
    PARTIALLY_FILLED = 2;
    FILLED = 3;
    CANCELLED = 4;
    REJECTED = 5;
}
```

### Request Messages

#### OrderRequest
Submit a new order to the trading engine.

```protobuf
message OrderRequest {
    string symbol = 1;        // Trading symbol (e.g., "AAPL", "GOOGL")
    Side side = 2;            // BUY or SELL
    OrderType type = 3;       // Order type
    int64 price = 4;          // Price in fixed-point (e.g., 15000 = $150.00)
    uint64 quantity = 5;      // Order quantity
    string client_id = 6;     // Client identifier for tracking
}
```

**Field Details:**
- `symbol`: Must be a valid trading symbol. Case-sensitive.
- `side`: Direction of the order (BUY or SELL)
- `type`: Order type determines execution behavior
  - `MARKET`: Execute immediately at best available price
  - `LIMIT`: Execute only at specified price or better
  - `STOP`: Trigger market order when price reaches stop level
  - `STOP_LIMIT`: Trigger limit order when price reaches stop level
- `price`: Fixed-point representation (divide by 100 for dollars)
  - Example: 15000 = $150.00, 15050 = $150.50
  - For MARKET orders, price is ignored
- `quantity`: Number of shares/contracts to trade
- `client_id`: Optional identifier for client-side tracking

#### CancelRequest
Cancel an existing order.

```protobuf
message CancelRequest {
    uint64 order_id = 1;      // Order ID to cancel
    string client_id = 2;     // Client identifier for verification
}
```

**Field Details:**
- `order_id`: The unique order ID returned from SubmitOrder
- `client_id`: Must match the client_id used when submitting the order

#### MarketDataRequest
Request market data for a symbol.

```protobuf
message MarketDataRequest {
    string symbol = 1;        // Trading symbol
}
```

### Response Messages

#### OrderResponse
Response after submitting an order.

```protobuf
message OrderResponse {
    uint64 order_id = 1;      // Unique order identifier
    OrderStatus status = 2;   // Current order status
    string message = 3;       // Human-readable status message
    int64 timestamp = 4;      // Server timestamp (nanoseconds since epoch)
}
```

**Status Codes:**
- `PENDING`: Order received, awaiting processing
- `ACCEPTED`: Order accepted and entered into order book
- `REJECTED`: Order rejected (see message for reason)

**Common Rejection Reasons:**
- Invalid symbol
- Invalid price (negative or zero for LIMIT orders)
- Invalid quantity (zero or negative)
- Risk limits exceeded
- Market closed

#### CancelResponse
Response after canceling an order.

```protobuf
message CancelResponse {
    bool success = 1;         // True if cancellation succeeded
    string message = 2;       // Status message
    int64 timestamp = 3;      // Server timestamp (nanoseconds since epoch)
}
```

**Success Cases:**
- Order successfully cancelled
- Order already filled (success=true, message indicates already filled)

**Failure Cases:**
- Order ID not found
- Order already cancelled
- Client ID mismatch

#### MarketDataResponse
Market data snapshot for a symbol.

```protobuf
message MarketDataResponse {
    string symbol = 1;        // Trading symbol
    int64 bid_price = 2;      // Best bid price (fixed-point)
    int64 ask_price = 3;      // Best ask price (fixed-point)
    uint64 bid_size = 4;      // Total quantity at best bid
    uint64 ask_size = 5;      // Total quantity at best ask
    int64 timestamp = 6;      // Market data timestamp (nanoseconds)
}
```

**Field Details:**
- `bid_price`: Highest price buyers are willing to pay
- `ask_price`: Lowest price sellers are willing to accept
- `bid_size`: Total quantity available at bid price
- `ask_size`: Total quantity available at ask price
- `timestamp`: When this market data was generated

**Spread Calculation:**
```
spread = ask_price - bid_price
mid_price = (bid_price + ask_price) / 2
```

## API Methods

### SubmitOrder

Submit a new order to the trading engine.

**Request**: `OrderRequest`  
**Response**: `OrderResponse`  
**Latency**: <500ns (P99)

**Example Request:**
```json
{
  "symbol": "AAPL",
  "side": "BUY",
  "type": "LIMIT",
  "price": 15000,
  "quantity": 100,
  "client_id": "client_123"
}
```

**Example Response:**
```json
{
  "order_id": 1234567890,
  "status": "ACCEPTED",
  "message": "Order accepted and entered into order book",
  "timestamp": 1697654321000000000
}
```

**Error Cases:**
- `INVALID_ARGUMENT`: Invalid symbol, price, or quantity
- `FAILED_PRECONDITION`: Market closed or trading halted
- `RESOURCE_EXHAUSTED`: Risk limits exceeded

### CancelOrder

Cancel an existing order.

**Request**: `CancelRequest`  
**Response**: `CancelResponse`  
**Latency**: <200ns (P99)

**Example Request:**
```json
{
  "order_id": 1234567890,
  "client_id": "client_123"
}
```

**Example Response:**
```json
{
  "success": true,
  "message": "Order cancelled successfully",
  "timestamp": 1697654321000000000
}
```

**Error Cases:**
- `NOT_FOUND`: Order ID does not exist
- `PERMISSION_DENIED`: Client ID mismatch
- `FAILED_PRECONDITION`: Order already filled or cancelled

### GetMarketData

Get current market data snapshot for a symbol.

**Request**: `MarketDataRequest`  
**Response**: `MarketDataResponse`  
**Latency**: <100μs (P99)

**Example Request:**
```json
{
  "symbol": "AAPL"
}
```

**Example Response:**
```json
{
  "symbol": "AAPL",
  "bid_price": 14950,
  "ask_price": 15050,
  "bid_size": 1000,
  "ask_size": 1500,
  "timestamp": 1697654321000000000
}
```

**Error Cases:**
- `NOT_FOUND`: Symbol does not exist
- `UNAVAILABLE`: Market data not available

### StreamMarketData

Stream real-time market data updates for a symbol.

**Request**: `MarketDataRequest`  
**Response**: `stream MarketDataResponse`  
**Update Frequency**: ~100μs (as market changes)

**Example Request:**
```json
{
  "symbol": "AAPL"
}
```

**Example Stream Response:**
```json
// Update 1
{
  "symbol": "AAPL",
  "bid_price": 14950,
  "ask_price": 15050,
  "bid_size": 1000,
  "ask_size": 1500,
  "timestamp": 1697654321000000000
}

// Update 2 (100μs later)
{
  "symbol": "AAPL",
  "bid_price": 14960,
  "ask_price": 15050,
  "bid_size": 1200,
  "ask_size": 1500,
  "timestamp": 1697654321000100000
}
```

**Stream Behavior:**
- Updates sent only when market data changes
- Client receives updates until stream is closed
- Automatic reconnection on network failure (client responsibility)

**Error Cases:**
- `NOT_FOUND`: Symbol does not exist
- `RESOURCE_EXHAUSTED`: Too many concurrent streams

## Usage Examples

### Python Client

```python
import grpc
import trading_api_pb2
import trading_api_pb2_grpc

# Connect to trading engine
channel = grpc.insecure_channel('localhost:50051')
stub = trading_api_pb2_grpc.TradingServiceStub(channel)

# Submit a limit order
order_request = trading_api_pb2.OrderRequest(
    symbol='AAPL',
    side=trading_api_pb2.BUY,
    type=trading_api_pb2.LIMIT,
    price=15000,  # $150.00
    quantity=100,
    client_id='python_client_1'
)

response = stub.SubmitOrder(order_request)
print(f"Order ID: {response.order_id}, Status: {response.status}")

# Get market data
market_request = trading_api_pb2.MarketDataRequest(symbol='AAPL')
market_data = stub.GetMarketData(market_request)
print(f"Bid: ${market_data.bid_price/100:.2f}, Ask: ${market_data.ask_price/100:.2f}")

# Stream market data
for update in stub.StreamMarketData(market_request):
    print(f"[{update.timestamp}] {update.symbol}: "
          f"Bid ${update.bid_price/100:.2f} x {update.bid_size}, "
          f"Ask ${update.ask_price/100:.2f} x {update.ask_size}")
```

### C++ Client

```cpp
#include <grpcpp/grpcpp.h>
#include "trading_api.grpc.pb.h"

int main() {
    // Connect to trading engine
    auto channel = grpc::CreateChannel(
        "localhost:50051", 
        grpc::InsecureChannelCredentials()
    );
    auto stub = trading_api::TradingService::NewStub(channel);
    
    // Submit a limit order
    trading_api::OrderRequest request;
    request.set_symbol("AAPL");
    request.set_side(trading_api::BUY);
    request.set_type(trading_api::LIMIT);
    request.set_price(15000);  // $150.00
    request.set_quantity(100);
    request.set_client_id("cpp_client_1");
    
    trading_api::OrderResponse response;
    grpc::ClientContext context;
    
    grpc::Status status = stub->SubmitOrder(&context, request, &response);
    
    if (status.ok()) {
        std::cout << "Order ID: " << response.order_id() 
                  << ", Status: " << response.status() << std::endl;
    } else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }
    
    return 0;
}
```

### Go Client

```go
package main

import (
    "context"
    "log"
    "google.golang.org/grpc"
    pb "path/to/trading_api"
)

func main() {
    // Connect to trading engine
    conn, err := grpc.Dial("localhost:50051", grpc.WithInsecure())
    if err != nil {
        log.Fatalf("Failed to connect: %v", err)
    }
    defer conn.Close()
    
    client := pb.NewTradingServiceClient(conn)
    
    // Submit a limit order
    request := &pb.OrderRequest{
        Symbol:   "AAPL",
        Side:     pb.Side_BUY,
        Type:     pb.OrderType_LIMIT,
        Price:    15000, // $150.00
        Quantity: 100,
        ClientId: "go_client_1",
    }
    
    response, err := client.SubmitOrder(context.Background(), request)
    if err != nil {
        log.Fatalf("SubmitOrder failed: %v", err)
    }
    
    log.Printf("Order ID: %d, Status: %v", response.OrderId, response.Status)
}
```

## Error Handling

### gRPC Status Codes

| Code | Description | Common Causes |
|------|-------------|---------------|
| `OK` | Success | Request completed successfully |
| `INVALID_ARGUMENT` | Invalid input | Bad symbol, price, or quantity |
| `NOT_FOUND` | Resource not found | Unknown order ID or symbol |
| `PERMISSION_DENIED` | Access denied | Client ID mismatch |
| `RESOURCE_EXHAUSTED` | Quota exceeded | Risk limits or rate limits |
| `FAILED_PRECONDITION` | Invalid state | Market closed, order already filled |
| `UNAVAILABLE` | Service unavailable | Trading engine down or overloaded |
| `INTERNAL` | Internal error | Unexpected server error |

### Error Response Format

All error responses include:
- `code`: gRPC status code
- `message`: Human-readable error description
- `details`: Additional context (when available)

**Example Error Response:**
```json
{
  "code": "INVALID_ARGUMENT",
  "message": "Invalid price: must be positive for LIMIT orders",
  "details": {
    "field": "price",
    "value": -15000,
    "constraint": "positive"
  }
}
```

### Retry Strategy

**Recommended retry policy:**
- `UNAVAILABLE`: Retry with exponential backoff (max 3 attempts)
- `RESOURCE_EXHAUSTED`: Retry after delay (respect rate limits)
- `INTERNAL`: Retry once, then escalate
- `INVALID_ARGUMENT`: Do not retry (fix input first)
- `NOT_FOUND`: Do not retry (resource doesn't exist)

**Example Retry Logic (Python):**
```python
import time
from grpc import StatusCode

def submit_order_with_retry(stub, request, max_retries=3):
    for attempt in range(max_retries):
        try:
            return stub.SubmitOrder(request)
        except grpc.RpcError as e:
            if e.code() == StatusCode.UNAVAILABLE:
                if attempt < max_retries - 1:
                    time.sleep(2 ** attempt)  # Exponential backoff
                    continue
            raise
```

## Rate Limits

**Current limits** (subject to change):
- Order submission: 10,000 orders/second per client
- Market data requests: 1,000 requests/second per client
- Concurrent streams: 100 per client

**Rate limit headers** (future):
- `X-RateLimit-Limit`: Maximum requests per window
- `X-RateLimit-Remaining`: Remaining requests in current window
- `X-RateLimit-Reset`: Time when limit resets (Unix timestamp)

## Performance Characteristics

| Operation | P50 Latency | P99 Latency | Throughput |
|-----------|-------------|-------------|------------|
| SubmitOrder | 387ns | 512ns | 2.4M ops/s |
| CancelOrder | 156ns | 203ns | 6.4M ops/s |
| GetMarketData | 89ns | 118ns | 11.2M ops/s |
| StreamMarketData | ~100μs | ~150μs | 50K updates/s |

**Note**: Latencies measured server-side. Network latency adds 0.1-1ms depending on distance.

## Future API Enhancements

Planned additions:
- [ ] Authentication (JWT tokens, mTLS)
- [ ] Order modification (ModifyOrder RPC)
- [ ] Bulk order submission (SubmitOrders with batching)
- [ ] Historical market data (GetHistoricalData RPC)
- [ ] Account management (GetAccount, GetPositions RPCs)
- [ ] WebSocket API (for browser clients)
- [ ] REST API (for simpler integrations)

---

*Last updated: 2024-10-18*  
*API version: 1.0.0*  
*ChronoTrade version: 1.0.0*
