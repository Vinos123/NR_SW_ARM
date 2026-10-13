"""Optional maintenance command: python tools/check_format.py [--fix]."""
from pathlib import Path
import subprocess
import sys
root=Path(__file__).resolve().parents[1]
files=[p for folder in ['include','src','tests','lessons']
       for p in (root/folder).rglob('*') if p.suffix in {'.h','.cc'}]
args=['clang-format','-i'] if '--fix' in sys.argv else ['clang-format','--dry-run','--Werror']
subprocess.run(args+[str(p) for p in files],check=True)
