import argparse
import json
import os
import shutil
import subprocess
import sys
import time


def run(binary, shot, extra, timeout):
    env = dict(os.environ)
    env.update(extra)
    started = time.time()
    r = subprocess.run([binary, shot], env=env, capture_output=True, text=True, errors='replace', timeout=timeout)
    print(f'  {os.path.basename(binary)} exited {r.returncode} after {time.time() - started:.0f}s')
    for line in (r.stdout + r.stderr).splitlines():
        print('   ', line)
    return r


def check(ok, what, failures):
    print(('  ok    ' if ok else '  FAIL  ') + what)
    if not ok:
        failures.append(what)


def runnable(path):
    return os.path.isfile(path) and (os.name == 'nt' or os.access(path, os.X_OK))


def config_version(root):
    path = os.path.join(root, 'baseq3', 'launcher.cfg')
    if not os.path.isfile(path):
        return None
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f.read().splitlines():
            if line.startswith('version '):
                return line.split(' ', 1)[1].strip()
    return None


def leftovers(root):
    found = []
    for dirpath, _, filenames in os.walk(root):
        for name in filenames:
            if name.endswith(('.new', '.part', '.old')):
                found.append(os.path.join(dirpath, name))
    return found


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--launcher', required=True)
    ap.add_argument('--json', required=True)
    ap.add_argument('--work', required=True)
    ap.add_argument('--game', required=True)
    ap.add_argument('--self', required=True, dest='self_name')
    ap.add_argument('--link', required=True)
    ap.add_argument('--version', required=True)
    ap.add_argument('--timeout', type=int, default=900)
    args = ap.parse_args()

    with open(args.json, encoding='utf-8') as f:
        latest = json.load(f)['tag_name'].lstrip('v')
    work = os.path.abspath(args.work)
    if os.path.isdir(work):
        shutil.rmtree(work)
    root = os.path.join(work, 'install')
    desktop = os.path.join(work, 'desktop')
    os.makedirs(desktop)
    failures = []

    print(f'fresh install of {latest} into {root}')
    run(os.path.abspath(args.launcher), os.path.join(work, 'install.png'), {
        'LAUNCHER_JSON': os.path.abspath(args.json),
        'LAUNCHER_STEPS': '1',
        'LAUNCHER_DIR': root,
        'LAUNCHER_DESKTOP': desktop + os.sep,
        'LAUNCHER_NOSTEAM': '1',
        'LAUNCHER_LANG': 'es',
    }, args.timeout)
    game = os.path.join(root, args.game)
    launcher = os.path.join(root, args.self_name)
    check(runnable(game), f'game is in place and runnable: {args.game}', failures)
    check(runnable(launcher), f'launcher copied itself and is runnable: {args.self_name}', failures)
    check(config_version(root) == latest, f'launcher.cfg records {latest} (found {config_version(root)})', failures)
    check(os.path.lexists(os.path.join(desktop, args.link)), f'desktop shortcut {args.link}', failures)
    check(not leftovers(root), 'no .new, .part or .old files left', failures)
    check(os.path.isfile(os.path.join(work, 'install.png')), 'install screenshot saved', failures)

    if runnable(launcher):
        print(f'update from 0.0.1 to {latest}')
        with open(os.path.join(root, 'baseq3', 'launcher.cfg'), 'w', encoding='ascii', newline='\n') as f:
            f.write(f'language es\nversion 0.0.1\nby {args.version}')
        old = time.time() - 86400 * 365
        os.utime(game, (old, old))
        run(launcher, os.path.join(work, 'update.png'), {
            'LAUNCHER_JSON': os.path.abspath(args.json),
            'LAUNCHER_UPDATE': '1',
            'LAUNCHER_NOSTEAM': '1',
            'LAUNCHER_LANG': 'es',
        }, args.timeout)
        check(config_version(root) == latest, f'launcher.cfg records {latest} after the update (found {config_version(root)})', failures)
        check(os.path.getmtime(game) > old + 3600, 'game files were replaced by the update', failures)
        check(runnable(game), 'game is still runnable after the update', failures)
        check(not leftovers(root), 'no .new, .part or .old files left after the update', failures)
        check(os.path.isfile(os.path.join(work, 'update.png')), 'update screenshot saved', failures)

    if sys.platform == 'darwin':
        app = os.path.join(root, args.game.split('/Contents/')[0])
        r = subprocess.run(['codesign', '--verify', '--strict', app], capture_output=True, text=True)
        check(r.returncode == 0, f'game bundle signature still valid ({(r.stdout + r.stderr).strip() or "valid"})', failures)

    print('result:', 'FAILED ' + str(len(failures)) if failures else 'passed')
    sys.exit(1 if failures else 0)


if __name__ == '__main__':
    main()
