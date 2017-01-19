
database-final
==============

This is the repository for final project of database 2016 fall.


## Report HackMD
[Report](https://hackmd.io/BwdgjAbGAMAmEFoCcAzAhogLE4mECN8BTAYwNgGYBWaNK2EoopIA?both)

## Project Structure

> * `src` All source files
>
> * `test` Test files where each test group are located in the sub-folder
>
> * `build` The output files
>
> * `report`The report document and its assets

## Usage

#### Build

Make sure you have `cmake` installed, change to `/build` and type in `cmake ..` to generate Makefile.

Build files by typing in `make`.

Run tests by typing in `make test`. Alternatively, you can find each test executable in `/build/test/${test name}` folders.

**NOTICE** If you change the CMakeLists.txt, make sure to remove the entire `/build` folder and run `cmake ..` again.

#### Development

The easiest way to write a new test is copying the initial test folder and change cmake configuration file in it.

