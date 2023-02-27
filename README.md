### Double Sided QR Generator

This is a naive implementation of Altsoph's [DOUBLE-SIDED QR-CODE](https://medium.com/altsoph/double-sided-qr-code-c946468f05d4O).

For example, the QR code below links to google.com, but when flopped it links to bing.

Original QR Code | Flopped QR Code
--- | ---
![](./example/search.jpg "Original QR code") | ![](./example/search-mirror.jpg "Flopped QR code")

Still a lot of optimization (on code size and on program speed) can be done: we may try non-symmetrical masks, or solve the binary equations in a better way, or try masks 'not best but still good enough'.

But I'd guess this serves as an interesting example.

(In fact, when I started this project, I tried a different way: use a big qr code and try to find error correction groups that do not intersect, but the data capacity was really low.)

See source code for a quick example. (Well, it is messy. To try out, you just need to modify the last lines of main.cpp)
