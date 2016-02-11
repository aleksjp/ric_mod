# ric_mod 

Android loadable kernel module that disables Sony RIC write protection on the system partition of Xperia M2 devices.

**Prerequisites:**  
Xperia M2 smartphone.  
Latest firmware(18.6.A.0.182).  
The kernel sources are published on the [SonyXperiaDev](https://github.com/sonyxperiadev/kernel-copyleft) or can be downloaded from the copyleft [archives](http://dl-developer.sonymobile.com/code/copylefts/18.6.A.0.182.tar.bz2).  
Cross compiler toolchain for ARM processors.  

**How to Build:**  
Edit Makefile and change:  
`KERNEL_BUILD :=` _< kernel source directory >_  
`KERNEL_CROSS_COMPILE :=` _< your toolchain >_  
Compile using: ``make`` 

**How to Install and load:**  
`adb push ric_mod.ko /data/local/tmp/ric_mod.ko`  
`$ adb shell`  
`$ su`  
 `# chmod 0644 /data/local/tmp/ric_mod.ko`  
`# insmod /data/local/tmp/ric_mod.ko`  


**How to Remove:**  
`$ adb shell`  
`$ su`   
`# mount -o remount,rw /system`  
`# rmmod ric_mod.ko`  
`# rm -rf /system/lib/modules/ric_mod.ko`

Flashable zip can be download from [here](http://d-h.st/m75F):

Inspired from [wp_mod](https://github.com/flar2/wp_mod).
