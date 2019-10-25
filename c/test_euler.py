from atexit import register
from functools import partial
from itertools import chain
from os import environ, sep, unlink
from pathlib import Path
from platform import machine, processor, system, uname
from pprint import pprint
from random import randrange
from shutil import copy, rmtree, which
from subprocess import check_call, check_output, run
from sys import path
from tempfile import TemporaryFile
from time import sleep
from typing import List, Set, Union
from uuid import uuid4
from warnings import warn

from pytest import fail, fixture, mark, skip, xfail

C_FOLDER = Path(__file__).parent
BUILD_FOLDER = C_FOLDER.joinpath('build', 'test-run-{}'.format(uuid4()))
# each test run is isolated using UUIDs. if a particular test needs to touch something
# outside this folder, it is expected to use a UUID to isolate it as well, and clean up
path.append(str(C_FOLDER.parent.joinpath("python")))

answers = {
    1: 233168,
    2: 4613732,
    3: 6857,
    4: 906609,
    5: 232792560,
    6: 25164150,
    7: 104743,
    8: 23514624000,
    9: 31875000,
    10: 142913828922,
    11: 70600674,
    13: 5537376230,
    14: 837799,
    15: 137846528820,
    16: 1366,
    20: 648,
    25: 4782,
    30: 443839,
    34: 40730,
    76: 190569291,
}

# this is the set of problems where I have the right answer but wrong solution
known_slow: Set[int] = set()

# platform variables section
IN_WINDOWS = system() == 'Windows'
IN_OSX = system() == 'Darwin'
IN_TERMUX = bool(which('termux-setup-storage'))
IN_LINUX = (not IN_TERMUX) and (system() == 'Linux')

if IN_TERMUX:
    BUILD_FOLDER = Path.home().joinpath('build', 'test-run-{}'.format(uuid4()))
    # Termux can't make executable files outside of $HOME

_raw_NO_SLOW = environ.get('NO_SLOW')
try:
    _parsed_NO_SLOW: Union[str, int, None] = int(_raw_NO_SLOW)  # type: ignore
except Exception:
    _parsed_NO_SLOW = _raw_NO_SLOW
_raw_ONLY_SLOW = environ.get('ONLY_SLOW')
try:
    _parsed_ONLY_SLOW: Union[str, int, None] = int(_raw_ONLY_SLOW)  # type: ignore
except Exception:
    _parsed_ONLY_SLOW = _raw_ONLY_SLOW
_raw_NO_OPTIONAL_TESTS = environ.get('NO_OPTIONAL_TESTS')
try:
    _parsed_NO_OPTIONAL_TESTS: Union[str, int, None] = int(_raw_NO_OPTIONAL_TESTS)  # type: ignore
except Exception:
    _parsed_NO_OPTIONAL_TESTS = _raw_NO_OPTIONAL_TESTS

if _parsed_NO_SLOW and _parsed_ONLY_SLOW:
    warn("Test suite told to ignore slow tests AND run only slow tests. Ignoring conflicting options")

# if in Termux, default to NO_SLOW, but allow users to explicitly override that decision
NO_SLOW = ((IN_TERMUX and _parsed_NO_SLOW is None) or _parsed_NO_SLOW) and not _parsed_ONLY_SLOW
ONLY_SLOW = _parsed_ONLY_SLOW and not _parsed_NO_SLOW
NO_OPTIONAL_TESTS = (_parsed_NO_OPTIONAL_TESTS is None and ONLY_SLOW) or _parsed_NO_OPTIONAL_TESTS

# this part isn't necessary, but I like having the binaries include their compile architecture
if IN_LINUX and processor() and ' ' not in processor():
    EXE_EXT = processor()
elif IN_WINDOWS:
    # processor() returns something too verbose in Windows
    EXE_EXT = "x86"
    if machine().endswith('64'):
        EXE_EXT += "_64"
elif IN_OSX:
    # processor() on OSX returns something too vague to be useful
    EXE_EXT = uname().machine.replace('i3', 'x')
elif IN_TERMUX:
    # processor() doesn't seem to work on Termux
    EXE_EXT = check_output('lscpu').split()[1].decode()
else:
    warn("Could not detect system architecture, defaulting to .exe")
    EXE_EXT = "exe"

GCC_BINARY = environ.get('GCC_OVERRIDE', 'gcc')
AOCC_BINARY = environ.get('AOCC_OVERRIDE', 'clang')

# compiler variables section
compilers: List[str] = []

if 'COMPILER_OVERRIDE' in environ:
    compilers.extend(environ['COMPILER_OVERRIDE'].upper().split(','))
else:
    if not (IN_TERMUX and GCC_BINARY == 'gcc') and which(GCC_BINARY):  # Termux maps gcc->clang
        compilers.append('GCC')
    if which('clang'):
        if b'AOCC' in check_output(['clang', '--version']):
            compilers.append('AOCC')
        else:
            compilers.append('CLANG')
    if AOCC_BINARY != 'clang' and which(AOCC_BINARY):
        compilers.append('AOCC')
    for x in ('cl', 'icc'):
        if which(x):
            compilers.append(x.upper())
if not compilers:
    raise RuntimeError("No compilers detected!")

valgrind_compilers = [x for x in compilers if x != 'CL']
COMPILER_LEN = len(max(compilers, key=len))  # make sure compiler fixtures are evenly spaced
V_COMPILER_LEN = len(max(valgrind_compilers or [''], key=len))
BUILD_FOLDER.mkdir(parents=True, exist_ok=True)
CL_NO_64 = False
if 'CL' in compilers:
    OBJ_FOLDER = BUILD_FOLDER.joinpath('objects')
    OBJ_FOLDER.mkdir(exist_ok=True)
    _test_file = BUILD_FOLDER.parent.joinpath('{}.c'.format(uuid4()))
    # need to rename because CL makes object files based on source name
    copy(C_FOLDER.joinpath('assertions', 'x64_assert.c'), _test_file)
    _test_exe = str(BUILD_FOLDER.joinpath('test_cl_64_support-{}.out'.format(uuid4())))
    CL_NO_64 = not (run(['cl', '-Fe:{}'.format(_test_exe), '-Fo{}\\'.format(OBJ_FOLDER), str(_test_file)]).returncode)
    unlink(_test_file)

_test_file = str(C_FOLDER.joinpath('p0000_template.c'))
GCC_NO_64 = False
if EXE_EXT == 'x86_64' and 'GCC' in compilers:
    # MingW GCC sometimes doesn't have 64-bit support on 64-bit targets
    # not knowing this will make the compiler macro test fail
    _test_exe = str(BUILD_FOLDER.joinpath('test_gcc_64_support-{}.out'.format(uuid4())))
    GCC_NO_64 = bool(run([GCC_BINARY, _test_file, '-O0', '-m64', '-o', _test_exe]).returncode)

CLANG_LINK_MATH = CLANG_ARCH = ''
if not IN_WINDOWS:
    CLANG_LINK_MATH = '-lm'
if 'CLANG' in compilers:
    _test_exe = str(BUILD_FOLDER.joinpath('test_clang_arch_native-{}.out'.format(uuid4())))
    CLANG_ARCH = '-march=native' * (not run(['clang', _test_file, '-O0', '-march=native', '-o', _test_exe]).returncode)

SOURCE_TEMPLATE = "{}{}p{{:0>4}}.c".format(C_FOLDER, sep)
EXE_TEMPLATE = "{}{}p{{:0>4}}.{{}}.{}".format(BUILD_FOLDER, sep, EXE_EXT)
# include sep in the recipe so that Windows won't complain

GCC_TEMPLATE = "{} {{}} -O3 {} -lm -Wall -Werror -std=gnu11 -march=native -flto -fwhole-program -o {{}}"
CLANG_TEMPLATE = "{} {{}} -O3 {} {} -Wall -Werror -std=gnu11 {} -o {{}}"

templates = {
    'GCC': GCC_TEMPLATE.format(GCC_BINARY, ''),
    'CLANG': CLANG_TEMPLATE.format('clang', CLANG_LINK_MATH, CLANG_ARCH, '-DAMD_COMPILER=0'),
    'CL': "cl -Fe:{{1}} -Fo{}\\ -O2 -GL -GF -Brepro -WX -W3 -TC {{0}}".format(BUILD_FOLDER.joinpath('objects')),
    'ICC': GCC_TEMPLATE.format('icc', ''),
    'AOCC': CLANG_TEMPLATE.format(AOCC_BINARY, CLANG_LINK_MATH, CLANG_ARCH, '-DAMD_COMPILER=1'),
    'debug': {
        'GCC': GCC_TEMPLATE.format(GCC_BINARY, '-g'),
        'CLANG': CLANG_TEMPLATE.format('clang', CLANG_LINK_MATH, CLANG_ARCH, '-DAMD_COMPILER=0 -g'),
        # CL would go here if I thought it worked with gdb/valgrind
        'ICC': GCC_TEMPLATE.format('icc', '-g'),
        'AOCC': CLANG_TEMPLATE.format(AOCC_BINARY, CLANG_LINK_MATH, CLANG_ARCH, '-DAMD_COMPILER=1 -g'),
    }
}


@register
def cleanup():
    for _ in range(10):
        try:
            rmtree(BUILD_FOLDER)
            break
        except PermissionError:  # Windows might complain an executable is still in use
            sleep(0.5)


@fixture(params=sorted(x.ljust(V_COMPILER_LEN) for x in valgrind_compilers))
def v_compiler(request):  # type: ignore
    return request.param.strip()


@fixture(params=sorted(x.ljust(COMPILER_LEN) for x in compilers))
def compiler(request):  # type: ignore
    return request.param.strip()


# to make sure the benchmarks sort correctly
@fixture(params=("{:03}".format(x) for x in sorted(answers)))
def key(request):  # type: ignore
    return int(request.param)  # reduce casting burden on test


@fixture(params=sorted(chain(
    (x.name for x in C_FOLDER.joinpath("tests").glob("*.c")),
    ("{:03}".format(x) for x in answers)
)))
def c_file(request):  # type: ignore
    try:
        return SOURCE_TEMPLATE.format(int(request.param))
    except Exception:
        return C_FOLDER.joinpath("tests", request.param)


@mark.skipif('NO_OPTIONAL_TESTS')
def test_compiler_macros(compiler):
    exe_name = EXE_TEMPLATE.format("test_compiler_macros", compiler)
    test_path = C_FOLDER.joinpath("tests", "test_compiler_macros.c")
    check_call(templates[compiler].format(test_path, exe_name).split())
    buff = check_output([exe_name])
    flags = [bool(int(x)) for x in buff.split()]
    expect_32 = (compiler == 'GCC' and GCC_NO_64) or (compiler == 'CL' and CL_NO_64)
    assert flags[0] == (compiler == "CL")
    assert flags[1] == (compiler == "CLANG")
    assert flags[2] == (compiler == "GCC")
    assert flags[3] == (compiler == "ICC")
    assert flags[4] == (compiler == "AOCC")
    assert flags[5] == (EXE_EXT == "x86" or expect_32)
    assert flags[6] == (EXE_EXT == "x86_64" and not expect_32)
    assert flags[7] == (EXE_EXT not in ("x86", "x86_64", "exe"))


@mark.skipif('NO_OPTIONAL_TESTS')
def test_deterministic_build(c_file, compiler):
    if IN_WINDOWS and compiler != 'CL':  # mingw gcc doesn't seem to make reproducible builds
        skip()
    exe_name1 = EXE_TEMPLATE.format("deterministic-build-{}".format(uuid4()), compiler)
    exe_name2 = EXE_TEMPLATE.format("deterministic-build-{}".format(uuid4()), compiler)
    environ['SOURCE_DATE_EPOCH'] = '1'
    environ['ZERO_AR_DATE'] = 'true'
    check_call(templates[compiler].format(c_file, exe_name1).split())
    sleep(2)
    check_call(templates[compiler].format(c_file, exe_name2).split())
    with open(exe_name1, "rb") as f, open(exe_name2, "rb") as g:
        assert f.read() == g.read()


@mark.skipif('NO_OPTIONAL_TESTS or ONLY_SLOW')
def test_bcd_int(benchmark, compiler):
    exe_name = EXE_TEMPLATE.format("test_bcd_int", compiler)
    test_path = C_FOLDER.joinpath("tests", "test_bcd_int.c")
    check_call(
        templates[compiler].format(test_path, exe_name).split() + ['-DSEED_OVERRIDE={}'.format(randrange(2**32))]
    )
    run_test = partial(check_output, [exe_name], timeout=60)
    buff = benchmark.pedantic(run_test, iterations=1, rounds=1).decode()
    seed, *otherlines = buff.splitlines()
    errors = []
    print(buff)
    print('')
    for line in otherlines:
        expr, answer = line.split('==')
        if eval(answer) != eval(expr):
            errors.append((line, eval(expr)))
    pprint(errors)
    assert not errors


@mark.skipif('NO_OPTIONAL_TESTS or ONLY_SLOW')
def test_is_prime(benchmark, compiler):
    import p0003
    import p0007
    MAX_PRIME = 1_000_000
    exe_name = EXE_TEMPLATE.format("test_is_prime", compiler)
    test_path = C_FOLDER.joinpath("tests", "test_is_prime.c")
    args = templates[compiler].format(test_path, exe_name) + " -DMAX_PRIME={}".format(MAX_PRIME)
    check_call(args.split())
    with TemporaryFile('wb+') as f:
        run_test = partial(check_call, [exe_name], stdout=f)
        benchmark.pedantic(run_test, iterations=1, rounds=1)
        prime_cache = tuple(p0003.primes(MAX_PRIME))
        for line in f.readlines():
            num, prime, composite, idx = (int(x) for x in line.split())
            assert bool(prime) == bool(p0007.is_prime(num))
            assert bool(composite) == (not p0007.is_prime(num))
            assert composite == 0 or composite == next(iter(p0003.prime_factors(num)))
            assert idx == -1 or prime_cache[idx] == num

    # sometimes benchmark disables itself, so check for .stats
    if hasattr(benchmark, 'stats') and benchmark.stats.stats.max > 200 * MAX_PRIME // 1000000:
        fail("Exceeding 200ns average! (time={}s)".format(benchmark.stats.stats.max))


def test_problem(benchmark, key, compiler):
    if (NO_SLOW and key in known_slow) or (ONLY_SLOW and key not in known_slow):
        skip()
    filename = SOURCE_TEMPLATE.format(key)
    exe_name = EXE_TEMPLATE.format(key, compiler)  # need to have both to keep name unique
    check_call(templates[compiler].format(filename, exe_name).split())
    run_test = partial(check_output, [exe_name])

    if key in known_slow:
        answer = benchmark.pedantic(run_test, iterations=1, rounds=1)
    else:
        answer = benchmark(run_test)
    assert answers[key] == int(answer.strip())
    # sometimes benchmark disables itself, so check for .stats
    if hasattr(benchmark, 'stats') and benchmark.stats.stats.median > 60:
        fail_func = xfail if key in known_slow else fail
        stats = benchmark.stats.stats
        fail_func("Exceeding 60s! (Max={:.6}s, Median={:.6}s)".format(stats.max, stats.median))


if valgrind_compilers:
    if which('valgrind'):
        valgrind_test_exe_name = EXE_TEMPLATE.format('valgrind-check-{}'.format(uuid4()), valgrind_compilers[0])
        check_call(
            templates['debug'][valgrind_compilers[0]].format(
                C_FOLDER.joinpath('p0000_template.c'),
                valgrind_test_exe_name
            ).split())
        VALGRIND_ERR_LIST = not run(['valgrind', '-s', valgrind_test_exe_name], cwd=BUILD_FOLDER).returncode

        def test_valgrind(c_file, v_compiler):
            if (NO_SLOW and key in known_slow) or (ONLY_SLOW and key not in known_slow) or NO_OPTIONAL_TESTS:
                skip()
            exe_name = EXE_TEMPLATE.format("valgrind-{}".format(uuid4()), v_compiler)
            check_call(templates['debug'][v_compiler].format(c_file, exe_name).split())
            if VALGRIND_ERR_LIST:
                args = ['valgrind', '--error-exitcode=1', '--leak-check=yes', '-s', exe_name]
            else:
                args = ['valgrind', '--error-exitcode=1', '--leak-check=yes', exe_name]
            check_output(args, cwd=BUILD_FOLDER)

    if which('pahole'):
        def test_pahole(c_file, v_compiler):
            if (NO_SLOW and key in known_slow) or (ONLY_SLOW and key not in known_slow) or NO_OPTIONAL_TESTS:
                skip()
            exe_name = EXE_TEMPLATE.format("pahole-{}".format(uuid4()), v_compiler)
            check_call(templates['debug'][v_compiler].format(c_file, exe_name).split())
            args = ['pahole', exe_name, '-ERS', '-x', '_IO_FILE']
            buff = check_output(args).decode()
            print(buff)
            if 'hole' in buff:
                for line in buff.splitlines():
                    if 'hole' in line and 'BRAIN FART' not in line:
                        raise Exception()
