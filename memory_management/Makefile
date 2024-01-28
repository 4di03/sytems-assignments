all : alloc-mem load-mem

alloc-mem : alloc-mem-palle.c
	gcc alloc-mem-palle.c -o allocmem

load-mem : load-mem-palle.c dynblock.c
	gcc load-mem-palle.c dynblock.c -o loadmem