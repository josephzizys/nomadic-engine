
static const float cube_vertices[8 * 3] =
{
    -1,  1,  1,
    1,  1,  1,
    1, -1,  1,
    -1, -1,  1,
    
    -1,  1, -1,
    1,  1, -1,
    1, -1, -1,
    -1, -1, -1
};

static const float cube_normals[6 * 3] =
{
    0,  0,  1,
    0,  1,  0,
    1,  0,  0,
    0,  0, -1,
    0, -1,  0,
   -1,  0,  0
};

static const float cube_colours[8 * 4] =
{
    1,  0,  0,1,
    0  ,1,  0,1,
    0  ,  0,1,1,
    1,1,  0,1,
    1,  0,1,1,
    0  ,1,1,1,

    0  ,  0,1,1,
    0  ,1,  0,1
};

static const float cube_tex[8 * 3] =
{
    1,0,0,
    0,1,0,
    0,0,0,
    1,1,0,
    1,0,0,
    0,1,0,
    0,0,0,
    0,1,0
};

static const u8 cube_triangles[12 * 3] =
{
    /* front */
    1,0,3,
    1,3,2,
    
    /* right */
    2,6,5,
    2,5,1,

    /* back */
    7,4,5,
    7,5,6,

    /* left */
    0,4,7,
    0,7,3,

    /* top */
    5,4,0,
    5,0,1,

    /* bottom */
    3,7,6,
    3,6,2
};

static const u8 cube_normal_indices[12 * 3] =
{
    /* front */
    0,0,0,
    0,0,0,
    
    /* right */
    2,2,2,
    2,2,2,

    /* back */
    3,3,3,
    3,3,3,

    /* left */
    5,5,5,
    5,5,5,

    /* top */
    1,1,1,
    1,1,1,

    /* bottom */
    4,4,4,
    4,4,4
};
