# `rendeer` - Graphics Programming Experiments 🦌

In order to correctly build, run the following commands in the root directory of the project:

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=/usr/bin/clang++
cmake --build build
```

notice that we are using `clang++` as our C++ compiler, if you run with `gcc` you might encounter
problems with the sanitizer flags not being recognized! If your path to `clang++` is different,
simply substitute it into the above commands and you should be good to go!
