#version 460 core
// ------------------------------------------------------------------
// CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------

#define PI 3.1415926535

// enum of field type
const int U    = 0;
const int V    = 1;
const int CELL = 2;

// ------------------------------------------------------------------
// INPUTS -----------------------------------------------------------
// ------------------------------------------------------------------

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

uniform uint  u_height;
uniform uint  u_width;
uniform float u_dx;
uniform float u_dt;
uniform float u_over_relaxation;

// ------------------------------------------------------------------
// BUFFERS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(std430, binding = 0) buffer vel_u_t
{
    float vel_u[];
}
vel_u;

layout(std430, binding = 1) buffer vel_v_t
{
    float vel_v[];
}
vel_v;

layout(std430, binding = 2) buffer vel_u_post_t
{
    float vel_u_post[];
}
vel_u_post;

layout(std430, binding = 3) buffer vel_v_post_t
{
    float vel_v_post[];
}
vel_v_post;

layout(std430, binding = 4) buffer boundary_t
{
    int boundary[];
}
boundary;

layout(std430, binding = 5) buffer mass_t
{
    float mass[];
}
mass;

layout(std430, binding = 6) buffer mass_post_t
{
    float mass_post[];
}
mass_post;

// ------------------------------------------------------------------
// Images ---------------------------------------------------------
// ------------------------------------------------------------------

// layout(binding = 0, rgba32f) uniform image2D image0;

// ------------------------------------------------------------------
// functions ---------------------------------------------------------
// ------------------------------------------------------------------

// average v field at i-1,i, j, j+1
float v_average_at(uint i, uint j)
{
    uint height = u_height;

    float bottom = vel_v.vel_v[i * height + j];
    float top    = vel_v.vel_v[i * height + j + 1];

    float left  = vel_v.vel_v[(i - 1) * height + j];
    float right = vel_v.vel_v[i * height + j];

    return (bottom + top + left + right) * 0.25f;
}

// average u field at i, i+1, j-1, j;   i+1 <= width-1
float u_average_at(uint i, uint j)
{
    uint height = u_height;

    float bottom = vel_u.vel_u[i * height + j];
    float top    = vel_u.vel_u[(i + 1) * height + j];

    float left  = vel_u.vel_u[i * height + j - 1];
    float right = vel_u.vel_u[i * height + j];

    return (bottom + top + left + right) * 0.25f;
}

// bilinear interpolation
float sample_field(int type, float _x, float _y)
{
    uint  width   = u_width;
    uint  height  = u_height;
    float dx      = u_dx;
    float dx_inv  = 1.0f / dx;
    float dx_half = 0.5f * dx;

    // cap to lower boundary, to first cell
    float x = clamp(_x, dx, float(width) * dx);
    float y = clamp(_y, dx, float(height) * dx);

    // staggered grid , for u move down half dx, so on
    float x_offset = 0.0f;
    float y_offset = 0.0f;
    if (type == U)
    {
        y_offset = dx_half;
    }
    else if (type == V)
    {
        x_offset = dx_half;
    }
    else if (type == CELL)
    {
        x_offset = dx_half;
        y_offset = dx_half;
    }

    // find cell index
    int x0 = int(floor((x - x_offset) * dx_inv));
    int x1 = x0 + 1;

    int y0 = int(floor((y - y_offset) * dx_inv));
    int y1 = y0 + 1;

    // cap to upper boundary
    x0 = min(int(floor((x - x_offset) * dx_inv)), int(width) - 1);
    x1 = min(x0 + 1, int(width) - 1);
    y0 = min(int(floor((y - y_offset) * dx_inv)), int(height) - 1);
    y1 = min(y0 + 1, int(height) - 1);

    // weights ;  physical distance  / cell size;
    float sx = (x - x_offset - x0 * dx) * dx_inv;
    float sy = (y - y_offset - y0 * dx) * dx_inv;
    float tx = 1.0f - sx;
    float ty = 1.0f - sy;

    float result = 0.0f;

    if (type == U)
    {
        result += tx * ty * vel_u.vel_u[x0 * height + y0];
        result += sx * ty * vel_u.vel_u[x1 * height + y0];
        result += tx * sy * vel_u.vel_u[x0 * height + y1];
        result += sx * sy * vel_u.vel_u[x1 * height + y1];
    }
    else if (type == V)
    {
        result += tx * ty * vel_v.vel_v[x0 * height + y0];
        result += sx * ty * vel_v.vel_v[x1 * height + y0];
        result += tx * sy * vel_v.vel_v[x0 * height + y1];
        result += sx * sy * vel_v.vel_v[x1 * height + y1];
    }
    else if (type == CELL)
    {
        result += tx * ty * mass.mass[x0 * height + y0];
        result += sx * ty * mass.mass[x1 * height + y0];
        result += tx * sy * mass.mass[x0 * height + y1];
        result += sx * sy * mass.mass[x1 * height + y1];
    }

    return result;
}

// ------------------------------------------------------------------
// entry ---------------------------------------------------------
// ------------------------------------------------------------------

// advect velocity field
void main()
{
    // common
    uint i = gl_GlobalInvocationID.x;
    uint j = gl_GlobalInvocationID.y;

    uint height = u_height;
    uint width  = u_width;

    if (i > width - 1 || j > height - 1)
    {
        return;
    }

    uint center = i * height + j;
    uint right  = (i + 1) * height + j;
    uint left   = (i - 1) * height + j;
    uint top    = i * height + j + 1;
    uint bottom = i * height + j - 1;

    float dx_half = 0.5f * u_dx;

    // extrapolate
    // boundary edges
    if (j == 0)
    {
        vel_u.vel_u[i * height + j] = vel_u.vel_u[i * height + j + 1];
    }
    else if (j == height - 1)
    {
        vel_u.vel_u[i * height + j] = vel_u.vel_u[i * height + j - 1];
    }

    if (i == 0)
    {
        vel_v.vel_v[i * height + j] = vel_v.vel_v[(i + 1) * height + j];
    }
    else if (i == width - 1)
    {
        vel_v.vel_v[i * height + j] = vel_v.vel_v[(i - 1) * height + j];
    }

    // advect

    // copy first, keeps boundary conditions;
    vel_u_post.vel_u_post[center] = vel_u.vel_u[center];
    vel_v_post.vel_v_post[center] = vel_v.vel_v[center];
    mass_post.mass_post[center]   = mass.mass[center];

    if (i == 0 || j == 0)
    {
        return;
    }

    // u component on edge
    // if not next to boundary
    // j+1 <= height-1
    if (boundary.boundary[center] != 0 && boundary.boundary[left] != 0
        && j < height - 1)
    {
        // recover velocity
        float u_vel = vel_u.vel_u[center];
        float v_vel = v_average_at(i, j);

        // simple euler step
        float x = i * u_dx - u_vel * u_dt;
        float y = j * u_dx - v_vel * u_dt + dx_half; // 2 be 2.5;

        // sample velocity by bilinear interpolation
        vel_u_post.vel_u_post[center] = sample_field(U, x, y);
    }

    // v component on edge, same as u
    // i+1 <= width-1
    if (boundary.boundary[center] != 0 && boundary.boundary[bottom] != 0
        && i < width - 1)
    {
        float u_vel = u_average_at(i, j);
        float v_vel = vel_v.vel_v[center];

        float x = i * u_dx - u_vel * u_dt + dx_half;
        float y = j * u_dx - v_vel * u_dt;

        vel_v_post.vel_v_post[center] = sample_field(V, x, y);
    }

    // move other attributes to other shader
}
