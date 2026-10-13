"""Configure/build all lessons independently, run answers, inspect dependencies.

Example: python tools/validate_lesson_builds.py --build-root build/standalone-check
Pass --generator Ninja only if that is your chosen/installed generator.
The CMake projects themselves never select a compiler or generator.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import json
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build-root', required=True)
parser.add_argument('--generator')
parser.add_argument('--workers', type=int, default=2)
parser.add_argument('--check-initial-exercises', action='store_true',
                    help='For untouched TODOs only; omit once you implement them.')
parser.add_argument('--report', default='docs/results/standalone_builds.json')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
build_root = Path(args.build_root).resolve()

def run(command, log):
    result = subprocess.run(command, capture_output=True, text=True)
    log.append('$ ' + ' '.join(map(str, command)) + '\n' + result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError('\n'.join(log))
    return result

def validate(number):
    label = f'{number:02}'
    build = build_root / label
    if build.exists():
        raise RuntimeError(f'Use a clean build root; {build} already exists')
    build.mkdir(parents=True)
    log = []
    configure = ['cmake', '-S', str(root / 'lessons' / label), '-B', str(build),
                 '-DCMAKE_BUILD_TYPE=Debug']
    if args.generator:
        configure += ['-G', args.generator]
    try:
        run(configure, log)
        run(['cmake', '--build', str(build), '--parallel', '2'], log)
        run(['ctest', '--test-dir', str(build), '--output-on-failure'], log)
        suffix = '.exe' if (build / f'lesson_{label}_solution.exe').exists() else ''
        solution = build / f'lesson_{label}_solution{suffix}'
        exercise = build / f'lesson_{label}_exercise{suffix}'
        result = subprocess.run([str(solution)], capture_output=True, text=True)
        assert result.returncode in [0, 77], result.stdout + result.stderr
        compiled = json.loads((build / 'compile_commands.json').read_text())
        sources = sorted({str(Path(entry['file']).resolve().relative_to(root))
                          for entry in compiled})
        assert not any('googletest' in s for s in sources), sources
        assert not any(s.startswith('lessons/') and not s.startswith(f'lessons/{label}/')
                       for s in sources), sources
        if number in [1, 4, 10]:
            assert not any('decoder' in s for s in sources), sources
        if number in [1, 4]:
            assert not any('ldpc' in s or 'kissfft' in s or 'ofdm' in s or
                           s == 'src/link.cc' for s in sources), sources
        if number == 1:
            assert [s for s in sources if s.startswith('src/')] == ['src/runtime.cc']
        if args.check_initial_exercises:
            initial = subprocess.run([str(exercise)], capture_output=True, text=True)
            assert initial.returncode in ([1, 77] if number >= 23 else [1])
            if initial.returncode == 1:
                assert 'NOT IMPLEMENTED' in initial.stderr
        return {'lesson': label, 'status': 'skip' if result.returncode == 77 else 'pass',
                'sources': sources, 'generator': args.generator or 'CMake default'}
    finally:
        (build / 'validation.log').write_text('\n'.join(log), encoding='utf-8')

results = []
with ThreadPoolExecutor(max_workers=args.workers) as pool:
    jobs = {pool.submit(validate, i): i for i in range(1, 25)}
    for future in as_completed(jobs):
        result = future.result()
        results.append(result)
        print(f'lesson {result["lesson"]}: {result["status"]}', flush=True)
results.sort(key=lambda result: result['lesson'])
report = root / args.report
report.parent.mkdir(parents=True, exist_ok=True)
report.write_text(json.dumps(results, indent=2) + '\n', encoding='utf-8')
print(f'Validated {len(results)} standalone projects; report: {report}', flush=True)
