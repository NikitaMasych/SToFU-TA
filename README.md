# Test assignment for the cyber-security engineering 
Using STL and WinAPI utilities implement a console program that accepts the full path to the .exe file and the .ico file as input.

# This console program:

* Lists all names of libraries and WinAPI which are in the import table of the .exe file and outputs them to the console;
* Counts the number of WinAPI names that contain the letter 'W' in the .exe import table;
* Replaces the main icon in the .exe file with the one that was passed to the console application as an argument;
* Calculates the informational entropy of the .exe file and outputs it to the console;
* Calculates the informational entropy of the .ico file and outputs it to the console.

** Warning: with some .EXE files need to use x64 architecture program build, with some - x86 due to peculiarities of the .PE build

** Entropy calculation may take some time, because I didn't use external boost mmap for fast reading, regarding the assignment specifications 
