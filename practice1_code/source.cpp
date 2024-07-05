#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <cstdio>
#include <cstdlib>
//
const int NumPoints = 5000;
//typedef vec2 point2; //define a point2 type identical to vec2

void init(void)
{
    vec2 points[NumPoints];

    for (int i = 1; i < NumPoints; i++) {
        points[i].x = (rand() % 200) / 100.0f - 1.0f;
        points[i].y = (rand() % 200) / 100.0f - 1.0f;
    }

    // create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // load shaders
    GLuint program = InitShader("vshader_ex1.glsl", "fshader_ex1.glsl");
    glUseProgram(program);

    // initialize vertex position attribute from vertex shader
    GLuint loc = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, NumPoints);
    glFlush();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(512, 512);

    //glutInitContextVersion(4, 3);//앞으로 포함시키지 말 것
    //In case of MacOS
    //glutInitContextProfie(GLUT_CORE_PROFILE);//앞으로 포함시키지 말 것
    glutCreateWindow("Many Points GPU");

    glewInit();
    init();
    printf("OpenGL %s, GLSL %s\n",
        glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION));
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}