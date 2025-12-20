# Hungarian Lottery

This repository contains a C++ implementation for the coding test. It includes:

- Reader for players file (`PlayersNumbersReader`).
- Console input reader (`ConsoleReader`).
- Console output writer (`ConsoleWriter`).
- Abstract `LotteryProcessor` and two implementations: brute-force and cached.
- Unit tests using Google Test.

Build (Linux, with dependencies):

1. Install dependencies (example Ubuntu):
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libgtest-dev libglog-dev doxygen
# build gtest if needed
```

2. Build:
```bash
mkdir -p build && cd build
cmake ..
make -j
```

3. Run tests (if built):
```bash
ctest --output-on-failure
```

Run application:
```bash
./build/src/hungarian_lottery_app tests/data/sample_players.txt
# then type the 5 numbers for the lottery pick into stdin, e.g.:
# 1 2 3 4 5
```

Doxygen:
Run `doxygen` from repo root if `Doxyfile` is available, or install Doxygen and run a minimal config.

Notes:
- Code is instrumented to use `glog` if available. Debug messages use `LOG(DEBUG)` (or `VLOG` when glog is present); control verbosity with glog flags.
- All parse/validation errors log `FATAL` and throw `std::runtime_error` so the app can return non-zero.
# hungarian_lottery