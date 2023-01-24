# Cerveurus
This is my personal fork from [cerveur](https://github.com/infraredCoding/cerveur), a simple web framework written in C for fun and learning.

## Why a seperate project?
I feel like the original project is pretty inactive and there are pull requests from 1+ months that are still open and there's a lot I want to contribute to this project so I decided to fork it and develop my own separate chain.

## Why call it cerveurus?
The original name `cerveur` reminded me of `cerberus` so I'm naming it `cerveurus`.

## How to Build from Source
The original project used plain Makefiles but I'm a fan of CMake, so I will use that as the build system for this fork.

Enter the root directory and use:
* `mkdir build && cd build`
* `cmake .. && make`

Which should build the `libcerveurus.a` static library that can be linked to your server.

## Running Tests
To run the tests, go to the `build` folder and use  `make test CTEST_OUTPUT_ON_FAILURE=TRUE`.

## How to Use

### Development
After running `make` inside your build folder, a `main` executable will be generated inside `build/src` and copy over the `templates/` and `static/` folders. Use `./src/main` (assuming you're inside the `build` folder) to run the server. By default it's on port `6969` then go to `localhost:6969` in your browser. This is the development/testing server that you can play around with.

### Building a webserver
Will write more detail once library is more mature.
