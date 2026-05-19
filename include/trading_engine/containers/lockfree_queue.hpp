#pragma once

#include <atomic>
#include <memory>
#include <array>

namespace trading_engine::containers {

template<typename T, size_t Capacity = 1024>
class LockFreeQueue {
private:
    struct alignas(64) Node {
        std::atomic<T*> data{nullptr};
        std::atomic<size_t> sequence{0};
    };
    
    static constexpr size_t buffer_mask_ = Capacity - 1;
    static_assert((Capacity & buffer_mask_) == 0, "Capacity must be power of 2");
    
    std::array<Node, Capacity> buffer_;
    alignas(64) std::atomic<size_t> enqueue_pos_{0};
    alignas(64) std::atomic<size_t> dequeue_pos_{0};

public:
    LockFreeQueue() {
        for (size_t i = 0; i < Capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }
    
    bool try_enqueue(T&& item) {
        Node* node;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        
        for (;;) {
            node = &buffer_[pos & buffer_mask_];
            size_t seq = node->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
            
            if (diff == 0) {
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1, 
                    std::memory_order_release, std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false; // Queue is full
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }
        
        T* data = new T(std::move(item));
        node->data.store(data, std::memory_order_release);
        node->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }
    
    bool try_dequeue(T& result) {
        Node* node;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        
        for (;;) {
            node = &buffer_[pos & buffer_mask_];
            size_t seq = node->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
            
            if (diff == 0) {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1,
                    std::memory_order_release, std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false; // Queue is empty
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }
        
        T* data = node->data.load(std::memory_order_acquire);
        result = std::move(*data);
        delete data;
        node->data.store(nullptr, std::memory_order_relaxed);
        node->sequence.store(pos + buffer_mask_ + 1, std::memory_order_release);
        return true;
    }
    
    size_t size() const noexcept {
        size_t enq = enqueue_pos_.load(std::memory_order_acquire);
        size_t deq = dequeue_pos_.load(std::memory_order_acquire);
        return enq - deq;
    }
    
    bool empty() const noexcept {
        return size() == 0;
    }
};

} // namespace trading_engine::containers