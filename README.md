<div id="top"></div>

# Simplified DES cipher and S-Box attack

This project is a simplified implementation of the Data Encryption Standard cipher written in C++.  This also includes a input-output differential distribution tables for the two S-Boxes used in the simplified DES.  We can also attack these S-Boxes which gives us the possible values for the key.

## Simplified DES

* Works on 8-bit blocks and uses a 10-bit key
* Only uses two rounds
* We consider that the plaintext and cipher text are known:
  * Plaintext:  *p*<sub>1</sub>, *p*<sub>2</sub>, ... , *p*<sub>8</sub> 
  * Ciphertext: *c*<sub>1</sub>, *c*<sub>2</sub>, ... , *c*<sub>8</sub>
  * Key: *k*<sub>1</sub>, *k*<sub>2</sub>, ... , *k*<sub>10</sub>
<p align="center">
    <img style="border-radius: 20px; width: 49%" src="./img/DES.png">
    <img style="border-radius: 20px; width: 49%" src="./img/f-func.png">
</p>

* The encryption algorithm will produce eight non-linear equations with 10 unknowns

<p align="center">
    <img style="border-radius: 20px; width: 49%" src="./img/encryption-decryption.png">
</p>

<p align="right">(<a href="#top">back to top</a>)</p>

## Differential Cryptanalysis

* Builds input-output distribution tables for the S-Boxes in the simplified DES
* Shows an attack of each S-Box and gives possible keys
  * Choosing a pair of inputs *x*<sub>1</sub> and *x*<sub>2</sub>, it computes &#9651;*X*. Then, it **XOR**s these inputs with the key to produce *x*'<sub>1</sub> and *x*'<sub>2</sub>. Running these values through an S-Box it gets *y*<sub>1</sub> and *y*<sub>2</sub>, which computes &#9651;*Y*.  Using *x*<sub>1</sub>, *x*<sub>2</sub>, &#9651;*X*, and &#9651;*Y*, it determines the possible set of values for the key.  The program repeats this process serveral times until it narrows the possible set of keys to either one or two individual keys.

<p align="center">
    <img style="border-radius: 20px; width: 49%" src="./img/s-box.png">
</p>

<p align="right">(<a href="#top">back to top</a>)</p>

## Getting Started

### Dependencies

* I used WSL2 on Windows 10/11, but it will most likely work on other platforms
* I used g++ 9.3.0 as a compiler

### Executing the program

* Make sure g++ is installed
* Using WSL2, compile `des8bit.cpp` or `sbox-attack.cpp` with g++

### Usage

For `des8bit.cpp`, the program will run two tests first before letting the user input the Plaintext and Key.

* For the plaintext, can be any word, but it must not include any spaces
* For the key, it must be a binary string of length 10

For `sbox-attack.cpp`, the program will give the user some commands to input into the command line.

* d0 : Prints the distribution table for S-Box 0.
* d1 : Prints the distribution table for S-Box 1.
* a0 : Shows S-Box 0 attack and the possible keys.
* a1 : Shows S-Box 1 attack and the possible keys.
* h : Prints the commands.
* q : Closes the program.

<p align="right">(<a href="#top">back to top</a>)</p>

## Contact

Noah Cussatti - noah.cussatti@gmail.com

Project Link: https://github.com/noahcussatti/Simplified-DES-Cipher

<p align="right">(<a href="#top">back to top</a>)</p>
