# Project_TIED
Text in Image Encryption and Decryption
## What does it do?
The program can encrypt text from a **.txt** file into a **.bmp** image. Or decrypt a **.bmp** image and store the message into a **.txt** file.
## How does it work?
The program, while encrypting, reads the **.txt** and **.bmp** file and checks if the text can fit into the image and that the image is not already encrypted by this program. If it fits and it does not contain any previous encryption, it applies a Caesar cipher on the text and writes it to a **.bmp** image. It will also add a tag in the header so the program knows what **.bmp** images it has encrypted itself, this is to prevent re-encrypting an image and trying to decrypt incompatible images.

While decrypting it will read a **.bmp** images and check if the header has the correct tag (ENCRYPTED). Once it has confirmed the tag to be there it will read the file. After this it will check if the Start Of Data (SOD) tag is at the first position, if it is it will read the data until the End Of Data (EOD) tag or if not found until the end of the file, but this will lead to the decryption failing. Once all data is collected it will decrypt the Caesar cipher and send the text to a **.txt** file.
## How to use?
The program has the following commands:
- //help :: commands overview
- /e [IMAGE] :: encrypt :: REQUIRES: /t, /o
- /d [IMAGE] :: decrypt :: REQUIRES: /o
- /t [TEST FILE] :: text file
- /o [OUTPUT FILE] :: output file, either .txt OR .bmp **--IT WILL CLEAR ANY DATA IF THE FILE EXISTS--**
- /k [KEY] :: encryption key, a key between 0 and 123 for the basic Caesar Cipher, an offset for the characters in ASCII format (0 - 123)

These command work with both the '-' or '/' prefix.
## How to compile?
### Using gcc:
gcc -Wall -pedantic -c lilog.c -o lilog.o<br/>
gcc -Wall -pedantic -c lilib.c -o lilib.o<br/>
gcc -Wall -pedantic -c TIEDlib.c -o TIEDlib.o<br/>
gcc -Wall -pedantic TIED.c liLog.o lilib.o TIEDlib.o -o TIED.exe<br/>
### Using CLion
Load the project.<br/>
Add to the CMake file:<br/>
add_executable(Project_TIED<br/>
TIEDlib.c<br/>
TIEDlib.h<br/>
lilib.c<br/>
lilib.h<br/>
lilog.c<br/>
lilog.h<br/>
TIED.c)<br/>

---
## Sources.
Binary string to number: [Stack Overflow](https://stackoverflow.com/a/15114188/4620857)<br/>
BMP files: [Wikipedia](https://en.wikipedia.org/wiki/BMP_file_format#:~:text=The%20BMP%20file%20format%2C%20also,and%20OS%2F2%20operating%20systems), [THE BMP FILE FORMAT](http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm)<br/>
Variable argument lists: [C Programming](https://www.cprogramming.com/tutorial/c/lesson17.html)<br/>
Github shields: [Shields io](https://shields.io/)<br/>
Caesar cipher: [Wikipedia](https://en.wikipedia.org/wiki/Caesar_cipher)<br/>
Markdown Cheatsheet: [Github](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet)<br/>
Substring: [Stack Overflow](https://stackoverflow.com/a/4214350/4620857)<br/>
Counting file characters: [Geeks for Geeks](https://www.geeksforgeeks.org/c-program-to-count-the-number-of-characters-in-a-file/)<br/>
Other: [Mental support](https://www.youtube.com/watch?v=dQw4w9WgXcQ)<br/>

---
![N|Solid](https://img.shields.io/github/last-commit/LennyIndustries/Project_TIED) ![N|Solid](https://img.shields.io/github/repo-size/LennyIndustries/Project_TIED) ![N|Solid](https://img.shields.io/tokei/lines/github/LennyIndustries/Project_TIED) ![N|Solid](https://img.shields.io/github/issues/LennyIndustries/Project_TIED) ![N|Solid](https://img.shields.io/github/issues-pr/LennyIndustries/Project_TIED)
