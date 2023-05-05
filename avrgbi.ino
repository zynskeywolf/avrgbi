#include "avrgbi-videogen.h"

bool debug=0;
unsigned char *scrnptr,cmd,x0,y0,x1,y1,c,chi,cmode,tmp;
unsigned short dtmp;

inline void fill(unsigned char c, unsigned char cmode, unsigned char y0, unsigned char y1) {
	c|=(c<<4);
	unsigned short ptr=y0*_HBYTES; // start for
	dtmp=(y1+1)*_HBYTES; // end for

	switch(cmode)
	{
	case 0: // replace
		for(; ptr<dtmp; ptr++)
			scrnptr[ptr] = c;
		break;
	case 1: // add
		for(; ptr<dtmp; ptr++)
			scrnptr[ptr] |= c;
		break;
	case 2: // subtract
		for(; ptr<dtmp; ptr++)
			scrnptr[ptr] &= c;
		break;
	case 3: // invert
		for(; ptr<dtmp; ptr++)
			scrnptr[ptr] ^= c;
		break;
	}
}

inline void pix(unsigned char c, unsigned char x, unsigned char y) {
	short i=(x%_HRES)/2+(y%_VRES)*_HBYTES;
	switch(c>>4)
	{
	case 0:
		scrnptr[i]=(x%2? (scrnptr[i]&0x0f)|(c<<4) : (scrnptr[i]&0xf0)|(c&0x0f));
		break;
	case 1:
		scrnptr[i]|=(x%2? c<<4 : c&0x0f);
		break;
	case 2:
		scrnptr[i]&=(x%2? (c<<4)|0x0f : c|0xf0);
		break;
	case 3:
		scrnptr[i]^=(x%2? c<<4 : c&0x0f);
		break;
	}
}

void row(unsigned char c, unsigned char cmode, unsigned char line, unsigned char x0, unsigned char x1)
{
	short i=line*_HBYTES+x0/2;

	// left end on odd pixel
	if(x0%2)
	{
		dtmp=line*_HBYTES+x0/2;
		switch(cmode)
		{
		case 0:
			scrnptr[dtmp]=(scrnptr[dtmp]&0x0f)|(c<<4);
			break;
		case 1:
			scrnptr[dtmp]|=c<<4;
			break;
		case 2:
			scrnptr[dtmp]&=(c<<4)|0x0f;
			break;
		case 3:
			scrnptr[dtmp]^=c<<4;
			break;
		}
		i++;
	}

	// right end on even pixel
	if(!(x1%2))
	{
		dtmp=line*_HBYTES+x1/2;
		switch(cmode)
		{
		case 0:
			scrnptr[dtmp]=(scrnptr[dtmp]&0xf0)|c;
			break;
		case 1:
			scrnptr[dtmp]|=c;
			break;
		case 2:
			scrnptr[dtmp]&=c|0xf0;
			break;
		case 3:
			scrnptr[dtmp]^=c;
			break;
		}
	}

	// in between
	dtmp=line*_HBYTES+(x1+1)/2;
	c|=c<<4;
	switch(cmode)
	{
	case 0: // replace
		for(; i<dtmp; i++)
			scrnptr[i] = c;
		break;
	case 1: // add
		for(; i<dtmp; i++)
			scrnptr[i] |= c;
		break;
	case 2: // subtract
		for(; i<dtmp; i++)
			scrnptr[i] &= c;
		break;
	case 3: // invert
		for(; i<dtmp; i++)
			scrnptr[i] ^= c;
		break;
	}
}

void col_even(unsigned char c, unsigned char cmode, unsigned char col, unsigned char y0, unsigned char y1)
{
	unsigned short i=y0*_HBYTES+col; // top
	dtmp=y1*_HBYTES+col; // bottom
	switch(cmode)
	{
	case 0:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]=(scrnptr[i]&0xf0)|c;
		break;
	case 1:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]|=c;
		break;
	case 2:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]&=c|0xf0;
		break;
	case 3:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]^=c;
		break;
	}
}

void col_odd(unsigned char c, unsigned char cmode, unsigned char col, unsigned char y0, unsigned char y1)
{
	unsigned short i=y0*_HBYTES+col; // top
	dtmp=y1*_HBYTES+col; // bottom
	c=c<<4;
	switch(cmode)
	{
	case 0:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]=(scrnptr[i]&0x0f)|c;
		break;
	case 1:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]|=c;
		break;
	case 2:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]&=c|0x0f;
		break;
	case 3:
		for(; i<=dtmp; i+=_HBYTES)
			scrnptr[i]^=c;
		break;
	}
}

void rect(unsigned char c, unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
	cmode=c>>4;
	c&=0x0f;

	if(x0>x1) // swap order if needed
	{
		tmp=x0;
		x0=x1;
		x1=tmp;
	}
	if(y0>y1)
	{
		tmp=y0;
		y0=y1;
		y1=tmp;
	}
	if(x0>=_HRES||y0>=_VRES) return; // top left corner out of screen: do nothing
	if(x1>=_HRES) x1=_HRES-1; // right edge out of screen: bring it back in
	if(y1>=_VRES) y1=_VRES-1; // bottom edge out of screen: bring it back in

	if(x0==0 && x1==_HRES-1) // full width
	{
		fill(c,cmode,y0,y1);
		return;
	}
	if(x0==x1) // no width: column
	{
		if(x0%2)
			col_odd(c,cmode,x0/2,y0,y1);
		else
			col_even(c,cmode,x0/2,y0,y1);
		return;
	}
	if(y0==y1) // no height: row
	{
		row(c,cmode,y0,x0,x1);
		return;
	}

	// many columns
	tmp=x1/2;
	for(unsigned char i=(x0+1)/2; i<=tmp; i++)
		col_even(c,cmode,i,y0,y1);
	tmp=(x1+1)/2;
	for(unsigned char i=x0/2; i<tmp; i++)
		col_odd(c,cmode,i,y0,y1);
}

void bmp(unsigned char x, unsigned char ypos, unsigned char width, unsigned char height) {
	unsigned char line=ypos, endwhile=ypos+height;
	dtmp=(unsigned short)x+width;
	while(line<endwhile) // each row
	{
		for(unsigned short i=x; i<dtmp; i++) // a row
		{
			while(!Serial.available()); // wait for data
			if(line==_VRES||i>=_HBYTES) Serial.read(); // out of screen: discard
			else scrnptr[line*_HBYTES+i]=Serial.read(); // in screen: draw
		}
		line++;
	}
}

void setup()
{
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0); // cpu clock

	VPORTB.DIR |= 1;
	VPORTB.OUT=255;
	PORTB.PIN0CTRL=128;
	PORTB.PIN1CTRL=8;
	VPORTD.DIR |= 0b00001111;
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTB_gc;

	scrnptr=render_setup();

	if (!(PORTB.IN&2)) // mode(PB1) pulled low: test pattern, enable debug
	{
		rect(0,0,0,_HRES/4-1,_VRES/4-1);
		rect(1,_HRES/4,0,_HRES/2-1,_VRES/4-1);
		rect(2,_HRES/2,0,_HRES*0.75-1,_VRES/4-1);
		rect(3,_HRES*0.75,0,_HRES-1,_VRES/4-1);
		rect(4,0,_VRES/4,_HRES/4-1,_VRES/2-1);
		rect(5,_HRES/4,_VRES/4,_HRES/2-1,_VRES/2-1);
		rect(6,_HRES/2,_VRES/4,_HRES*0.75-1,_VRES/2-1);
		rect(7,_HRES*0.75,_VRES/4,_HRES-1,_VRES/2-1);
		rect(8,0,_VRES/2,_HRES/4-1,_VRES*0.75-1);
		rect(9,_HRES/4,_VRES/2,_HRES/2-1,_VRES*0.75-1);
		rect(10,_HRES/2,_VRES/2,_HRES*0.75-1,_VRES*0.75-1);
		rect(11,_HRES*0.75,_VRES/2,_HRES-1,_VRES*0.75-1);
		rect(12,0,_VRES*0.75,_HRES/4-1,_VRES-1);
		rect(13,_HRES/4,_VRES*0.75,_HRES/2-1,_VRES-1);
		rect(14,_HRES/2,_VRES*0.75,_HRES*0.75-1,_VRES-1);
		rect(15,_HRES*0.75,_VRES*0.75,_HRES-1,_VRES-1);
		debug=1;
	}
	PORTB.PIN1CTRL=0; // mode(PB0) pullup off (to be eco friendly)
	Serial.begin(115200);
}

void loop()
{
	while(!Serial.available()); // wait for command

	cmd=Serial.read();

	if(debug)
		cmd%=3; // modulo to make sure to react to any number

	switch(cmd)
	{
	case 0: // pixel
		while(Serial.available()<3);
		c=Serial.read();
		x0=Serial.read();
		y0=Serial.read();
		pix(c,x0,y0);
		if(debug)
		{
			Serial.write(cmd);
			Serial.write(c);
			Serial.write(x0);
			Serial.write(y0);
		}
		break;

	case 1: // rectangle
		while(Serial.available()<5);
		c=Serial.read();
		x0=Serial.read();
		y0=Serial.read();
		x1=Serial.read();
		y1=Serial.read();
		rect(c,x0,y0,x1,y1);
		if(debug)
		{
			Serial.write(cmd);
			Serial.write(c);
			Serial.write(x0);
			Serial.write(y0);
			Serial.write(x1);
			Serial.write(y1);
		}
		break;

	case 2: // bitmap
		while(Serial.available()<4);
		x0=Serial.read();
		y0=Serial.read();
		x1=Serial.read();
		y1=Serial.read();
		bmp(x0,y0,x1,y1);
		if(debug)
		{
			Serial.write(cmd);
			Serial.write(x0);
			Serial.write(y0);
			Serial.write(x1);
			Serial.write(y1);
		}
		break;
	}
}
