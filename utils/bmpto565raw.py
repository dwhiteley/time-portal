from PIL import Image
import sys, struct
from glob import glob

VERIFY = False

def convert(in_name, out_name):
    im = Image.open(in_name)

    rs = im.getdata(0)
    gs = im.getdata(1)
    bs = im.getdata(2)
    
    rgb565 = [(r>>3)*2048 + (g>>2)*32 + (b>>3) for (r,g,b) in zip(rs, gs, bs)]
    
    f = open(out_name, "wb")
    #Big Endian
    # f.write(struct.pack(">%dH" % len(rgb565), *rgb565))
    #Little Endian
    f.write(struct.pack("<%dH" % len(rgb565), *rgb565))
    f.close()

for i in glob('../img/originals/*.bmp'):
    #in_name = "img/originals/%06da.bmp"%i
    #out_name = "img/raw/%06db.raw"%i
    o = i.replace('originals','raw').replace('a.bmp','b.raw')
    convert(i, o)

# verify
if VERIFY:
    imv = Image.new("RGB",(im.width, im.height))

    x = 0
    y = 0
    for v in rgb565:
        rv = (v / 2048) * 8
        gv = ((v % 2048) / 32) * 4
        bv = (v % 32) * 8
        imv.putpixel((x, y), (rv, gv, bv))

        x += 1
        if x == im.width:
            x = 0
            y += 1

    imv.show()
