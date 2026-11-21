// Optional Wolfcam FX aliases. Requires the locally installed dlc_gibs pack.
// Shader definitions only; no proprietary textures or models are distributed here.
q3gibs
{
    {
        map dlc_gibs/gibs.jpg
        rgbGen lightingDiffuse
    }
}

q3bloodMark
{
    nopicmip
    polygonOffset
    {
        clampmap dlc_gibs/blood_stain.png
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen identityLighting
        alphaGen vertex
    }
}

q3bloodTrail
{
    nopicmip
    entityMergable
    {
        clampmap dlc_gibs/blood_spurt.png
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen vertex
        alphaGen vertex
    }
}

railCoreBlack
{
    sort nearest
    cull none
    {
        map gfx/misc/railcoreblack.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen vertex
        tcMod scroll -1 0
    }
}

railDiscBlack
{
    sort nearest
    cull none
    deformVertexes wave 100 sin 0 .5 0 2.4
    {
        clampmap gfx/misc/raildiscblack.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen vertex
        tcMod rotate -30
    }
}
