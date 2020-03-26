# NTNU TDT4260 Prefetcher Exercise 2020V

An implementation of the Delta Correlattion Prediction Table (DCPT) cache prefetcher algorithm, using a modified M5 simulator and the CPU SPEC2000 benchmark suite.

## Implementation Notes

- Since this does not need to be performance-optimized or anything, we chose to use fragmented C++-style dynamic data structures of pointers instead of C-style arrays as circular buffers of data, for simplicity of implenentation's sake.

## Setup

### Installation (Ubuntu Disco)

1. Download the modified M5 simulator and SPEC CPU200 benchmarks provided by the course admins (not redistributable).
1. Install available dependencies: `apt install python-dev scons swig zlib1g-dev m4`
1. Install g++-4.8:
    1. Temporarily enable the Bionic APT repo by adding `deb <url> bionic main universe` to `/etc/apt/sources.list`,
       where `<url>` is the same as for the other `main` line.
    1. Install: `sudo apt update && sudo apt install gcc-4.8 g++-4.8`
    1. Remove the temporary APT source list.
1. Unpack the downloaded archive to `/opt/`: `sudo tar xvf prefetcher.tgz -C /opt/`
1. Enter the parent of the directory you wish to setup the simulator and source code in.
1. Setup the simulator and tools: `/opt/prefetcher/bin/setup_prefetcher ./prefetcher`
1. Try compiling and running it to see if it's set up correctly (see below).

### Link the Modified Source

1. Clone the repo or something.
1. Backup the original source: `mv src old_src`
1. Link the new source: `ln -s <repo>/src/ src/`

## Run

- Build: `make compile`
- Benchmark:
    - Run `make test` (takes a while).
    - See `stats.txt` for aggregated statistics.
