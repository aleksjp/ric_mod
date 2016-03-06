# ric_mod 

Android loadable kernel module that disables Sony RIC write protection on the system partition of Xperia M2.

**Prerequisites:**  
Xperia smartphone.  
The kernel sources are published on the [SonyXperiaDev](https://github.com/sonyxperiadev/kernel-copyleft) or can be downloaded from the copyleft [archives](http://dl-developer.sonymobile.com/code/copylefts/18.6.A.0.182.tar.bz2).  
Cross compiler toolchain for ARM processors.  

**Build:**  
Edit Makefile and change:  
`KERNEL_BUILD :=` _< kernel source directory >_  
`KERNEL_CROSS_COMPILE :=` _< your toolchain >_  
Compile using: ``make`` 

**Install and load:**  
`adb push ric_mod.ko /data/local/tmp/ric_mod.ko`  
`$ adb shell`  
`$ su`  
 `# chmod 0644 /data/local/tmp/ric_mod.ko`  
`# insmod /data/local/tmp/ric_mod.ko`  


**Remove:**  
`$ adb shell`  
`$ su`   
`# mount -o remount,rw /system`  
`# rmmod ric_mod.ko`  
`# rm -rf /system/lib/modules/ric_mod.ko`
- - - -

In order to use with older firmware versions the module symbol crc can be patched via copymodulecrc.  
When you'd like to build it, run the `ndk-build` command from your project root directory, or modify the provided _Makefile.android_.

Flashable zip and batch file can be found in the *utils* folder.

References:   
[wp_mod](https://github.com/flar2/wp_mod)  
[modulecrcpatch](https://github.com/dosomder/modulecrcpatch)
