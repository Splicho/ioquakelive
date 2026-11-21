// Minimal ioquakelive FX script.
// Load with:  set cg_fxfile "scripts/example.fx"; fxload
// Run with:   runfx test/spark

test/spark {
    shader flareShader
    size 10
    color 1 1 1 1
    emitter 1 {
        sprite
    }
}
