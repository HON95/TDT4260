# NTNU TDT4260 Prefetcher Exercise 2020V

## Setup (Ubuntu Disco)

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

## Run

- Build: `make compile`
- Benchmark:
    - Run `make test` (takes a while).
    - See `stats.txt` for aggregated statistics.

## Link the Modified Source

1. Clone the repo or something.
1. Backup the original source: `mv src old_src`
1. Link the new source: `ln -s <repo>/src/ src/`
