#include <stdio.h>
#include <assert.h>

char* encode 
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


char* enc(int* reg)
{
    static char buf[4];
    int i0 = (reg[0] & 0xfc) >> 2;
    int i1 = ((reg[0] & 0x3) << 4) | ((reg[1] & 0xf0) >> 4);
    int i2 = ((reg[1] & 0xf) << 2) | ((reg[2] & 0xc0) >> 6);
    int i3 = ((reg[2] & 0x3f));
    assert(i0 >= 0 && i0 < 64);
    assert(i1 >= 0 && i1 < 64);
    assert(i2 >= 0 && i2 < 64);
    assert(i3 >= 0 && i3 < 64);

    buf[0] = encode[i0];
    buf[1] = encode[i1];
    buf[2] = encode[i2];
    buf[3] = encode[i3];
    return buf;
}



int main()
{
    int i = 0;
    int reg[3];
    char* p;
    int width = 0;

    while((reg[i] = getchar()) != EOF)
    {
/*	putchar(reg[i]); */
	++i;
	i = i % 3;
	if(i == 0)
	{
	    p = enc(reg);
	    for(i = 0 ; i < 4; ++i) 
	    {
		putchar(p[i]);
	    }
	    width += 4;
	    if(width >= 72)
	    {
		putchar('\n');
		width = 0;
	    }
	    i = 0;
	}
    }

    if(i == 1)
    {
	reg[1] = 0;
	reg[2] = 0;
	p = enc(reg);
	putchar(p[0]);
	putchar(p[1]);
	putchar('=');
	putchar('=');
	width += 4;
    }
    if(i == 2)
    {
	reg[2] = 0;
	p = enc(reg);
	putchar(p[0]);
	putchar(p[1]);
	putchar(p[2]);
	putchar('=');
	width += 4;
    }
    if(width != 0)
    {
	putchar('\n');
    }

    return 0;
}
