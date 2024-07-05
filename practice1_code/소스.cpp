#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <cstdio>
#include <cstdlib>
//

const int NumVertices = 36; // 6 faces * 2 tri/face * 3 vtx/tri

vec4 points[NumVertices];
vec4 colors[NumVertices];

int Index = 0;

// 8 corner points of a unit cube centered at the origin
vec4 vertices[8] = {
    vec4(-0.5, -0.5,  0.5, 1.0),
    vec4(-0.5,  0.5,  0.5, 1.0),
    vec4(0.5,  0.5,  0.5, 1.0),
    vec4(0.5, -0.5,  0.5, 1.0),
    vec4(-0.5, -0.5, -0.5, 1.0),
    vec4(-0.5,  0.5, -0.5, 1.0),
    vec4(0.5,  0.5, -0.5, 1.0),
    vec4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
vec4 vertex_colors[8] = {
    vec4(0.0, 0.0, 0.0, 1.0),  // black
    vec4(1.0, 0.0, 0.0, 1.0),  // red
    vec4(1.0, 1.0, 0.0, 1.0),  // yellow
    vec4(0.0, 1.0, 0.0, 1.0),  // green
    vec4(0.0, 0.0, 1.0, 1.0),  // blue
    vec4(1.0, 0.0, 1.0, 1.0),  // magenta
    vec4(1.0, 1.0, 1.0, 1.0),  // white
    vec4(0.0, 1.0, 1.0, 1.0)   // cyan
};

//make two triangls that divide a quad
void quad(int a, int b, int c, int d)
{
    //one triangle a-b-c
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;

    //one triangle a-c-d
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

// generate a cube
void colorcube()
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}
void init(void)
{
    colorcube();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL,
        GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

    //load shaders
    GLuint program = InitShader("vshader_colorcube1.glsl", "fshader_colorcube1.glsl");
    glUseProgram(program);

    //initialize vertex position attribute from vertex shader
    GLuint vPos = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPos);
    glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    //initialize vertex color attribute from vertex shader
    GLuint vCol = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vCol);
    glVertexAttribPointer(vCol, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));


    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    glutSwapBuffers();
    //glFlush();
}

void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 033:
        exit(EXIT_SUCCESS);
        break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);

    //glutInitContextVersion(4, 3);
    //glutInitContextProfie(GLUT_CORE_PROFILE);
    glutCreateWindow("Many Tetrahedra GPU");

    glewInit();
    init();
    /*
    printf("OpenGL %s, GLSL %s\n",
        glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION));
    */
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}