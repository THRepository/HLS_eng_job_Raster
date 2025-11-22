# HLS Engineering job Raster

This project implementing a component with help of intel hls tools to rasterize for graphics to VGA-component.

Whit 3 components **VGA_raster**, **hierarchi_part** and **write_pixle**. Implementation of a hierarchical raster for some increase in speed.

A VGA-component with support to be able to communicate with raster components are not implemented in this example as on-chip memory used as an image buffer.This is improved and written directly to an image RAM directly in another project. 

![Picture of filled screen of triangle drawn by component.](IMG_0266.png)

Example nios system for MAX10 fpga demonstrate raster component. Image shows the resulting image on the VGA screen.

Demonstration video link: [link](https://youtu.be/ynYZmR4_Qbk)
