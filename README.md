# gappleto97's Project Euler solutions

Windows testing[¹](#footnotes): [![Windows build status](https://ci.appveyor.com/api/projects/status/lqi44hqr7jgqjwpy/branch/dev?svg=true)](https://ci.appveyor.com/project/gappleto97/euler) Linux testing[²](#footnotes): [![Linux build status](https://api.shippable.com/projects/5d7ab0a9686e0a0007cb5970/badge?branch=dev)](https://app.shippable.com/github/gappleto97/Euler/dashboard) OSX testing[³](#footnotes): [![OSX build status](https://travis-ci.org/gappleto97/Euler.svg?branch=dev)](https://travis-ci.org/gappleto97/Euler)

This is the repository I keep for prospective employers to look at project Euler code that I have written.

This is useful because it has defined problems and answers such that you can compare answers with other people.

## This is NOT meant to help others on Project Euler problems

## If you are working on Project Euler problems, DO NOT use this as a source

The repo is divided into sections for each language. The top-level Makefile will direct recipes using prefixes

- make c_____ will go to the c Makefile
- make py______ will go to the python Makefile
- make js______ will go to the javascript Makefile

## Footnotes

1: Windows testing is done on C, Python, and Javascript. C compilers are CL (MSVC 2015/2017 x86/x64) and clang

2: Linux testing is done on Ubuntu 14.04 for C, Python, and Javascript. C compilers are gcc, clang, and icc.

3: OSX testing is done on C, Python, and Javascript. C compilers are gcc and clang
