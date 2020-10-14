#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#define WIDTH 100
#define HEIGHT 50

#define SHADE " .*#@"
#define STEPSIZE 100

typedef struct Vec3 {
    double x, y, z;
} Vec3;

void print_display(char disp[HEIGHT][WIDTH])
{
    printf("\033[H\033[J");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++)
            printf("%c", SHADE[disp[y][x]]);

        printf("\n");
    }
}

void set_point(Vec3 *point, char disp[HEIGHT][WIDTH])
{
    int ny = (int)((HEIGHT / 2) * (-point->y + 1));
    int nx = (int)((WIDTH / 2) * (point->x + 1));
    int nz = (int)((strlen(SHADE) / 2) * (point->z + 1));
    nz = (nz < 1) ? 1 : nz;

    if ((ny < HEIGHT) && (nx < WIDTH) && (nz > disp[ny][nx]))
        disp[ny][nx] = nz;
}

Vec3* point_mul(Vec3 *point, Vec3 mat[3])
{
    Vec3 *res = malloc(sizeof(Vec3));

    res->x = point->x * mat[0].x + point->y * mat[0].y + point->z * mat[0].z;
    res->y = point->x * mat[1].x + point->y * mat[1].y + point->z * mat[1].z;
    res->z = point->x * mat[2].x + point->y * mat[2].y + point->z * mat[2].z;

    return res;
}

void draw_line(Vec3 *point1, Vec3 *point2, char disp[HEIGHT][WIDTH])
{
    Vec3 spoint = *point1;
    Vec3 dpoint = { (point2->x - point1->x) / STEPSIZE, (point2->y - point1->y) / STEPSIZE, (point2->z - point1->z) / STEPSIZE };

    for (int i = 0; i < STEPSIZE; i++) {
        spoint.x += dpoint.x;
        spoint.y += dpoint.y;
        spoint.z += dpoint.z;

        set_point(&spoint, disp);
    }
}

void draw_plane(Vec3 *point1, Vec3 *point2, Vec3 *point3, Vec3 *point4, char disp[HEIGHT][WIDTH])
{
    Vec3 spoint1 = *point1;
    Vec3 spoint2 = *point3;

    Vec3 dpoint1 = { (point2->x - point1->x) / STEPSIZE, (point2->y - point1->y) / STEPSIZE, (point2->z - point1->z) / STEPSIZE };
    Vec3 dpoint2 = { (point4->x - point3->x) / STEPSIZE, (point4->y - point3->y) / STEPSIZE, (point4->z - point3->z) / STEPSIZE };

    for (int i = 0; i < STEPSIZE; i++) {
        spoint1.x += dpoint1.x;
        spoint1.y += dpoint1.y;
        spoint1.z += dpoint1.z;

        spoint2.x += dpoint2.x;
        spoint2.y += dpoint2.y;
        spoint2.z += dpoint2.z;

        draw_line(&spoint1, &spoint2, disp);
    }
}

int main()
{
    char disp[HEIGHT][WIDTH] = { 0 };

    Vec3 b_points[8] = {
        { 0.4, 0.4, -0.4 },
        { -0.4, 0.4, -0.4 },
        { -0.4, -0.4, -0.4 },
        { 0.4, -0.4, -0.4 },
        { 0.4, 0.4, 0.4 },
        { -0.4, 0.4, 0.4 },
        { -0.4, -0.4, 0.4 },
        { 0.4, -0.4, 0.4 },
    };

    double angle = 0;

    while (1) {
        memset(&disp, 0, HEIGHT * WIDTH);

        Vec3 rotx[3] = {
            { 1, 0, 0 },
            { 0, cos(angle), -sin(angle) },
            { 0, sin(angle), cos(angle) },
        };

        Vec3 roty[3] = {
            { cos(angle), 0, sin(angle) },
            { 0, 1, 0 },
            { -sin(angle), 0, cos(angle) },
        };

        Vec3 rotz[3] = {
            { cos(angle), -sin(angle), 0 },
            { sin(angle), cos(angle), 0 },
            { 0, 0, 1 },
        };

        Vec3 *rot_points[8];

        for (int i = 0; i < 8; i++) {
            Vec3 *point  = point_mul(&b_points[i], rotx);
            Vec3 *point2 = point_mul(point, roty);
            Vec3 *point3 = point_mul(point2, rotz);

            rot_points[i] = point3;

            free(point);
            free(point2);
        }

        // These are the edges

        // draw_line(rot_points[0], rot_points[1], disp);
        // draw_line(rot_points[1], rot_points[2], disp);
        // draw_line(rot_points[2], rot_points[3], disp);
        // draw_line(rot_points[3], rot_points[0], disp);

        // draw_line(rot_points[4], rot_points[5], disp);
        // draw_line(rot_points[5], rot_points[6], disp);
        // draw_line(rot_points[6], rot_points[7], disp);
        // draw_line(rot_points[7], rot_points[4], disp);

        // draw_line(rot_points[0], rot_points[4], disp);
        // draw_line(rot_points[1], rot_points[5], disp);
        // draw_line(rot_points[2], rot_points[6], disp);
        // draw_line(rot_points[3], rot_points[7], disp);

        // There are the faces

        draw_plane(rot_points[0], rot_points[3], rot_points[1], rot_points[2], disp);
        draw_plane(rot_points[1], rot_points[2], rot_points[5], rot_points[6], disp); // Delete for cilender
        draw_plane(rot_points[4], rot_points[7], rot_points[5], rot_points[6], disp);
        draw_plane(rot_points[0], rot_points[1], rot_points[4], rot_points[5], disp);
        draw_plane(rot_points[3], rot_points[2], rot_points[7], rot_points[6], disp);
        draw_plane(rot_points[0], rot_points[3], rot_points[4], rot_points[7], disp); // Delete for cilender

        for (int i = 0; i < 8; i++) free(rot_points[i]);


        print_display(disp);

        angle += 0.1;

        usleep(100000);
    }

    return 0;
}
