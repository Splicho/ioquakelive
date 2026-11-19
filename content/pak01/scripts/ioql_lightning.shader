lightningBolt1Faded
{
	nopicmip
	novlcollapse
	cull none
	{
		map gfx/misc/lightning3new.tga
		blendfunc GL_SRC_ALPHA GL_ONE
		rgbgen wave sin 1 0.5 0 7.1
		alphagen entity
		tcmod scale 2 1
		tcMod scroll -5 0
	}
	{
		map gfx/misc/lightning3new.tga
		blendfunc GL_SRC_ALPHA GL_ONE
		rgbgen wave sin 1 0.8 0 8.1
		alphagen entity
		tcmod scale -1.3 -1
		tcMod scroll -7.2 0
	}
}

lightningBolt2Faded
{
	nopicmip
	novlcollapse
	cull none
	{
		map gfx/misc/lightning2.jpg
		blendfunc GL_SRC_ALPHA GL_ONE
		alphagen entity
		tcMod scale 1.5 1
		tcMod scroll -1.8 0
	}
	{
		map gfx/misc/lightning2.jpg
		blendfunc GL_SRC_ALPHA GL_ONE
		alphagen entity
		tcMod scale -1.5 -1
		tcMod scroll -4 0
	}
}

lightningBolt3Faded
{
	nopicmip
	novlcollapse
	cull none
	{
		map gfx/misc/lightning3.tga
		blendfunc GL_SRC_ALPHA GL_ONE
		rgbgen wave sin 1 0.5 0 7.1
		alphagen entity
		tcmod scale 2 1
		tcMod scroll -5 0
	}
	{
		map gfx/misc/lightning3.tga
		blendfunc GL_SRC_ALPHA GL_ONE
		rgbgen wave sin 1 0.8 0 8.1
		alphagen entity
		tcmod scale -1.3 -1
		tcMod scroll -7.2 0
	}
}

lightningBolt4Faded
{
	nopicmip
	novlcollapse
	cull disable
	{
		map gfx/misc/lightning4.tga
		blendfunc GL_SRC_ALPHA GL_ONE
		alphagen entity
		tcMod scale 4 1
		tcMod scroll -4 0
	}
}

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
