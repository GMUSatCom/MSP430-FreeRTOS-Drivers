#pragma once
#include <string.h> // NULL
#include <../RingBuffer/RingBuffer.h> // to improve algorithmic complexity.

namespace SatLib
{
	template<typename T, size_t maxSize>
	class PriorityQueue
	{
	private:
	    struct
	    {
	        T * previous;
	        T * next;
	        T item;
	    }typedef listNode;

	    listNode nodes[maxSize];
	    RingBuffer<listNode> buffer(nodes, maxSize);

	public:

	    PriorityQueue()
	    {
	        size_t i = 0;
	        for(i = 0; i < maxSize; i++)
	            nodes[i].next = nodes[i].previous = NULL;
	    }
	    /**
	     * Adds an item and returns the position of the item.
	     */
	    inline int32_t push(T item)
	    {
	        if(buffer.size() + 1 >= maxSize)
	            return -1;

	        // the following loop only runs until
	        size_t currentPosition;
	        for(currentPosition = 0; currentPosition <= buffer.size(); currentPosition++)
	        {
	            if(item <= buffer.peekPos(currentPosition))
	            {
	                buffer.insert(currentPosition, item);
	                return currentPosition;
	            }
	        }
	        buffer.put(item);
	    }

	    inline T pop()
	    {
	        return buffer.get();
	    }

	    inline size_t capacity()
	    {
	        return maxSize;
	    }

	    inline size_t size()
	    {
	        return buffer.size();
	    }
	};
}
