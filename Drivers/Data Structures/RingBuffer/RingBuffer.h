/*
 * RingBuffer.h
 *
 *  Created on: Oct 11, 2019
 *      Author: Embedded Artistry
 *  Found here: https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/circular_buffer.cpp
 *  I changed this to be a little more optimized (no mutex locks/unlocks if they're not needed) and removed all references to the standard library
 */

#pragma once
#include <FreeRTOS.h>
#include <semphr.h>

namespace SatLib
{
    template <class T>
    class RingBuffer {
    public:
        explicit RingBuffer(T* storage, size_t size) :
            max_size_(size)
        {
            buf_ = storage;
            mutex_ = xSemaphoreCreateMutexStatic(&mutex_buffer_);
        }

        inline bool takeMutex(TickType_t ticksToWait)
        {
            return xSemaphoreTakeMutex(mutex_, ticksToWait) == pdPASS;
        }

        inline bool takeMutexFromISR()
        {
            return xSemaphoreTakeMutexFromISR(mutex_, NULL) == pdPASS;
        }

        inline void giveMutex()
        {
            xSemaphoreGive(mutex_);
        }

        inline void giveMutexFromISR()
        {
            xSemaphoreGiveFromISR(mutex_, NULL);
        }

        void put(T item)
        {
            buf_[head_] = item;

            if(full_)
            {
                tail_ = (tail_ + 1) % max_size_;
            }

            head_ = (head_ + 1) % max_size_;

            full_ = head_ == tail_;
        }

        T get()
        {
            //Read data and advance the tail (we now have a free space)
            T val = buf_[tail_];

            full_ = false;
            tail_ = (tail_ + 1) % max_size_;

            return val;
        }

        void reset()
        {
            head_ = tail_;
            full_ = false;
        }

        inline bool empty() const
        {
            //if head and tail are equal, we are empty
            return (!full_ && (head_ == tail_));
        }

        inline bool full() const
        {
            //If tail is ahead the head by 1, we are full
            return full_;
        }

        inline size_t capacity() const
        {
            return max_size_;
        }

        inline size_t size() const
        {
            size_t size = max_size_;

            if(!full_)
            {
                if(head_ >= tail_)
                {
                    size = head_ - tail_;
                }
                else
                {
                    size = max_size_ + head_ - tail_;
                }
            }

            return size;
        }

    private:
        SemaphoreHandle_t mutex_;
        StaticSemaphore_t mutex_buffer_;
        T * buf_;
        size_t head_ = 0;
        size_t tail_ = 0;
        const size_t max_size_;
        bool full_ = 0;
    };
}
