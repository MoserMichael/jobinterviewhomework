from CountWords import CountWords
from Heap import Heap
import io
import unittest

class HeapBackend:

    def __init__(self):
        self.txt_buffer = []

        # for top five elements
        self.top_five_min_heap = Heap()

        # for returning min item
        self.min_heap = Heap()

        # for maintaining median
        self.median_min_heap = Heap()
        self.median_max_heap = Heap()

    def add_text(self, txt):
        self.txt_buffer.append(txt)

    def get_stats(self):
        self._merge_update()
        return self._format()

    def _format(self):
        out = io.StringIO()
        self._format_tops(out)
        self._format_least(out)
        self._format_median(out)

        return out.getvalue()

    def _format_median(self, out):
        med = self._get_median()
        if med is None:
            out.write("\nmedian: empty-list\n\n")
            return
        
        out.write(f"median {med}\n")


    def _format_least(self, out):
        if self.min_heap.len() != 0:
            out.write(f"\nleaset: {self.min_heap.heap[0][0]}\n\n")
        else:
            out.write("\nleaset: empty-list\n\n")

    def _format_tops(self, out):
        tops = self.top_five_min_heap.heap.copy()
        tops.sort(reverse=True)

        for idx in range( len(tops) ):
            item =  tops[idx]
            out.write(f"{item[1]}: {item[0]}\n")


    def _merge_update(self):
        wcount = CountWords()
        for buf in self.txt_buffer:
            wcount.process(buf)
        self.txt_buffer = []
    
        self._merge_count(wcount)

    def _merge_count(self, wcount):
        for key, count in wcount.count_words.items():
            self._add_top_five(key, count)
            self._add_min_heap(key, count)
            self._add_median_heap(key, count)

    def _add_median_heap(self, key, count):
        has, idx = self.median_min_heap.has_key(key)
        if has:
            self.median_min_heap.heap_del(idx)
      
        has, idx = self.median_max_heap.has_key(key)
        if has:
            self.median_max_heap.heap_del(idx)
    
        # see https://leetcode.com/problems/find-median-from-data-stream/solutions/3238662/295-time-97-64-solution-with-step-by-step-explanation/

        if self.median_max_heap.len() == self.median_min_heap.len():
            self.median_max_heap.heappush( (-count, key) )        
            entry = self.median_max_heap.heappop()
            
            self.median_min_heap.heappush( (-entry[0], entry[1]) )
        else:
            self.median_min_heap.heappush( (count, key) )
            entry = self.median_min_heap.heappop()                    
            self.median_max_heap.heappush( (-entry[0], entry[1]) )

        #print(f"max_heap: {self.median_max_heap.heap} min_heap: {self.median_min_heap.heap}") 

    def _get_median(self):
        if self.median_min_heap.len() == 0 and self.median_max_heap.len() == 0:
            return None

        if self.median_min_heap.len() == self.median_max_heap.len():
            assert self.median_min_heap.len() != 0
            assert self.median_max_heap.len() != 0

            return (self.median_min_heap.heap[0][0] - self.median_max_heap.heap[0][0]) / 2.0
        else:
            assert self.median_min_heap.len() != 0

            return self.median_min_heap.heap[0][0] / 2
           
    
    def _add_top_five(self, key, count):
        has, idx = self.top_five_min_heap.has_key(key)
        if has:
            self.top_five_min_heap.heap_del(idx)

        if self.top_five_min_heap.len() < 5:
            self.top_five_min_heap.heappush( ( count, key ) )
        else:
            if self.top_five_min_heap.heap[0][0] < count:
                self.top_five_min_heap.heappop()
                self.top_five_min_heap.heappush( (count, key) )

    def _add_min_heap(self, key, count):
        return HeapBackend._update(key, count, self.min_heap)

    def _update(key, count, heap):
        has, idx = heap.has_key(key)
        if has:
            heap.heap_del(idx)
        heap.heappush( (count, key) )


class TestHeapBackend(unittest.TestCase):

    def test_basic(self):
        bkg = HeapBackend()
        bkg.add_text("ball,eggs,pool,dart,ball,ball")
        bkg.add_text("table,eggs,pool,mouse,ball,eggs")
        bkg.add_text("table,mouse")
        expected = """
ball: 4
eggs: 3
table: 2
pool: 2
mouse: 2

leaset: 1

median 2.0
"""
        assert expected.strip() == bkg.get_stats().strip()

if __name__ == '__main__':
    unittest.main()     


