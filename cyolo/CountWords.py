import re
from collections import defaultdict

class CountWords:
    def __init__(self):
        self.count_words = defaultdict(int)

    def process(self, text):
        for word in text.split(","):
            self.count_words[word] += 1
