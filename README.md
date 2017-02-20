#Bootloader and command line tool for STM32 Discovery Board

Bootloader is written in C and a small function in ARM assembly.
[Based on spec here](http://www.st.com/content/ccc/resource/technical/document/application_note/51/5f/03/1e/bd/9b/45/be/CD00264342.pdf/files/CD00264342.pdf/jcr:content/translations/en.CD00264342.pdf)

Command line tool has 3 different versions.
* Python version, used as a prototype. Written in about a day, so code is ugly.
* C version, written based on the Python prototype. Somewhat cleaner, but parsing args in C is still ugly.
* Better Python version using getopt. Unfinished, since the C version worked well enough at this point.


