# WRO-2025-Future-Engineers-Canada

The following repository contains the code and technical documentation regarding
the robot created by the CarletonU-uOttawa joint team for the 2025 WRO Future
Engineers competition.

## Introduction to the problem

To be completed.

## Our solution

To be completed.

### Chosen computing platform & programming language

To be completed.

### Basic hardwear design

To be completed.

### Compiling and running the robot's firmware (Linux)

To get started, clone the repository and build the firmware using `make`:

```sh
git clone https://github.com/teostefan/WRO-2025-Future-Engineers-Beta.git
cd WRO-2025-Future-Engineers-Beta
make
```

> **Note**: Depending on how your Linux system is configured, an "insufficient
> privileges" error may be thrown on compilation regarding the file
> `kf_matrix_gen.sh`. To solve this, run
> `sudo chmod 777 ./software/kf/kf_matrix_gen.sh` in the repository's root. If
> this error still persists, use `sudo` in front of `make` as your user might
> not have enough privileges to execute the above bash file.

Then, simply run the `main` executable in the generated `builds` folder:

```sh
builds/main
```

### Compiling and running individual tests

To generate the test executables, run:

```sh
make all-tests
```

Afterwards, all tests will be in the `builds` folder and have a name in the
format `test_xx`. Each test can then be individually executed as in the
following example:

```sh
builds/test_cv
```

### Software dependencies

If running on Linux, all of the below dependencies should be pre-installed.

- `ffmpeg` - Abstraction layer for the driver, also used for applying filters to
  the video feed.
- `v4l2` - Video4Linux2 multimedia drivers.
- `ffplay` - Media player GUI for debugging.
- `unistd.h` - The UNIX standard header.
