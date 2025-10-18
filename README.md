**ZSM Assembler**

**About:** 
The ZSM Assmbler assembles binaries for the [ZVM Project](https://github.com/flagzerointeractive/ZeroVirtualMachine).  Information about ZCode and the Virtual Hardware Can be found in [this repository](https://github.com/flagzerointeractive/ZVM-SPEC). 

**Usage** 

    ZSM [YourZSMFile].zsm -o [YourBinaryName].zx
   
   
   **Assembler flags (so far)**

    
|-d|  Generates Debug Symbols at the end of the File|
|--|--|


**Build Info**

This Project does not have any dependancies. 
Alls you need is a C++ Compiler. 

***Mac OS - Terminal Build***

    make macos
    sudo make install
 
***GNU Linux - Terminal Build***

    make linux
    sudo make install

***Windows -  ([w64devkit](https://github.com/skeeto/w64devkit))***

    make windows 
 
