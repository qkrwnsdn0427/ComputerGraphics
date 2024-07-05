#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <cstdio>
#include <cstdlib>
//

const int NumTimesToSubdivide = 4;
const int NumTetrahedra = 256; //4^4
const int NumTriangles = 4 * NumTetrahedra;
const int NumVertices = 3 * NumTriangles;

vec3 points[NumVertices];
vec3 colors[NumVertices];

int Index = 0;

// Create a triangle from three vertices 
void triangle(const vec3 &a, const vec3 &b, const vec3 &c, const int color)
{
    static vec3  base_colors[] = {
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        vec3(0.0, 0.0, 0.0)
    };

    points[Index] = a;  colors[Index] = base_colors[color];  Index++;
    points[Index] = b;  colors[Index] = base_colors[color];  Index++;
    points[Index] = c;  colors[Index] = base_colors[color];  Index++;
}

// Create a tetrahedron from 4 triangles
void tetra( const vec3& a, const vec3& b, const vec3& c, const vec3& d )
{
    triangle( a, b, c, 0 );
    triangle( a, c, d, 1 );
    triangle( a, d, b, 2 );
    triangle( b, d, c, 3 );
}

// Divide a tetrahedron recursively
void divide_tetra( const vec3& a, const vec3& b,
	      const vec3& c, const vec3& d, int count )
{
    if ( count > 0 ) {
        vec3 v0 = ( a + b ) / 2.0;
        vec3 v1 = ( a + c ) / 2.0;
        vec3 v2 = ( a + d ) / 2.0;
	vec3 v3 = ( b + c ) / 2.0;
	vec3 v4 = ( c + d ) / 2.0;
	vec3 v5 = ( b + d ) / 2.0;
        divide_tetra( a, v0, v1, v2, count - 1 );
        divide_tetra( v0, b, v3, v5, count - 1 );
        divide_tetra( v1, v3, c, v4, count - 1 );
	divide_tetra( v2, v4, v5, d, count - 1 );
    }
    else {
        tetra( a, b, c, d );    
    }
}


void init( void )
{
    vec3 vertices[4] = {
	vec3( 0.0, 0.0, -1.0 ),
	vec3( 0.0, 0.942809, 0.333333 ),
	vec3( -0.816497, -0.471405, 0.333333 ),
	vec3( 0.816497, -0.471405, 0.333333 )
    };	
    divide_tetra( vertices[0], vertices[1], vertices[2], vertices[3],
		  NumTimesToSubdivide );

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);


    //load shaders
    GLuint program = InitShader("vshader_ex2.glsl", "fshader_ex2.glsl");
    glUseProgram(program);

    //initialize vertex position attribute from vertex shader
    GLuint vPos = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPos);
    glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


    GLuint vCol = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vCol);
    glVertexAttribPointer( vCol, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(points)) );

    //glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display()
{
    glClear( GL_COLOR_BUFFER_BIT );
    //glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT );
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    glFlush();
}

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(512, 512);

    //glutInitContextVersion(4, 3);
    //glutInitContextProfie(GLUT_CORE_PROFILE);
    glutCreateWindow("Many Tetrahedra GPU");

    glewInit();
    init();
    printf("OpenGL %s, GLSL %s\n",                         
	    glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION));
    glutDisplayFunc(display);
    glutKeyboardFunc( keyboard );
    
    glutMainLoop();
    return 0;
}