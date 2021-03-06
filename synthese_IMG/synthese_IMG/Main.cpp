#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define _USE_MATH_DEFINES
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>

#include "Sphere.h"
#include "Cylindre.h"

//================================================
/*variables*/

char presse;
int anglex, angley, x, y, xold, yold;
int haut = 0;

double zoom = 60.0f;
GLfloat posProj[4];
GLfloat posAmbient[4];

//texture de la sph�re
int width_tex_sphere;
int height_tex_sphere;
int bpp_tex_sphere;
GLuint id_tex_sphere;
unsigned char* tex_sphere;

//texture des cylindres
int width_tex_cylindre;
int height_tex_cylindre;
int bpp_tex_cylindre;
GLuint id_tex_cylindre;
unsigned char* tex_cylindre;

//valeurs et statut des animations de la fourmi
double animValue_tete = 0.0;
bool incrTete = true;
double animValue_queue = 0.0;
bool incrQueue = false;
double animValue_antenne = 0.0;
bool incrAntenne = true;
double animValue_mandibule = 0.0;
bool incrMandibule = false;
double animValue_patte = 0.0;
bool incrPatte = true;

//yeux de la fourmi (primitive sph�re)
int n_oeil = 20;
int m_oeil = 20;
Point* pts_oeil1;
Point* pts_oeil2;
Face* faces_oeil1;
Face* faces_oeil2;

//patte de la fourmi (primitive cylindre)
int n_cylindre = 50;
Point* pts_patteHaut;
Point* pts_patteBas;
Face* faces_patteHaut;
Face* faces_patteBas;

//===================================================
/*signatures des m�thodes*/
void affichage();
void clavier(unsigned char touche, int x, int y);
void reshape(int x, int y);
void idle();
void mouse(int bouton, int etat, int x, int y);
void mousemotion(int x, int y);
void specialKeyInput(int key, int x, int y);
void updateAnimPattes();
unsigned char* loadJpegImage(const char* fichier, int* width, int* height, int* bpp);
void marron();

//================================================================
/*m�thodes affichage fourmi*/
void patte()
{
    glPushMatrix();
        glRotatef(15, -1, 0, 0);

        //cylindre coll� � l'abdomen
        glPushMatrix();
            marron();
            glTranslatef(0, 0, 1.0);
            glScalef(0.15, 0.15, 2.0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, id_tex_cylindre);
            render_cylindre(faces_patteHaut, pts_patteHaut, n_cylindre);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        //jointure
        glPushMatrix();
            marron();
            glTranslatef(0, 0, 3.15);
            glScalef(0.2, 0.2, 0.2);
            glutSolidSphere(1.0, 10, 10);
        glPopMatrix();

        //cylindre bas
        glPushMatrix();
            marron();

            glTranslatef(0, 0, 3.15);
            glRotatef(90, 1.0, 0.0, 0.0);

            glTranslatef(0, 0, 0.2);
            glScalef(0.10, 0.10, 2.0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, id_tex_cylindre);
            render_cylindre(faces_patteBas, pts_patteBas, n_cylindre);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();

    glPopMatrix();
}

void antenne()
{
    glPushMatrix();
        glRotatef(animValue_antenne, 0, 1, 0);

        //base de l'antenne (li�e � le t�te)
        glPushMatrix();
            marron();
            glTranslatef(0, 0.8, 0);
            glRotatef(90, 0, 1, 0);
            glScalef(0.05, 0.6, 0.05);
            glutSolidCylinder(1, 1, 10, 10);
        glPopMatrix();

        //jointure inter
        glPushMatrix();
            marron();
            glTranslatef(-0.02, 1.35, 0);
            glScalef(0.03, 0.03, 0.03);
            glutSolidSphere(1.0, 10, 10);
        glPopMatrix();

        //bout de l'antenne
        glPushMatrix();
            marron();
            glTranslatef(0, 1.35, 0);
            glRotatef(90, 0, -1, 0);
            glScalef(0.02, 0.02, 2.0);
            glutSolidCylinder(1, 1, 10, 10);
        glPopMatrix();

    glPopMatrix();
}

void abdomen()
{
    glPushMatrix();
        marron();
        glScalef(1.6, 1.0, 1.0);
        glutSolidSphere(1, 50, 50);
    glPopMatrix();
}

void queue()
{
    glPushMatrix();
    glRotatef(animValue_queue, 0, 0, 1);

        glPushMatrix();

            glTranslatef(1.4, 0, 0);
            glRotatef(20, 0, 0, 1);

            //jointure abdomen - queue
            glPushMatrix();
                marron();
                glScalef(0.5, 0.5, 0.35);
                glRotatef(90, 0, 1.0, 0);
                glutSolidCylinder(1.0, 1.0, 10, 10);
            glPopMatrix();

            //queue
            marron();
            glTranslatef(1.8, 0, 0);
            glScalef(1.8, 1.2, 1.2);
            glutSolidSphere(1, 50, 50);
        glPopMatrix();

    glPopMatrix();
}

void mandibule()
{
    //base de la mandibule (li�e � la t�te)
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glScalef(0.1, 0.1, 1);
        glutSolidCylinder(1, 1, 10, 10);
    glPopMatrix();

    //jointure
    glPushMatrix();
        glTranslatef(0, -1.1, 0);
        glScalef(0.1, 0.1, 0.1);
        glutSolidSphere(1, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, -1.1, 0);
        glRotatef(animValue_mandibule, 0, 0, 1);

        //bout de la mandibule
        glPushMatrix();
            glRotatef(90, 1, 1, 0);
            glScalef(0.1, 0.1, 1.2);
            glTranslatef(0, 0, 0.05);
            glutSolidCylinder(1, 1, 10, 10);
        glPopMatrix();

    glPopMatrix();
}

void tete()
{
    //animation continue
    glPushMatrix();
    glRotatef(animValue_tete, 0, 1, 0);

        //jointure
        glPushMatrix();
            marron();
            glTranslatef(-2.0, 0, 0);
            glRotatef(90, 0, 1.0, 0);
            glScalef(0.3, 0.3, 0.6);
            glutSolidCylinder(1.0, 1.0, 10, 10);
        glPopMatrix();

        //t�te
        glPushMatrix();
            marron();
            glTranslatef(-2.5, 0, 0);
            glScalef(1, 0.7, 0.9);
            glutSolidSphere(1.0, 100, 100);
        glPopMatrix();

        //mandibules
        glPushMatrix();
            marron();
            glTranslatef(-2.5, 0, 0);

            //mandibule droite
            glPushMatrix();
                glRotatef(90, 0, 1, 0);
                glRotatef(90, 1, 0, 0);
                glTranslatef(-0.5, -0.8, 0);
                glScalef(0.5, 0.5, 0.5);
                mandibule();
            glPopMatrix();

            //mandibule gauche
            glPushMatrix();
                glRotatef(90, 0, -1, 0);
                glRotatef(90, -1, 0, 0);
                glTranslatef(-0.5, -0.8, 0);
                glScalef(0.5, 0.5, 0.5);
                mandibule();
            glPopMatrix();
        glPopMatrix();

        //yeux
        glPushMatrix();
            glTranslatef(-2.7, 0, 0);
            glScalef(0.2, 0.2, 0.2);

            glPushMatrix();
                glTranslatef(0, 0.2, -4);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, id_tex_sphere);
                render_sphere(pts_oeil1, faces_oeil1, n_oeil, m_oeil);
                glDisable(GL_TEXTURE_2D);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(0, 0.2, 4);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, id_tex_sphere);
                render_sphere(pts_oeil2, faces_oeil2, n_oeil, m_oeil);
                glDisable(GL_TEXTURE_2D);
            glPopMatrix();

        glPopMatrix();

        //antennes
        glPushMatrix();
            glTranslatef(-2.5, 0, 0);
            glRotatef(20, 0, 0, 1);

            glPushMatrix();
                glTranslatef(0, 0, 0.5);
                glRotatef(20, 0, 1, 0);
                antenne();
            glPopMatrix();

            glPushMatrix();
                glTranslatef(0, 0, -0.5);
                glRotatef(20, 0, -1, 0);
                antenne();
            glPopMatrix();

        glPopMatrix();
    glPopMatrix();
}

void fourmi()
{
    abdomen();
    tete();
    queue();

    //pattes cot� gauche
    glPushMatrix();
        glRotatef(10 * cos(animValue_patte + (M_PI) / 2), 1, 1, 0);
        patte();
    glPopMatrix();

    glPushMatrix();
        glRotatef(10 * cos(animValue_patte + (M_PI) / 3), 1, 1, 0);

        glPushMatrix();
            glRotatef(45, 0.0, 1.0, 0.0);
            patte();
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        glRotatef(10 * cos(animValue_patte + (2 * M_PI) / 3), 1, 1, 0);

        glPushMatrix();
            glRotatef(45, 0.0, -1.0, 0.0);
            patte();
        glPopMatrix();
    glPopMatrix();

    //pattes cot� droit
    glPushMatrix();
    glScalef(1.0, 1.0, -1.0);

        glPushMatrix();
            glRotatef(10 * sin(animValue_patte + (M_PI) / 2), 1, 1, 0);
            patte();
        glPopMatrix();

        glPushMatrix();
            glRotatef(10 * sin(animValue_patte + (M_PI) / 3), 1, 1, 0);
            glPushMatrix();
                glRotatef(45, 0.0, -1.0, 0.0);
                patte();
            glPopMatrix();
        glPopMatrix();

        glPushMatrix();
            glRotatef(10 * sin(animValue_patte + (2 * M_PI) / 3), 1, 1, 0);
            glPushMatrix();
                glRotatef(45, 0.0, 1.0, 0.0);
                patte();
            glPopMatrix();
        glPopMatrix();

    glPopMatrix();
}
//=================================================================

void marron()
{
    glColor3f(0.7, 0.3, 0);
}

int main(int argc, char** argv)
{
    /*cr�ation et param�trage de la fen�tre et de GLUT*/
    glutInit(&argc, argv);
    glutSetOption(GLUT_MULTISAMPLE, 8); //anticr�nelage x8
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("cube");

    /* Initialisation d'OpenGL */
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    /*application des r�flections de mat�riau (type OR)*/
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    GLfloat ambientOr[] = { 0.24725, 0.1995, 0.0745, 1.0f };
    GLfloat diffuseOr[] = { 0.75164, 0.60648, 0.22648, 1.0f };
    GLfloat specularOr[] = { 0.628281, 0.555802, 0.366065, 1.0f };
    GLfloat shine = 51.2;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientOr);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseOr);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularOr);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

    /*Lumi�re 0: spot blanc au dessus de la fourmi*/
    glEnable(GL_LIGHT0);
    posProj[0] = 0;
    posProj[1] = 5;
    posProj[2] = 0;
    posProj[3] = 1.0;   //important pour d�finir un spot
    GLfloat dirProj[] = { 0.0, 0.0, 0.0, 1.0};
    GLfloat ambient[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, posProj);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dirProj);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
    

    /*Lumi�re 1: ambiente verte en dessous de la fourmi*/
    posAmbient[0] = 0;
    posAmbient[1] = -5;
    posAmbient[2] = 0;
    posAmbient[3] = 0;
    GLfloat ambient2[] = { 0.0, 0.4, 0.0, 1.0 };
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient2);
    glLightfv(GL_LIGHT1, GL_POSITION, posAmbient);
    glEnable(GL_LIGHT1);

    /*cr�ation des yeux (primitive Sphere)*/
    pts_oeil1 = sphere(n_oeil, m_oeil);
    faces_oeil1 = sphere_faces(pts_oeil1, n_oeil, m_oeil);

    pts_oeil2 = sphere(n_oeil, m_oeil);
    faces_oeil2 = sphere_faces(pts_oeil2, n_oeil, m_oeil);

    /*cr�ation des cylindres des pattes*/
    pts_patteHaut = cylindre(n_cylindre, 1, 1);
    pts_patteBas = cylindre(n_cylindre, 1, 1);

    faces_patteHaut = cylindre_faces(pts_patteHaut, n_cylindre);
    faces_patteBas = cylindre_faces(pts_patteBas, n_cylindre);

    /*chargement et param�trage des textures*/
    glGenTextures(1, &id_tex_sphere);
    glBindTexture(GL_TEXTURE_2D, id_tex_sphere);
    tex_sphere = loadJpegImage("noir.png", &width_tex_sphere, &height_tex_sphere, &bpp_tex_sphere);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_tex_sphere, height_tex_sphere, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_sphere);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glGenTextures(1, &id_tex_cylindre);
    glBindTexture(GL_TEXTURE_2D, id_tex_cylindre);
    tex_cylindre = loadJpegImage("gold.png", &width_tex_cylindre, &height_tex_cylindre, &bpp_tex_cylindre);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_tex_cylindre, height_tex_cylindre, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_cylindre);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    /*liaison fonctions glut avec fonctions d�finies*/
    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeyInput);

    glutMainLoop();
    
    return 0;
}

unsigned char* loadJpegImage(const char* fichier, int* width, int* height, int* bpp)
{
    /*chargement de l'image*/
    unsigned char* image = stbi_load(fichier, width, height, bpp, 3);

    /*v�rification du chargement*/
    if (image == nullptr)
        std::cout << "Erreur, impossible de charger l'image " << fichier << std::endl;
    else
        std::cout << "Texture chargee: " << fichier << std::endl;

    return image;
}


void updateAnimPattes()
{
    /*mise a jour de l'animation des pattes. pas dans idle() car d�clench�e par un event clavier*/
    if (incrPatte)
        animValue_patte += 0.3;
    else
        animValue_patte -= 0.5;
}

void idle()
{
    /*mise a jour des animations*/

    /*v�rification des seuils*/
    if (animValue_tete >= 15.0)
        incrTete = false;
    else if (animValue_tete <= -15.0)
        incrTete = true;

    if (animValue_queue >= 10.0)
        incrQueue = false;
    else if (animValue_queue <= -10.0)
        incrQueue = true;

    if (animValue_antenne >= 10.0)
        incrAntenne = false;
    else if (animValue_antenne <= 0.9)
        incrAntenne = true;

    if (animValue_mandibule >= 10.0)
        incrMandibule = false;
    else if (animValue_mandibule <= 0.0)
        incrMandibule = true;

    /*modification des valeurs*/
    if (incrTete)
        animValue_tete += 0.01;
    else
        animValue_tete -= 0.01;

    if (incrQueue)
        animValue_queue += 0.01;
    else
        animValue_queue -= 0.01;

    if (incrAntenne)
        animValue_antenne += 0.1;
    else
        animValue_antenne -= 0.1;

    if (incrMandibule)
        animValue_mandibule += 0.04;
    else
        animValue_mandibule -= 0.04;

    /*actualisation de l'affichage*/
    glutPostRedisplay();
}

void affichage()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);

    /*d�finition de la perspective et application du zoom*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, 500.0 / 500.0, 0.1, 100.0);
    glPopMatrix();

    /*passage en affichage des objets*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*positionnement de la cam�ra*/
    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(angley, 1.0, 0.0, 0.0);
    glRotatef(anglex, 0.0, 1.0, 0.0);

    /*application des lumi�res*/
    glLightfv(GL_LIGHT0, GL_POSITION, posProj);
    glLightfv(GL_LIGHT1, GL_POSITION, posAmbient);

    /*application des textures*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    fourmi();

    glFlush();
    glutSwapBuffers();
}

void clavier(unsigned char touche, int x, int y)
{
    switch (touche)
    {
    case 'z':   /*Zoom avant*/
        zoom -= 10.0f;
        std::cout << "zoom: " << zoom << std::endl;
        glutPostRedisplay();
        break;

    case 'Z':   /*Zomm arriere*/
        zoom += 10.0f;
        std::cout << "zoom: " << zoom << std::endl;
        glutPostRedisplay();
        break;

    case '1': /* affichage du carre plein */
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutPostRedisplay();
        break;
    case '2': /* affichage en mode fil de fer */
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glutPostRedisplay();
        break;
    case '3': /* Affichage en mode sommets seuls */
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glutPostRedisplay();
        break;

    case 'e':
        updateAnimPattes();
        glutPostRedisplay();
        break;

    case 27: /*la touche 'q' permet de quitter le programme */
        exit(0);
    }
}

void reshape(int x, int y)
{
    if (x < y)
        glViewport(0, (y - x) / 2, x, x);
    else
        glViewport((x - y) / 2, 0, y, y);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        presse = 1; 
        xold = x; 
        yold = y;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        presse = 0;
}

void specialKeyInput(int key, int x, int y)
{
    /*Modification des positions de la cam�ra*/
    switch (key)
    {
    case GLUT_KEY_UP:
        angley += 10.0f;
        glutPostRedisplay();
        break;

    case GLUT_KEY_DOWN:
        angley -= 10.0f;
        glutPostRedisplay();
        break;

    case GLUT_KEY_LEFT:
        anglex += 10.0f;
        glutPostRedisplay();
        break;

    case GLUT_KEY_RIGHT:
        anglex -= 10.0f;
        glutPostRedisplay();
        break;
    }
}

void mousemotion(int x, int y)
{
    if (presse)
    {
        anglex = anglex + (x - xold);
        angley = angley + (y - yold);
        glutPostRedisplay();
    }

    xold = x;
    yold = y;
}
