lightningBolt5Faded
{
	nopicmip
	novlcollapse
	cull disable
	{
		map gfx/misc/lightning5.tga
		blendfunc GL_SRC_ALPHA GL_ONE
		rgbgen wave sin 1 0.1 0 3
		alphagen entity
	}
}
