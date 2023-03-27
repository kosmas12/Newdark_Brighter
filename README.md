# Newdark_Brighter

This is the repository for Newdark_Brighter, an attempt
of reimplementation of the Newdark engine from the TFix 
mod for Thief 1 and 2 using clean reverse engineering
efforts. This is supposed to match the original code
as closely as possible at first, and once there's enough
code reimplemented, it will be rewritten if needed. The
goal of the project is to have a completely open
implementation of Newdark, written in a cross-platform way
to allow porting the engine to other operating systems
and platforms in general.

# Licensing
Newdark_Brighter is licensed under the GNU General Public
License, Version 2. Roughly, this allows you to do anything
you want to the source code, as long as you publish your
changes under GPL2 or any later version. Please see
COPYING for details.

# Goals and current state
By the end of this, Newdark_Brighter aims to be fully
compatible with Newdark and able to use its files.

However, it's also meant to work in a cross-platform
way. For this, cross-platform APIs must be used. The
current idea is:

* SDL2 for input, audio and windowing (migrate to SDL3
when ready) (This is supported by pretty much all platforms
that might need it)
* GL for graphics (Supported by all major PC operating 
systems and has variations available in other platforms)
* Anything else that might pop up later will be decided

Note that this is not final, but is the current initial
plan.

Newdark_Brighter is currently in an extremely early
stage and no actual engine magic is implemented as of yet.

# Reading and understanding the code
In its current state, the code has 2 "microversions",
as I like to call them. These microversions are separated
by preprocessor statements. The code under all of the
`#if defined(__WIN32__)` checks is the original code
that Ghidra's decompiler shows for Newdark using Windows
APIs. The rest is a more cross-platform version to do
the same thing using non-Windows APIs.

Additionally, please note that if a function is defined
as `inline`, it means it was inlined in the original code
and I just made a function for code clarity.

I have also tried to name things in a sane, intuitive way
in the Ghidra project and have carried over the names
to this as much as I can. In case the names or code
change in some way compared to the decompilation, 
it is documented with comments right above.

Code that has not been reimplemented but has been
figured out and is pending to be added in is copied
and pasted straight out of Ghidra and written in TODO 
comments in the places where it's supposed to be.

# Building and running
Few things are needed to build Newdark_Brighter.
Specifically, you need a compiler that supports C++ 20
(any compiler written in the last 3 years will work, or
even further back if your compiler supported the C++ 20
drafts) and the SDL2 libraries. Installing a compiler
and SDL2 is dependent on the platform you're using,
and guides for it can be found on the internet.

Here's how to compile Newdark_Brighter after making
sure you have all the necessary dependencies:

```shell
mkdir build
cmake -B build
cmake --build build
```

To run, enter the `build` directory and launch
`Newdark_Brighter` or `Newdark_Brighter.exe`, depending
on your platform.
