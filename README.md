# SFBTXT: Graphics from text for custom SSDs

## Introduction

SFBTXT is a command line utility for generating PNG files from text.  The available fonts are designed to generate text that looks like SSD text from the ***Star Fleet Battles*** game (SFB is owned by Amarillo Design Bureau; no affiliation).

Running the program is trivial:

```
% sfbtxt foo.png
```

And then type in the text, line by line, as you want it to appear and terminate input with ***CTRL-D***.  Alternatively, you can create a (unix-formatted) text file to be used as input:

```
% echo HELLO WORLD > t.txt
% sfbtxt -i t.txt foo.png
```

## Compilation

At this time, SFBTXT is only supported on Linux.  You will need libpng installed in order to build.

```
% make
```

This will generate the ***sfbtxt*** utility in the current directory, which you can copy into your PATH.

## Bugs/Questions/Contributions

You can contact the maintainer, William M. Leiserson, at willtor@mit.edu with bugs or questions.

We appreciate contributions of bug fixes, features, etc.  If you would like to contribute, please read the GPL-v2 license (LICENSE) carefully to be sure you agree to the terms under which this library is released.  If you make a change, add your name to the AUTHORS file.
