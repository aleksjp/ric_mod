#APP_BUILD_SCRIPT := Android.mk                # By default, the NDK build system looks under jni/ for a file named Android.mk.
APP_ABI := armeabi armeabi-v7a                # This setting tells the NDK to build two versions of your machine code (e.g. all)
#APP_OPTIM := release                          # Define this optional variable as either release or debug
APP_PIE = true                                # From Android 5.0 (API level 21), executables require PIE. To use PIE set the -fPIE flag.
APP_PLATFORM := android-21                    # Name of the target Android platform
#NDK_TOOLCHAIN_VERSION := 4.8                  # Select a version of the GCC compiler. Version 4.9 is the default for 64-bit ABIs, and 4.8 is the default for 32-bit ABIs.
#NDK_PROJECT_PATH  :=.                         # This variable stores the absolute path to your app's project-root directory
