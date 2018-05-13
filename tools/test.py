
import os
import subprocess
import sys


cmd = './build/sjs'
env = {'SJS_PATH': os.getenv('SJS_PATH')}

results = {}


def green(text):
    return '\033[92m{}\033[0m'.format(text)


def red(text):
    return '\033[91m{}\033[0m'.format(text)


def run_test(f):
    test_file = os.path.basename(f)
    sys.stdout.write('Running {:50}'.format(test_file))
    sys.stdout.flush()
    try:
        subprocess.check_output('%s %s' % (cmd, f), stderr=subprocess.STDOUT, shell=True, env=env)
    except subprocess.CalledProcessError as e:
        sys.stdout.write(red('ERROR'))
        sys.stdout.write('\n')
        results[test_file] = e
    else:
        sys.stdout.write(green('OK'))
        sys.stdout.write('\n')
        results[test_file] = None


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise RuntimeError('run as follows: tools/test.py test_directory')

    test_dir = sys.argv[1]
    for f in sorted(os.listdir(test_dir)):
        if f.startswith('test-') and f.endswith('.js'):
            run_test(os.path.join(test_dir, f))

    sys.stdout.write('\n')

    if results and all(val is None for val in results.values()):
        print(green('All tests passed!'))
        sys.exit(0)
    else:
        print(red('Some tests failed :-('))
        sys.stdout.write('\n')
        for test, result in results.items():
            if result is not None:
                print('{}:\n{}\n'.format(test, result.output))
        sys.exit(1)

