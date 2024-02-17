import unittest

class Heap:
    def __init__(self):
        self.heap = []
        self.map_key_to_idx = {}

    def has_key(self, key):
        if key in self.map_key_to_idx:
            return True, self.map_key_to_idx[key]
        return False, None

    def len(self):
        return len(self.heap)

    def empty(self):
        return len(self.heap) != 0

    def top_entry(self):
        self.heap[0]

    def heap_del(self, idx):
        assert len(self.heap) != 0
        
        self.map_key_to_idx.pop( self.__getitemkey__( self.heap[idx] ) )
        #self.heap[idx] = self.heap.pop()
        self._setitem(idx, self.heap.pop() )

        if idx > 0:
            parentpos = (idx - 1) >> 1
            if self.heap[parentpos] > self.heap[idx]:
                self._siftdown(0, idx)
                return

        self._siftup(idx)

    def heappush(self, item):
        """Push item onto heap, maintaining the heap invariant."""
        self.heap.append(item)
        self.map_key_to_idx[ self.__getitemkey__(item) ] = len(self.heap)-1

        self._siftdown(0, len(self.heap)-1)

    def heappop(self):
        """Pop the smallest item off the heap, maintaining the heap invariant."""
        lastelt = self.heap.pop()    # raises appropriate IndexError if heap is empty

        if self.heap:
            returnitem = self.heap[0]

            self.map_key_to_idx.pop( self.__getitemkey__(returnitem) )

            #self.heap[0] = lastelt
            self._setitem(0, lastelt)

            self._siftup(0)
            return returnitem

        self.map_key_to_idx.pop( self.__getitemkey__(lastelt) )

        return lastelt

    # 'heap' is a heap at all indices >= startpos, except possibly for pos.  pos
    # is the index of a leaf with a possibly out-of-order value.  Restore the
    # heap invariant.
    def _siftdown(self, startpos, pos):
        newitem = self.heap[pos]
        # Follow the path to the root, moving parents down until finding a place
        # newitem fits.
        while pos > startpos:
            parentpos = (pos - 1) >> 1
            parent = self.heap[parentpos]
            if newitem < parent:
                #self.heap[pos] = parent
                self._setitem(pos, parent)
                pos = parentpos
                continue
            break
        
        #self.heap[pos] = newitem
        self._setitem(pos, newitem)

    def _siftup(self, pos):
        endpos = len(self.heap)
        startpos = pos
        newitem = self.heap[pos]
        # Bubble up the smaller child until hitting a leaf.
        childpos = 2*pos + 1    # leftmost child position
        while childpos < endpos:
            # Set childpos to index of smaller child.
            rightpos = childpos + 1
            if rightpos < endpos and not self.heap[childpos] < self.heap[rightpos]:
                childpos = rightpos
            
            # Move the smaller child up.
            #self.heap[pos] = self.heap[childpos]
            self._setitem(pos, self.heap[childpos])

            pos = childpos
            childpos = 2*pos + 1
        # The leaf at pos is empty now.  Put newitem there, and bubble it up
        # to its final resting place (by sifting its parents down).
    
        #self.heap[pos] = newitem
        self._setitem(pos, newitem)

        self._siftdown(startpos, pos)

    def _setitem(self, pos, item):
        self.heap[pos] = item
        self.map_key_to_idx[ self.__getitemkey__(item) ] = pos

    # return the key that identifies this item.
    def __getitemkey__(self, item):
        return item[1]

    def verify(self):
        if len(self.heap) == 0:
            return
        self._verify(0)

        assert len(self.map_key_to_idx) == len(self.heap)

        for k, v in self.map_key_to_idx.items():
            assert self.__getitemkey__(self.heap[v]) == k


    def _verify(self, idx):
        childpos = 2*idx+1
        if childpos < len(self.heap):
            assert self.heap[idx] < self.heap[childpos]
            self._verify(childpos)
        
        childpos += 1

        if childpos < len(self.heap):
            assert self.heap[idx] < self.heap[childpos]
            self._verify(childpos)

class TestHeap(unittest.TestCase):

    def test_basic(self):
        heap = Heap()
        for num in range(10, -1, -1):
            heap.heappush( (num, str(num) ) )
            heap.verify()

        print(heap.heap)            
        print(heap.map_key_to_idx)

        for idx in range(0,10,3):
            has, nidx = heap.has_key(str(idx))
            assert has
            print(f"del {idx} at {nidx}")
            heap.heap_del(nidx)
            print(heap.heap)
            heap.verify()

        print(heap.heap)            
        print(heap.map_key_to_idx)



if __name__ == '__main__':
    unittest.main() 
    
