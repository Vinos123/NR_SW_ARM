"""Package source and teaching material, excluding all local build caches."""
from pathlib import Path
import zipfile

root = Path(__file__).resolve().parents[1]
archive = root / 'dist/pusch_lab_8weeks.zip'
archive.parent.mkdir(exist_ok=True)
with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as output:
    for path in sorted(root.rglob('*')):
        relative = path.relative_to(root)
        if (not path.is_file() or path.name == '.DS_Store' or
            any(part in {'build', 'dist', '.git', '__pycache__', '.idea'} or
                part.startswith('cmake-build-') for part in relative.parts)):
            continue
        output.write(path, Path('pusch_lab') / relative)
with zipfile.ZipFile(archive) as output:
    assert output.testzip() is None
    assert not any(p.endswith('/CMakePresets.json') for p in output.namelist())
    for number in range(1, 25):
        assert f'pusch_lab/lessons/{number:02}/CMakeLists.txt' in output.namelist()
print(f'{archive}: {archive.stat().st_size} bytes; archive CRC and entries PASS')
