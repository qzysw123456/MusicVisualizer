//
//  main.cpp
//  glutanimation
//
//  Created by QiuZhiyue on 12/10/17.
//  Copyright © 2017 QiuZhiyue. All rights reserved.
//

#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <iostream>
#include <cmath>
#include <FreeImage.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
GLuint texID[3],tex00;  // Texture ID's for the three textures.

char* textureFileNames = "/Users/qzy/Desktop/glutanimation/glutanimation/milky.jpg";

double fft[1026];
double get(double i,double j)
{
    double dis = (i*i+j*j)/1800.0*1000;
    return 100*fft[(int)dis];
}

void initGL() {
    // called by main() to initialize the OpenGL drawing context
    
    glClearColor(0.0, 0.0, 0.0, 1.0); // background color
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float white[4] = { 1, 1, 1, 1 };  // A white material, suitable for texturing.
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( -3.75,3.75, -1.5,1.5, -2,2 );
    glMatrixMode(GL_TEXTURE);  // Matrix mode for manipulating the texture transform matrix.
    glLoadIdentity();
    glScalef(1,-1,1);  // It turns out the texture coordinates for the GLUT teapot put an upside
    // down texture on the sides of the teapot.  To fix that, I apply a texture
    // transform that flips the texture vertically.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}  // end initGL()

void changeSize(int w, int h) {
    
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0)
        h = 1;
    
    float ratio =  w * 1.0 / h;
    
    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    
    // Reset Matrix
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    
    // Set the correct perspective.
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    
    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

float angle = 0.0f;

void renderScene(void) {
    
    
    for(int i=1;i<=1025;i++) scanf("%lf",&fft[i]);
    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(0.0f, 20.0f, 40.0f,
              0.0f, 5.0f,  0.0f,
              0.0f, 1.0f,  0.0f);
    
    //glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    GLfloat delta = 1;
    for (GLfloat i = -30;i<=30;i+=delta)
        for(GLfloat j = -30;j<=30;j+=delta)
        {
            if(j!=30){
                glVertex3f(i, get(i, j), j);
                glVertex3f(i, get(i, j+delta), j+delta);
            }
            if(i!=30){
                glVertex3f(i, get(i,j), j);
                glVertex3f(i+delta, get(i+delta,j), j);
            }
        }
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, tex00 );
    
    glPushMatrix();
    glRotatef(-20, 1, 0, 0);
    glRotatef(angle, 0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1);
    glVertex3f(-65, 65, -35);
    glTexCoord2f(0, 1);
    glVertex3f(65, 65, -35);
    glTexCoord2f(0, 0);
    glVertex3f(65, -65, -35);
    glTexCoord2f(1, 0);
    glVertex3f(-65, -65, -35);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    angle+=0.1f;
    
    glutSwapBuffers();
}

void loadTextures() {
    
    glGenTextures( 1, &tex00 );  // Get the texture object IDs.

    void* imgData;  // Pointer to image color data read from the file.
    int imgWidth;   // The width of the image that was read.
    int imgHeight;  // The height.
    FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames);
    if (format == FIF_UNKNOWN) {
        printf("Unknown file type for texture image file %s\n", textureFileNames);
        return ;
    }
    FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames, 0);  // Read image from file.
    if (!bitmap) {
        printf("Failed to load image %s\n", textureFileNames);
        return ;
    }
    FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap);  // Convert to RGB or BGR format
    FreeImage_Unload(bitmap);
    imgData = FreeImage_GetBits(bitmap2);     // Grab the data we need from the bitmap.
    imgWidth = FreeImage_GetWidth(bitmap2);
    imgHeight = FreeImage_GetHeight(bitmap2);
    if (imgData) {
        printf("Texture image loaded from file %s, size %dx%d\n",
               textureFileNames, imgWidth, imgHeight);
        int format; // The format of the color data in memory, depends on platform.
        if ( FI_RGBA_RED == 0 )
            format = GL_RGB;
        else
            format = GL_BGR;
        glBindTexture( GL_TEXTURE_2D, tex00 );  // Will load image data into texture object #i
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, format,
                     GL_UNSIGNED_BYTE, imgData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Required since there are no mipmaps.
    }
    else {
        printf("Failed to get texture data from %s\n", textureFileNames);
    }
    
}
int setDelay = 32;
void My_timer_routine( int t )
{
    // Animation code
    
    // Update display
    glutPostRedisplay();
    // Reset timer
    glutTimerFunc( setDelay, My_timer_routine, 0);
}
int main(int argc, char **argv) {
    
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music *background;
    background = Mix_LoadMUS("/Users/qzy/Desktop/glutanimation/glutanimation/roam.wav");
    if(!background) printf("ca %s\n",Mix_GetError());
    if(Mix_PlayMusic(background, 1)==-1) printf("hee\n");
   // SDL
    freopen("/Users/qzy/Desktop/glutanimation/glutanimation/data.txt", "r", stdin);
    
    //for(int i=1;i<=1024;i++) scanf("%lf",&fft[i]);
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE );//| GLUT_RGBA
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,600);
    glutCreateWindow("FinalProject");
    
    initGL();
    loadTextures();
    printf("hehe\n");
    
    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    //glutIdleFunc(renderScene);
    
    glutTimerFunc( setDelay, My_timer_routine, 0);
    // enter GLUT event processing cycle
    glutMainLoop();
    
    Mix_CloseAudio();
    Mix_FreeMusic(background);
    return 1;
}
