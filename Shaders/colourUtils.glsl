
vec4 colourUnpackUnsigned(uint c)
{
	return vec4((c >> 24 & 0xFF) / 255.0,
				(c >> 16 & 0xFF) / 255.0,
				(c >>  8 & 0xFF) / 255.0,
				(c >>  0 & 0xFF) / 255.0);
}

uint colourPackUnsigned(vec4 c)
{
	return (int(c.r * 255.0) & 0xFF << 24) +
		   (int(c.g * 255.0) & 0xFF << 16) +
		   (int(c.b * 255.0) & 0xFF <<  8) +
		   (int(c.a * 255.0) & 0xFF <<  0);
}