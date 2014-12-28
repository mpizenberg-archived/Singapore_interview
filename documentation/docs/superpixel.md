# SEEDS Superpixels algorithm

I will use the SEEDS superpixels algorithm ([http://docs.opencv.org/trunk/modules/ximgproc/doc/superpixels.html](http://docs.opencv.org/trunk/modules/ximgproc/doc/superpixels.html)) to segment the cat from the background.

You can find a lot more information on the algorithm here :
[http://www.mvdblive.org/seeds/](http://www.mvdblive.org/seeds/)

## Ideas to segment the cat

By using SEEDS superpixels the simplest way on the images,
we obtains segmentation like that :

![Screenshot](img/superpixels_basic.png)

It is quite difficult then to identify the superpixels corresponding or not to the cat.
Furthermore we can notice that some superpixels does not correspond well to the cat.

To improve superpixels segmentation
