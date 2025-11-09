# ProjectC-Sorting

## Summary

This project implements various sorting algorithms in C using SDL2.
It shows a graphical representation of a list being sorted in real-time.

This project was created by Tristan Clowez and Baptise Huot de Saint-Albin

## Features

- Various sorting algorithms:
  - Bubble Sort
  - Quick Sort
  - Merge Sort
  - Insertion Sort
  - Bitonic Sort
  - Bogo Sort
  - Selection Sort
  - Radix Sort
- Selecting seed for random number generation
- Show statistics like sort time, swap, memory access...
- Adjustable delay when sorting
- Change number of graphs (from 1 to 8)
- Change color of the graphs
- Change size of the graphs (from 1 to 10000 elements)
- Launch multiple sorts at the same time

## Requirements
- SDL2 library (SDL2, SDL2_image, SDL2_ttf, SDL2_mixer)
- TinyfileDialogs library

## Compilation
To compile the project, use the provided Makefile. Run the following command in the terminal:
```bash
make deps gcc
```

If you want to clean the build files, use:
```bash
make clean-all
```

If you don't have make installed, you can compile the project manually with:
```bash
gcc -Wall -Wextra -Werror -O2 src/*.c libs/tinyfiledialogs/tinyfiledialogs.c -o build/ProjectC-Sorting -Iinclude -Ilibs/tinyfiledialogs -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm
```

## Video Demonstration
You'll find a video demonstration of the project in the `demo` folder.

