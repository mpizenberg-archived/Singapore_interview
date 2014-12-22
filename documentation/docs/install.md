# Description of the installation of Eclipse and OpenCV

Here I describe the Linux installation process for Eclipse and OpenCV

## Installation of Eclipse on linux

1. First you need to download the [Eclipse IDE for C/C++ Developpers](http://www.eclipse.org/downloads/).
2. Extract it were you want to install it.
3. Add that folder to your path.

Eclipse is now ready.


## Installation of OpenCV

If we want to be able to use the latest functionalities of OpenCV we need to build it from sources.
There is the [installation procedure](http://docs.opencv.org/trunk/doc/tutorials/introduction/linux_install/linux_install.html).

First we clone it :
```
git clone https://github.com/Itseez/opencv.git
git clone https://github.com/Itseez/opencv_contrib.git
```

Then configure and build it :
```
cd opencv/
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
make -j4
sudo make install
```

> Remark : if we need to use some advanced function (for exemple SEEDS superpixels algorithm) that are in opencv contrib libraries we need to add `-DOPENCV_EXTRA_MODULES_PATH=<opencv_contrib>/modules` to the cmake options.

Finally we need to assure that the loader will find it (otherwise it will compile but there would be an error at the execution). For that, we just need to put the folder where the libs are installed to the file `/etc/ld.so.conf.d/opencv.conf`. For me it is :
```
/usr/local/lib
```
Then we update it with :
```
sudo ldconfig /etc/ld.so.conf
```
