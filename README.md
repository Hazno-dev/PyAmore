# PyAmore

PyAmore is a PyArmo(u)r runtime deobfuscator designed to extract/generate PYC files from encrypted python artifacts.

## Supported Features

PyAmore has been tested with PyArmour **V8** and Python `3.7` but PyArmour has so many settings that I can't guarantee it works for every setup.

- Automatic stripping of PyArmour from bytecode 
- Recursive deobfuscation of code objects
- Generating .pyc files into subfolder for use in a decompiler such as [Uncompyle6](https://github.com/rocky/python-uncompyle6), [Decompyle3](https://github.com/rocky/python-decompile3), [PycDC](https://github.com/zrax/pycdc), or [PyLingual](https://github.com/syssec-utd/pylingual)
- Deobfuscate methods **without** execution

With (varying) support of the following PyArmour features:

- Wrapped Mode
```
LOAD_GLOBALS    N (__armor_enter__)     N = length of co_consts
CALL_FUNCTION   0
POP_TOP
SETUP_FINALLY   X (jump to wrap footer) X = size of original byte code

...
... Here it's obfuscated bytecode of original function
...

LOAD_GLOBALS    N + 1 (__armor_exit__)
CALL_FUNCTION   0
POP_TOP
END_FINALLY
```
- Unwrapped Mode
```
0   JUMP_ABSOLUTE            n = 3 + len(bytecode)

3    ...
     ... Here it's obfuscated bytecode of original function
     ...

n   LOAD_GLOBAL              ? (__armor__)
n+3 CALL_FUNCTION            0
n+6 POP_TOP
n+7 JUMP_ABSOLUTE            0
```
- Obfuscation mode "1"
```
__pyarmor__(__name__, __file__, b'\x02\x0a...', 1)
```


## Usage

### Linux:

Preload the object and execute your script (i.e. `LD_PRELOAD=/home/../PyAmore.so sh init.sh`

### Windows:

TBD

## Building

PyAmore has been built and tested on `clang version 19.1.7` target `x86_64-pc-linux-gnu` with `Python 3.7` and cmake `4.3.1`.

*Note:* PyAmore uses C++20 modules. A modern, up-to-date compiler will be required.

### Commands:
- `cmake .`
- `cmake build .`

Use the build artifact `lib/PyAmore` for injection.

### Changing the Python version

Edit CMakeLists.txt with the target version.
You may be required to build python from source so the project can link against Python as a shared object, though this frankly may not be required, I didn't test it, so YMMV. 

```
find_package(Python 3.7 # YOUR PYTHON VERSION HERE!
        EXACT
        COMPONENTS
        Interpreter
        Development.Embed
)
```

## What Does PyAmore Do?

PyAmour ships with a library `pyTransform.so/.dll/etc` which implements the core functionality behind PyArmour.

PyAmore shims some of these exported functions using dlsym (to avoid a presumed digest check?) or runtime hooking.
It abuses the fact PyArmour will simply extract the target code from `PyEval_GetFrame` to force it to decrypt all codeobjects found in a given module, and then copies it all to a new hierarchy for extraction to a `.pyc` of the same name.

## Sources

- [8dcc/LibDetour](https://github.com/8dcc/libdetour)
- [Leonard Rapp, Melissa Eckardt @ cyber.wtf/unpacking-pyarmor](https://cyber.wtf/2025/02/12/unpacking-pyarmor-v8-scripts/)
- [pyarmor.readthedocs.io](https://pyarmor.readthedocs.io/en/stable/topic/obfuscated-script.html)
