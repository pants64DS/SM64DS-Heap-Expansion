# SM64DS Heap Expansion

In Super Mario 64 DS, the Wi-Fi driver code takes a significant amount of memory.
This patch turns that into allocatable heap memory.
(Only compatible with the European version of the game.)

#### Installation for an existing project that uses NSMBe patches

1. Copy [multiheap.cpp](source/multiheap.cpp) to your source folder

2. If you're using [More Objects Mod](https://github.com/Gota7/MoreObjectsMod) (and it hasn't been ported to DLs yet), check the starting address of overlay 155 using the overlay editor in SM64DSe. If it's something other than 023c4000, change it to that and recompile the overlay.

3. Open the ROM in [NSMBe](https://github.com/pants64DS/NSMB-Editor) and click "Run 'make' and insert"
	* If you get a compile or linker error,
	you will probably have to update the headers and symbols you use.
	In this case, it's recommended to add the
	[SM64DS Programming Interface](https://github.com/pants64DS/SM64DS-PI)
	to your project as a Git submodule.
	Manually update the rest of your source files to make them
	compatible with the newest version if necessary.
	The [Makefile](Makefile) in this repository shows how to get
	the code to compile with the headers and symbols from the submodule.

#### Installation for a ROM that doesn't have any inserted code yet[^1]

1. Clone or download this repository

2. Put the ROM at its root (the same folder as this file)

3. Make it compatible with NSMBe patches
	* In SM64DSe, there is a button that says "Toggle Suitability for NSMBe ASM Patching"
	* If you don't want to patch your ROM with SM64DSe, you can also decompress the ARM9 binary using NSMBe
4. Open the ROM in [NSMBe](https://github.com/pants64DS/NSMB-Editor) and click "Run 'make' and insert"
	* If that doesn't work, make sure you have [devkitPro](https://devkitpro.org) properly installed

[^1]: If the ROM uses MOM, it definitely has inserted code, even if you didn't insert it.
