# JI_cond_clk

*Java* Implementation of **conditional_click**

An old project dates back to Feb 2020 when I was in university.  
I wrote a *Win32* program `cond_clk` in *C++* in 2018 to auto-play those annoying online classes ~~don't tell my teachers lol~~ and re-implement in *Java* when I moved to *Ubuntu* platform to automate annoying daily sign-in requested by the university during *COVID19 pandemic*

**conditional_click** basically means *click something if some condition meets*  
take a snapshot of the screen, load an image from file, compare specific region of them, and click/type something if matches. Everything in *Lua*, That's it  
Note that the image comparation is **pixel by pixel** which is **CPU consuming**, I had neither idea nor intention to optimise it at that time, it just solved the problem for me during university.  
You can do literally *anything* from auto logging into accounts to automated software testing, or even aid you playing games ~~with high CPU cost~~ , use your imagination.

**Internal dependency** as follows, compile the corresponding *C++* code to library of your platform
* Ubuntu: libcond_clk_lib.so <= cond_clk_lib_X11.cpp
* Win32: cond_clk_lib.dll <= cond_clk_lib_win32.cpp

**External dependency**
* luaj-jse-3.0.2.jar
* jna.jar

It's been a while and I even didn't know *Makefile* at that time, so I don't know exactly how to build the project. But it shouldn't be hard if you know about the language and have a look at the source code.
