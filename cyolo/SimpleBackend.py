from CountWords import CountWords
import io
import unittest

class SimpleBackend:

    def __init__(self):
        self.txt_buffer = []
        self.wcount = CountWords()

    def add_text(self, txt):
        #print(f"add text: {txt}")
        self.txt_buffer.append(txt)

    def get_stats(self):
        for buf in self.txt_buffer:
            self.wcount.process(buf)
        self.txt_buffer = []

        sorted_list = []
        for k,v in self.wcount.count_words.items():
            sorted_list.append( (v, k) )

        sorted_list.sort()

        #print(f"sorted_list: {sorted_list}")

        return self._format(sorted_list)

    
    def _format(self, sorted_list):
        out = io.StringIO()
        self._format_tops(out, sorted_list)
        self._format_least(out, sorted_list)
        self._format_median(out, sorted_list)

        ret=out.getvalue()
        #print(ret)
        return ret 

    def _format_median(self, out, sorted_list):
        if not sorted_list:
            out.write("\nmedian: empty-list\n\n")
            return

        m_idx = len(sorted_list) // 2
        if len(sorted_list) % 2 == 1:
            median = sorted_list[ m_idx ][0]
        else:
            median = (sorted_list[ m_idx-1 ][0] + sorted_list[ m_idx ][0]) / 2
        
        out.write(f"median {median}\n")


    def _format_least(self, out, sorted_list):
        if sorted_list:
            out.write(f"\nleaset: {sorted_list[0][0]}\n\n")
        else:
            out.write("\nleaset: empty-list\n\n")
    
    def _format_tops(self, out, sorted_list):
        for idx in range( min(5,len(sorted_list) ) ):
            item =  sorted_list[ len(sorted_list) - 1 - idx ]
            out.write(f"{item[1]}: {item[0]}\n")



class TestSimpleBackend(unittest.TestCase):

    def test_basic(self):
        bkg = SimpleBackend()
        bkg.add_text("ball,eggs,pool,dart,ball,ball")
        bkg.add_text("table,eggs,pool,mouse,ball,eggs")
        bkg.add_text("table,mouse")
        res = bkg.get_stats()
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
    
    


    
            
