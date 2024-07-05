#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <cstdio>
#include <cstdlib>

const int NumCrvVertices = 1024;

int Index = 0;

GLfloat HermiteBasis[NumCrvVertices][4];

struct HermiteCurve {
    vec2 BndPos[2];
    vec2 BndTan[2];

    vec2 points[NumCrvVertices + 4];
    vec3 colors[NumCrvVertices + 4];

    GLuint vao;
    GLuint vbo;

    GLuint vaoBndPts;
    GLuint vboBndPts;
    vec2 bndPoints[4];
    vec3 bndColors[4];

    HermiteCurve() {
    }
    HermiteCurve(const vec2& p0, const vec2& p1, const vec2& v0, const vec2& v1)
    {
        BndPos[0] = p0;
        BndPos[1] = p1;
        BndTan[0] = v0;
        BndTan[1] = v1;
    }

    ~HermiteCurve() {
            
    }

    void evalulate() {
        for (int i = 0; i < NumCrvVertices; i++) {
            for (int j = 0; j < 2; j++)
                points[i + 2][j] = HermiteBasis[i][0] * BndPos[0][j] + HermiteBasis[i][1] * BndPos[1][j]
                                 + HermiteBasis[i][2] * BndTan[0][j] + HermiteBasis[i][3] * BndTan[1][j];
        }
    }
    void updateForRendering() {
        evalulate();

        // for drawing tangent handles
        points[0] = BndPos[0] + BndTan[0];
        points[1] = BndPos[0];

        points[NumCrvVertices + 2] = BndPos[1];
        points[NumCrvVertices + 3] = BndPos[1] + BndTan[1];


        bndPoints[0] = points[0];
        bndPoints[1] = points[1];
        bndPoints[2]=  points[NumCrvVertices + 2];
        bndPoints[3] = points[NumCrvVertices + 3];
    }
};

HermiteCurve curve;
int crv_edit_handle = -1;

// precompute polynomial bases for Hermite spline
void precomputeHermiteBasis()
{
    GLfloat t, t2, t3;
    for (int i = 0; i < NumCrvVertices; i++) {
        t = i * 1.0 / (GLfloat)(NumCrvVertices - 1.0);
        t2 = t * t;
        t3 = t2 * t;
        HermiteBasis[i][0] = 2 * t3 - 3 * t2 + 1;
        HermiteBasis[i][1] = -2 * t3 + 3 * t2;
        HermiteBasis[i][2] = t3 - 2 * t2 + t;
        HermiteBasis[i][3] = t3 - t2;
    }
}

//viewing transformation parameters
GLuint model_view; // model-view matrix uniform shader variable location

                   // Projection transformation parameters
GLuint projection; //projection matrix uniform shader variable location

int Width;
int Height;

GLfloat left = -1.0, right = 1.0;
GLfloat bottom = -1.0, top = 1.0;

// If we grasp one of two end points, then return 0/1.
// If we grasp one of two end tangents' tip point, then return 2/3
// otherwise return 0
int HitIndex(HermiteCurve* curve, int x, int y)
{
    int ret = -1;
    GLfloat dist[4], mindist = FLT_MAX;

    // a size of one pixel in the curve coordinate system.
    vec2 pixelLen((right - left) / (GLfloat)(Width), (top - bottom) / (GLfloat)Height);
    
    // the current mouse point in the curve coordinate
    vec2 mousePt, tmpVec;
    mousePt.x = left + pixelLen[0] * (GLfloat)x;
    mousePt.y = bottom + pixelLen[1] * (GLfloat)y;

    // Measure the squared distance between mouse point and handles
    tmpVec = curve->BndPos[0] - mousePt;
    dist[0] = dot(tmpVec, tmpVec);
    tmpVec = curve->BndPos[1] - mousePt;
    dist[1] = dot(tmpVec, tmpVec);

    tmpVec = (curve->BndPos[0] + curve->BndTan[0]) - mousePt;
    dist[2] = dot(tmpVec, tmpVec);
    tmpVec = (curve->BndPos[1] + curve->BndTan[1]) - mousePt;
    dist[3] = dot(tmpVec, tmpVec);
    
    for (int i = 0; i < 4; i++) {
        if (mindist > dist[i]) {
            ret = i;
            mindist = dist[i];
        }
    }

    // if clicked within 10 pixel radius of one of handles then return
    if (mindist < 100.0 * dot(pixelLen, pixelLen))
        return ret;
    else
        return -1;
}

void init( void )
{
    precomputeHermiteBasis();

    curve = HermiteCurve(vec2(0.0, 0.0), vec2(0.9, 0.0), vec2(0.0, 0.9), vec2(0.0, -0.9));
    
    curve.updateForRendering();

    //for colors
    curve.colors[0] = vec3(1.0, 0.0, 0.0);
    curve.colors[1] = vec3(1.0, 0.0, 0.0);
    for (int i = 0; i < NumCrvVertices; i++)
        curve.colors[i + 2] = vec3(0.0, 0.0, 0.0);
    curve.colors[NumCrvVertices + 2] = vec3(1.0, 0.0, 0.0);
    curve.colors[NumCrvVertices + 3] = vec3(1.0, 0.0, 0.0);
    
    //for boundary colors;
    curve.bndColors[0] = vec3(0.0, 0.0, 1.0);
    curve.bndColors[1] = vec3(1.0, 0.0, 1.0);
    curve.bndColors[2] = vec3(1.0, 0.0, 1.0);
    curve.bndColors[3] = vec3(0.0, 0.0, 1.0);

   glGenVertexArrays(1, &(curve.vao));
   glBindVertexArray(curve.vao);

   glGenBuffers(1, &(curve.vbo));
   glBindBuffer(GL_ARRAY_BUFFER, curve.vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(curve.points) + sizeof(curve.colors), NULL, 
                GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(curve.points), curve.points);
   glBufferSubData(GL_ARRAY_BUFFER, sizeof(curve.points), sizeof(curve.colors), curve.colors);
   glBindVertexArray(0);

   glGenVertexArrays(1, &(curve.vaoBndPts));
   glBindVertexArray(curve.vaoBndPts);

   glGenBuffers(1, &(curve.vboBndPts));
   glBindBuffer(GL_ARRAY_BUFFER, curve.vboBndPts);
   glBufferData(GL_ARRAY_BUFFER, sizeof(curve.bndPoints) + sizeof(curve.bndColors), NULL,
       GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(curve.bndPoints), curve.bndPoints);
   glBufferSubData(GL_ARRAY_BUFFER, sizeof(curve.bndPoints), sizeof(curve.bndColors), curve.bndColors);
   glBindVertexArray(0);

   // 
   //load shaders
   GLuint program = InitShader("vshader_2dhermite2.glsl", "fshader_2dhermite2.glsl");
   glUseProgram(program);

   //initialize vertex position attribute from vertex shader
  /* glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(curve.points)));*/

   //initialize uniform variable from vertex shander
   model_view = glGetUniformLocation(program, "model_view");
   projection = glGetUniformLocation(program, "projection");

   //OpenGL rendering initiailization
   glEnable(GL_DEPTH_TEST);
   glClearColor(1.0, 1.0, 1.0, 0.0);

}

void display()
{
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT );

    mat4 p = Ortho2D(left, right, bottom, top);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p);

    glBindVertexArray(curve.vao);
    glBindBuffer(GL_ARRAY_BUFFER, curve.vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(curve.points)));

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(curve.points), curve.points);

    glDrawArrays(GL_LINE_STRIP, 0, NumCrvVertices + 4);
    glBindVertexArray(0);

    // for boundary points
    glBindVertexArray(curve.vaoBndPts);
    glBindBuffer(GL_ARRAY_BUFFER, curve.vboBndPts);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(curve.bndPoints)));

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(curve.bndPoints), curve.bndPoints);

    glPointSize(10.0);
    glDrawArrays(GL_POINTS, 0, 4);
    glBindVertexArray(0);
    
    glutSwapBuffers();
}

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;


    case ' ':  // reset values to their defaults
        left = -1.0;
        right = 1.0;
        bottom = -1.0;
        top = 1.0;
       
        break;
    }
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    Width = width;
    Height = height;
}

void mouse(GLint button, GLint action, GLint x, GLint y)
{
    if (GLUT_LEFT_BUTTON == button)
    {
        if (GLUT_LEFT_BUTTON == button)
        {
            switch (action)
            {
            case GLUT_DOWN:
                crv_edit_handle = HitIndex(&curve, x, Height - y);
                break;
            case GLUT_UP:
                crv_edit_handle = -1;
                break;
            default: break;
            }
        }
        glutPostRedisplay();
    }
}

void mouseMove(GLint x, GLint y)
{
    if (crv_edit_handle != -1) {
        vec2 pixelLen((right - left) / (GLfloat)(Width), (top - bottom) / (GLfloat)Height);
        vec2 mousePt;
        mousePt.x = left + (GLfloat)x * pixelLen[0];
        mousePt.y = bottom + (GLfloat)(Height - y) * pixelLen[1];

        if (crv_edit_handle < 2) {
            curve.BndPos[crv_edit_handle] = mousePt;
        }
        else {
            curve.BndTan[crv_edit_handle - 2] = mousePt - curve.BndPos[crv_edit_handle - 2];
        }
        curve.updateForRendering();
        
        glutPostRedisplay();
    }
}
int main(int argc, char **argv)
{
    Width = 512;
    Height = 512;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(Width, Height);

    //glutInitContextVersion(4, 3);
    //In case of MacOS
    //glutInitContextProfie(GLUT_CORE_PROFILE);
    glutCreateWindow("Hermite 2D curve (with interactive editing) ");

    glewInit();
    init();
   
    /*
    printf("OpenGL %s, GLSL %s\n",                         
	    glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION));
    */

    glutDisplayFunc(display);
    glutKeyboardFunc( keyboard );
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMove);

    glutMainLoop();
    return 0;
}