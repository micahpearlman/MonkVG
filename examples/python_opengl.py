import sys
import os 

# add the path to the monkvg python bindings to the system path
sys.path.append(os.path.join(os.path.dirname(__file__), '../build/debug/bindings'))
print(sys.path)

import monkvg_py as vg

print(vg.add(1, 2)) # 3