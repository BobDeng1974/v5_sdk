import sys
import gdb

# Update module path.
dir_ = '/home/wanlong/workspace/v5/out/v5-hdmi/staging_dir/target/host/share/glib-2.0/gdb'
if not dir_ in sys.path:
    sys.path.insert(0, dir_)

from gobject_gdb import register
register (gdb.current_objfile ())
