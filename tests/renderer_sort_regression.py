"""Exercise the actual renderer radix-sort functions without starting a renderer.
Run from a VS developer shell (cl) or a system with cc: python tests/renderer_sort_regression.py
"""
from pathlib import Path
import os
import re
import shutil
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
compiler = shutil.which('cl') or shutil.which('cc')
if not compiler:
    raise SystemExit('Run from a VS developer shell, or install a C compiler.')

with tempfile.TemporaryDirectory(prefix='ioql-sort-') as directory:
    directory = Path(directory)
    for renderer in ('renderergl1', 'renderergl2', 'renderervk'):
        source = (root / 'code' / renderer / 'tr_main.c').read_text()
        begin = source.index('static ID_INLINE void R_Radix(')
        end = source.index('\n}', source.index('static void R_RadixSort(', begin)) + 2
        functions = source[begin:end]
        header = (root / 'code' / renderer / 'tr_local.h').read_text()
        assert re.search(r'uint64_t\s+sort;', header), renderer
        surface = (root / 'code' / renderer / 'tr_surface.c').read_text()
        match = re.search(r'static void DoRailDiscs\([^;]+?\)\s*\{', surface)
        begin = match.start()
        end = surface.index('\n}', match.end()) + 2
        discs = surface[begin:end]
        geometry_types = r"""
#include <math.h>
typedef float vec3_t[3];
#define DEG2RAD(x) ((x)*3.14159265358979323846/180.0)
#define VectorCopy(a,b) do { (b)[0]=(a)[0]; (b)[1]=(a)[1]; (b)[2]=(a)[2]; } while(0)
#define VectorAdd(a,b,c) do { (c)[0]=(a)[0]+(b)[0]; (c)[1]=(a)[1]+(b)[1]; (c)[2]=(a)[2]+(b)[2]; } while(0)
#define RB_CHECKOVERFLOW(v,i)
#define RB_CheckVao(v)
static struct { struct { unsigned char shaderRGBA[4]; } e; } entity;
static struct { __ENTITY_TYPE__ *currentEntity; } backEnd;
static struct {
    int numVertexes, numIndexes;
    float xyz[64][4];
    __TEX_COORDS__
    struct { unsigned char rgba[4]; } vertexColors[64];
    unsigned short color[64][4];
    int indexes[96];
} tess;
"""
        # Name the small mocked entity type without relying on compiler extensions.
        geometry_types = geometry_types.replace('static struct { struct { unsigned char shaderRGBA[4]; } e; } entity;',
            'typedef struct { struct { unsigned char shaderRGBA[4]; } e; } testEntity_t;\nstatic testEntity_t entity;')
        geometry_types = geometry_types.replace('__ENTITY_TYPE__', 'testEntity_t').replace('__TEX_COORDS__',
            'float texCoords[64][2];' if renderer == 'renderergl2' else 'float texCoords[1][64][2];')
        alpha = 'tess.color[i][3] != 73 * 257' if renderer == 'renderergl2' else 'tess.vertexColors[i].rgba[3] != 73'
        geometry_test = r"""
    {
        vec3_t origin = {100, 200, 300}, dir = {1, 0, 0};
        vec3_t right = {0, 1, 0}, up = {0, 0, 1};
        backEnd.currentEntity = &entity;
        entity.e.shaderRGBA[3] = 73;
        DoRailDiscs(1, origin, dir, right, up, 7);
        if (tess.numVertexes != 4 || tess.numIndexes != 6) return 3;
        for (i=0; i<4; i++) {
            if (__ALPHA__) return 4;
            if (fabs(tess.xyz[i][0]-100) > 0.001) return 5;
        }
    }
""".replace('__ALPHA__', alpha)
        program = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define ID_INLINE
#define Q3_LITTLE_ENDIAN
#define MAX_DRAWSURFS 256
/* Payload verifies stability and correct indexing with the widened member. */
typedef struct { uint64_t sort; int payload; void *surface; } drawSurf_t;
''' + functions + geometry_types + discs + r'''
int main(void) {
    drawSurf_t entries[MAX_DRAWSURFS];
    int i;
    for (i=0; i<MAX_DRAWSURFS; i++) {
        entries[i].sort = ((uint64_t)((255-i)%17) << 32) | (uint64_t)((255-i)%7);
        entries[i].payload = i;
    }
    R_RadixSort(entries, 0);
    R_RadixSort(entries, 1);
    R_RadixSort(entries, MAX_DRAWSURFS);
    for (i=1; i<MAX_DRAWSURFS; i++) {
        if (entries[i-1].sort > entries[i].sort) return 1;
        if (entries[i-1].sort == entries[i].sort && entries[i-1].payload > entries[i].payload) return 2;
    }
    return 0;
}
'''
        path = directory / (renderer + '.c')
        exe = directory / (renderer + ('.exe' if os.name == 'nt' else ''))
        program = program.replace('    return 0;\n}', geometry_test + '    return 0;\n}')
        path.write_text(program)
        if Path(compiler).stem.lower() == 'cl':
            args = [compiler, '/nologo', str(path), '/Fe:' + str(exe), '/Fo:' + str(directory / (renderer + '.obj'))]
        else:
            args = [compiler, str(path), '-o', str(exe), '-lm']
        subprocess.run(args, cwd=directory, check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        subprocess.run([str(exe)], check=True)
        print('PASS:', renderer, '64-bit sorting, stable equal keys and rail-ring geometry/alpha')
