#include "avrgbi-videogen.h"

unsigned char cmd,x0,y0,x1,y1,c,tmp,*scrnptr;
bool debug=0;

void fill(unsigned char c) {
	for(unsigned short i=0;i<_VBUFSIZE;i++)
		scrnptr[i] = (c&0x0f)|(c<<4);
}

inline void pix(unsigned char c, unsigned char x, unsigned char y) {
	int i=(x%_HRES)/2+(y%_HRES)*_HBYTES;
	scrnptr[i]=(x%2? scrnptr[i]&0x0f|(c<<4) : scrnptr[i]&0xf0|(c&0x0f));
}

void row(unsigned char c, unsigned char line, unsigned char x0, unsigned char x1)
{
	if(line>=_VRES) return; // line out of screen: do nothing
	if(x0>x1) // swap order if needed
	{
		tmp=x0;
		x0=x1;
		x1=tmp;
	}
	if(x0>=_HRES) return; // left end out of screen: do nothing
	if(x1>=_HRES) x1=_HRES-1; // right end out of screen: bring it back in

	unsigned short i=line*_HBYTES+x0/2;
	if(x0%2) // left end on odd pixel
	{
		scrnptr[line*_HBYTES+x0/2]=scrnptr[line*_HBYTES+x0/2]&0x0f|(c<<4);
		i++;
	}
	for(;i<line*_HBYTES+(x1+1)/2;i++)
	{
		if(debug) delay(5);
		scrnptr[i] = (c&0x0f)|(c<<4);
	}
	if(!(x1%2)) // right end on even pixel
		scrnptr[line*_HBYTES+x1/2]=scrnptr[line*_HBYTES+x1/2]&0xf0|(c&0x0f);
}

void col(unsigned char c, unsigned char col, unsigned char y0, unsigned char y1)
{
	if(col>=_HRES) return; // column out of screen: do nothing
	if(y0>y1) // swap order if needed
	{
		tmp=y0;
		y0=y1;
		y1=tmp;
	}
	if(y0>=_VRES) return; // top end out of screen: do nothing
	if(y1>=_VRES) y1=_VRES-1; // bottom end out of screen: bring it back in
	if(col%2) // odd column
		for(unsigned short i=y0*_HBYTES+col/2;i<y1*_HBYTES+col;i+=_HBYTES)
		{
			if(debug) delay(5);
			scrnptr[i] = scrnptr[i]&0x0f|(c<<4);
		}
	else // even column
		for(unsigned short i=y0*_HBYTES+col/2;i<=y1*_HBYTES+col;i+=_HBYTES)
		{
			if(debug) delay(5);
			scrnptr[i] = scrnptr[i]&0xf0|(c&0x0f);
		}
}

void rect(unsigned char c, unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
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
	if(x0==0 && x1==_HRES-1 && y0==0 && y1==_VRES-1) // full screen
	{
		fill(c);
		return;
	}
	if(x0==x1) // no width: column
	{
		col(c,x0,y0,y1);
		return;
	}
	if(y0==y1) // no height: row
	{
		row(c,y0,x0,x1);
		return;
	}
	for(unsigned char i=x0;i<=x1;i++) // many columns
	{
		col(c,i,y0,y1);
		if(debug) delay(32);
	}
}

void bmp(unsigned char x, unsigned char ypos, unsigned char width, unsigned char height) {
	unsigned char y=ypos;
	unsigned char tmp;
	while(y<ypos+height) // each row
	{
		for(unsigned char i=x;i<x+width;i++) // a row
		{
			while(!Serial.available()); // wait for data
			if(y==_VRES||i>=_HBYTES) Serial.read(); // out of screen: discard
			else scrnptr[y*_HBYTES+i]=Serial.read(); // in screen: draw
		}
		y++;
	}
}

void setup()
{
	DDRB=0b00100010; // outputs: led(PB5), sync(PD9)
	PORTB|=0b00000001; // mode(PB0) input pullup
	scrnptr=render_setup();
	if (!PINB%2) // mode(PB0) pulled low: test pattern, enable debug
	{
		rect(0,0,0,_HRES/4-1,_VRES/4-1);
		rect(1,_HRES/4,0,_HRES/2-1,_VRES/4-1);
		rect(2,_HRES/2,0,_HRES*0.75-1,_VRES/4-1);
		rect(3,_HRES*0.75,0,_HRES-1,_VRES/4-1);
		rect(4,0,_VRES/4,_HRES/4-1,_VRES/2-1);
		rect(5,_HRES/4,_VRES/4,_HRES/2-1,_VRES/2-1);
		rect(6,_HRES/2,_VRES/4,_HRES*0.75-1,_VRES/2-1);
		rect(7,_HRES*0.75,_VRES/4,67,_VRES/2-1);
		rect(8,0,_VRES/2,_HRES/4-1,_VRES*0.75-1);
		rect(9,_HRES/4,_VRES/2,_HRES/2-1,_VRES*0.75-1);
		rect(10,_HRES/2,_VRES/2,_HRES*0.75-1,_VRES*0.75-1);
		rect(11,_HRES*0.75,_VRES/2,67,_VRES*0.75-1);
		rect(12,0,_VRES*0.75,_HRES/4-1,_VRES-1);
		rect(13,_HRES/4,_VRES*0.75,_HRES/2-1,_VRES-1);
		rect(14,_HRES/2,_VRES*0.75,_HRES*0.75-1,_VRES-1);
		rect(15,_HRES*0.75,_VRES*0.75,_HRES-1,_VRES-1);
		debug=1;
	}
	PORTB&=0b11011110; // mode(PB0) pullup and led(PB5) off (to be eco friendly)
	Serial.begin(115200);
}

void loop()
{
	while(!Serial.available()); // wait for command
	cmd=Serial.read();
	if(debug) cmd%=6; // debug mode: modulo to make sure to react to any number
	switch(cmd)
	{
		case 0: // fill
		while(!Serial.available());
		c=Serial.read();
		fill(c);
		if(debug) Serial.write(c);
		break;

		case 1: // pixel
		while(Serial.available()<3);
		c=Serial.read();
		x0=Serial.read();
		y0=Serial.read();
		pix(c,x0,y0);
		if(debug)
		{
			Serial.write(c);
			Serial.write(x0);
			Serial.write(y0);
		}
		break;

		case 2: // row
		while(Serial.available()<4);
		c=Serial.read();
		y0=Serial.read();
		x0=Serial.read();
		x1=Serial.read();
		row(c,y0,x0,x1);
		if(debug)
		{
			Serial.write(c);
			Serial.write(y0);
			Serial.write(x0);
			Serial.write(x1);
		}
		break;

		case 3: // column
		while(Serial.available()<4);
		c=Serial.read();
		x0=Serial.read();
		y0=Serial.read();
		y1=Serial.read();
		col(c,x0,y0,y1);
		if(debug)
		{
			Serial.write(c);
			Serial.write(x0);
			Serial.write(y0);
			Serial.write(y1);
		}
		break;

		case 4: // rectangle
		while(Serial.available()<5);
		c=Serial.read();
		x0=Serial.read();
		y0=Serial.read();
		x1=Serial.read();
		y1=Serial.read();
		rect(c,x0,y0,x1,y1);
		if(debug)
		{
			Serial.write(c);
			Serial.write(x0);
			Serial.write(y0);
			Serial.write(x1);
			Serial.write(y1);
		}
		break;

		case 5: // bitmap
		while(Serial.available()<4);
		x0=Serial.read();
		y0=Serial.read();
		x1=Serial.read();
		y1=Serial.read();
		bmp(x0,y0,x1,y1);
		if(debug)
		{
			Serial.write(x0);
			Serial.write(y0);
			Serial.write(x1);
			Serial.write(y1);
		}
		break;
	}
}
