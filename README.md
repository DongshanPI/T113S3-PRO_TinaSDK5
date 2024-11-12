# T113S3-PRO_TinaSDK5

## 环境搭建

### 获取补丁包

获取扩展支持仓库，然后加以应用

~~~bash
ubuntu@ubuntu1804:~$ git clone https://github.com/DongshanPI/T113S3-PRO_TinaSDK5.git
ubuntu@ubuntu1804:~$ cd T113S3-PRO_TinaSDK5
ubuntu@ubuntu1804:~/T113S3-PRO_TinaSDK5$ git submodule update --init
ubuntu@ubuntu1804:~/T113S3-PRO_TinaSDK5$ cp ./* -rfvd ~/tina5sdk-bsp
~~~

### 配置单板编译

应用完成，可以进入之前解压缩过的 tina5sdk-bsp 目录内，执行如下命令来开始编译sdk

~~~bash
ubuntu@ubuntu1804:~/tina5sdk-bsp$ source build/envsetup.sh 
ubuntu@ubuntu1804:~/tina5sdk-bsp$ ./build.sh 
All available platform:
   0. android
   1. linux
Choice [android]: 1
All available linux_dev:
   0. bsp
   1. buildroot
   2. openwrt
Choice [bsp]: 1
All available ic:
   0. t113
   1. t113_i
Choice [t113]: 0
All available board:
   0. dev
   1. evb1
   2. evb1_auto
   3. evb1_auto_nor
   4. pro
Choice [dev]: 2
All available flash:
   0. default
   1. nor
Choice [default]: 0
INFO: Prepare toolchain ...
INFO: kernel defconfig: generate /home/ubuntu/tina5sdk-bsp/out/t113/kernel/build/.config by /home/ubuntu/tina5sdk-bsp/device/config/chips/t113/configs/evb1_auto/linux-5.4/config-5.4
INFO: Prepare toolchain ...
make: Entering directory '/home/ubuntu/tina5sdk-bsp/kernel/linux-5.4'
make[1]: Entering directory '/home/ubuntu/tina5sdk-bsp/out/t113/kernel/build'
  GEN     Makefile
*** Default configuration is based on '../../../../../device/config/chips/t113/configs/evb1_auto/linux-5.4/config-5.4'
#
# No change to .config
#
make[1]: Leaving directory '/home/ubuntu/tina5sdk-bsp/out/t113/kernel/build'
make: Leaving directory '/home/ubuntu/tina5sdk-bsp/kernel/linux-5.4'
make: Entering directory '/home/ubuntu/tina5sdk-bsp/buildroot/buildroot-201902'
  GEN     /home/ubuntu/tina5sdk-bsp/out/t113/evb1_auto/buildroot/buildroot/Makefile
Config.in.legacy:1769:warning: choice value used outside its choice group
#
# configuration written to /home/ubuntu/tina5sdk-bsp/out/t113/evb1_auto/buildroot/buildroot/.config
#
make: Leaving directory '/home/ubuntu/tina5sdk-bsp/buildroot/buildroot-201902'
INFO: buildroot defconfig is sun8iw20p1_t113_defconfig 
INFO: clean buildserver
INFO: prepare_buildserver
========ACTION List: build_linuxdev;========
options : 
INFO: ----------------------------------------
INFO: build linuxdev ...
INFO: chip: sun8iw20p1
INFO: platform: linux
INFO: kernel: linux-5.4
INFO: board: evb1_auto
INFO: output: /home/ubuntu/tina5sdk-bsp/out/t113/evb1_auto/buildroot
INFO: ----------------------------------------
INFO: don't build dtbo ...
INFO: build arisc
find: '/home/ubuntu/tina5sdk-bsp/brandy/brandy-2.0/spl': No such file or directory
find: '/home/ubuntu/tina5sdk-bsp/brandy/dramlib': No such file or directory
INFO: build_bootloader: brandy_path=/home/ubuntu/tina5sdk-bsp/brandy/brandy-2.0
INFO: skip build brandy.
INFO: build kernel ...
INFO: prepare_buildserver
INFO: Prepare toolchain ...
Makefile:681: arch//Makefile: No such file or directory
make: *** No rule to make target 'arch//Makefile'.  Stop.
ERROR: build  Failed
INFO: build kernel failed
~~~

出现这个报错，执行以下指令：

~~~bash
ubuntu@ubuntu1804:~/tina5sdk-bsp$ ./build.sh -d
========ACTION List: build_linuxdev;========
options : 
INFO: ----------------------------------------
INFO: build linuxdev ...
INFO: chip: sun8iw20p1
INFO: platform: linux
INFO: kernel: linux-5.4
INFO: board: evb1_auto
INFO: output: /home/ubuntu/tina5sdk-bsp/out/t113/evb1_auto/buildroot
INFO: ----------------------------------------
INFO: don't build dtbo ...
INFO: build arisc
~~~

