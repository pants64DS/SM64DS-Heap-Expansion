# SM64DS Heap Expansion

In Super Mario 64 DS, there is an unused memory region of 236 kilobytes.\* This patch turns that into allocatable heap memory. Only compatible with the European version of the game.

#### Installation for an existing project that uses NSMBe patches

1. Copy [multiheap.cpp](source/multiheap.cpp) to your source folder

2. If you're using [More Objects Mod](https://github.com/Gota7/MoreObjectsMod) (and it hasn't been ported to DLs yet), set the variable called `momOverlayExists` in multiheap.cpp to `true`

3. Open the ROM in [NSMBe](https://github.com/pants64DS/NSMB-Editor) and click "Run 'make' and insert"
   
   * If you get a compile error, you might have to copy [Memory.h](source/include/Memory.h) or other headers from [source/include](source/include) to your source/include folder
   
   * If you get a linker error, you might have to copy any missing symbols from [symbols.x](symbols.x) in this repo to your symbols.x

#### Installation for a ROM that doesn't have any inserted code yet\*\*

1. Clone or download this repository

2. Put the ROM at its root (the same folder as this file)

3. Make it compatible with NSMBe patches
   
   * In SM64DSe, there is a button that says "Toggle Suitability for NSMBe ASM Patching"
   
   * If you don't want to patch your ROM with SM64DSe, you can also decompress the ARM9 binary using NSMBe

4. Open the ROM in [NSMBe](https://github.com/pants64DS/NSMB-Editor) and click "Run 'make' and insert"
   
   * If that doesn't work, make sure you have [devkitPro](https://devkitpro.org) properly installed

<sub>\*It might technically be 239 kilobytes, but I heard some flashcards might use three of them.</sub>

<sub>\*\*If the ROM uses MOM, it definitely has inserted code, even if you didn't insert it.</sub>
