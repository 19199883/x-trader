# -*- coding: utf-8 -*-

import os
import sys
from docutils.parsers import Parser

sys.path.insert(0, os.path.abspath('.'))


class DummyMarkdownParser(Parser):
    pass


extensions = ['source_parser']
source_suffix = ['.rst', '.md']
source_parsers = {
    '.md': DummyMarkdownParser
}
