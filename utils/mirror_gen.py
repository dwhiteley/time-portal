from random import uniform
from struct import pack
from math import floor

def coord(u_max,v_max,frames):
    sub = v_max / frames;
    sub_inc = 1.0/sub
    z_inc = (frames + 1.0)/v_max
    if u_max%sub > 0:
        raise AssertionError("Number of frames do no divide evently in both u and v directions")
    for v in range(v_max):
        y = (v%sub)*sub_inc
        z = v*z_inc
        z -= floor(z)
        for u in range(u_max):
            yield ((u%sub)*sub_inc, y, z)

def min_abs(s_n,p_n):
    d = abs(s_n - p_n)
    return (1.0 - d) if (d > 0.5) else d

def max_abs(s_n, p_n):
    d = abs(s_n - p_n)
    return d if (d > 0.5) else (1.0 - d)

def l_dist(f_a,n):
    return lambda s,p: sum(f_a(s_n,p_n)**n for (s_n,p_n) in zip(s,p))**(1.0/n)

def dists(f_d,S,p):
    return sorted(f_d(s,p) for s in S)

def val(S,p):
    return 1.0/sum(1.0/x for x in dists(l_dist(min_abs,2),S,p)[0:3])

def gen_rand_set(n,dim):
    return {tuple(uniform(0,1) for i in range(dim)) for j in range(n)}

def def_array_idx(u_max, v_max):
    '''Return a function which gets the array index from picture coordinates.
    Can use negative indexes'''
    return lambda u,v: (v%v_max)*u_max+(u%u_max)

def scale(D,low,high):
    '''Linearly remaps values in array to new [low,high) domain'''
    data = list(D)
    d_low, d_high = min(data), max(data)
    d_r = d_high - d_low
    r = high - low
    return ((d-d_low)*r/d_r+low for d in data)

def greyTo16(D):
    '''Converts floating point grayscale to 16-bit raw color data'''
    scaled = scale(D,0,63)
    return ((p//2)*2048+p*32+(p//2) for p in (round(i) for i in scaled))

def normalize(D):
    '''Linearly remaps values in array to [0,1) domain'''
    data = list(D)
    d_low, d_high = min(data), max(data)
    d_r = d_high - d_low
    return ((d-d_low)/d_r for d in data)

def make_grad_func(low,high):
    '''Creates a function which scales a normalized(i.e. [0,1)) value'''
    return lambda p: tuple(p*(h-l)+l for h,l in zip(high,low))

def floatTo16RawGrey(pic):
    grayScale = make_grad_func((0,0,0),(31,63,31))
    rgb = (tuple(floor(x) for x in grayScale(pt)) for pt in normalize(pic))
    return ((p[0]<<12)+(p[1]<<5)+p[2] for p in rgb)

def floatToBMPGrey(pic):
    grayScale = make_grad_func((0,0,0),(255,255,255))
    return (tuple(floor(x) for x in p) for p in grayScale(normalize(pic)))

def gen_pic(points,frames):
    f_d = l_dist(min_abs,2)
    S = gen_rand_set(points,3)
    return (1.0 - dists(f_d, S, p)[0] for p in coord(240,320,frames))

def write_raw(pic):
    buf = list(greyTo16(pic))
    f = open('bubble.raw','wb')
    f.write(pack('<%dH'%len(buf),*buf))
    f.close()

def sub_pic(p, idx, size):
	limit = size//2
	sub = []
	for v in range(idx[1]-limit, idx[1]+limit+1):
		for u in range(idx[0]-limit, idx[0]+limit+1):
			sub.append(p[pic_idx(u,v)])
	return sub

def median(l):
    return sorted(l)[len(l)//2]

def median_filter(p):
    m_p = []
    for v in range(320):
	    for u in range(240):
		    m_p.append(median(sub_pic(p,(u,v),5)))
    return m_p

blue_low = (0,0,154)
blue_high = (204,204,255)

#Note: bubble.raw = median3.bmp
