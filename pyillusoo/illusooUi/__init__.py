#!/usr/bin/env python

import inspect
from base import *
from rttrace import *
from element import *
from widget import *
from TabWidget import *

__all__ = sorted(name for name, obj in locals().items()
                 if not (name.startswith('_') or inspect.ismodule(obj)))



__author__ = "Claude BIDEAU"
__copyright__ = "Copyright 2013 FABLAB Lannion"
__credits__ = ["Claude BIDEAU"]
__license__ = "GPL"
__version__ = "0.0.1"
__maintainer__ = "Claude BIDEAU"
__email__ = "claude.bideau@gmail.org"
__status__ = "Development"


if __name__ == '__main__':
    print [name for name, obj in locals().items() if inspect.ismodule(obj)]
    print __all__
