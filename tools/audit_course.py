"""Check lesson completeness, local documentation links and vector hashes."""
from pathlib import Path
import hashlib
import json
import re

root = Path(__file__).resolve().parents[1]
for i in range(1, 25):
    for name in ['README.md', 'exercise.h', 'exercise.cc', 'solution.cc', 'main.cc']:
        assert (root / f'lessons/{i:02}' / name).is_file(), (i, name)
    assert f'lesson_{i:02}::' in (root/f'lessons/{i:02}/main.cc').read_text(), i
for path in [root/'README.md', * (root/'docs').glob('*.md'),
             * (root/'lessons').glob('*/README.md'), root/'third_party/README.md']:
    for target in re.findall(r'\]\(([^)]+)\)', path.read_text(encoding='utf-8')):
        if '://' not in target and not target.startswith('#'):
            assert (path.parent / target.split('#')[0]).exists(), (path, target)
manifest=json.loads((root/'tests/data/manifest.json').read_text())
for name, expected in manifest['files'].items():
    assert hashlib.sha256((root/'tests/data'/name).read_bytes()).hexdigest()==expected,name
for path in [root/'CMakePresets.json', *(root/'.vscode').glob('*.json')]:
    json.loads(path.read_text())
print('24 lesson packages, local links, JSON and reference hashes: PASS')
