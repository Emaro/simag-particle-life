#include "HelperDraw.h"

#include <glad/gl.h>
#include <imgui/imgui.h>
#include <imgui_glfw/imgui_impl_glfw.h>
#include <imgui_glfw/imgui_impl_opengl2.h>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>

#include "../Helper/HelperDraw.h"

namespace Helper
{
    void drawQuad(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3)
    {
        glBegin(GL_QUADS);
        glVertex3fv(&p0[0]);
        glVertex3fv(&p1[0]);
        glVertex3fv(&p2[0]);
        glVertex3fv(&p3[0]);
        glEnd();
    }

    void drawQuad(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3, glm::vec3 const& normal)
    {
        glBegin(GL_QUADS);
        glNormal3fv(&normal[0]);
        glVertex3fv(&p0[0]);
        glVertex3fv(&p1[0]);
        glVertex3fv(&p2[0]);
        glVertex3fv(&p3[0]);
        glEnd();
    }

    void drawPlane()
    {
        static const float s = 10.0f;
        static const float eps = -0.01f; //  -0.01f;
        static const GLfloat floorVertices[4][3] = {
          { -s, eps, s },
          { s, eps, s },
          { s, eps, -s },
          { -s, eps, -s },
        };

        glBegin(GL_QUADS);
        glVertex3fv(floorVertices[0]);
        glVertex3fv(floorVertices[1]);
        glVertex3fv(floorVertices[2]);
        glVertex3fv(floorVertices[3]);
        glEnd();
    }

    void drawFloor(float y, float alpha)
    {
        // Draw "bottom" of floor in blue.
        glFrontFace(GL_CW);
        glColor4f(0.0f, 0.2f, 0.2f, alpha >= 0.0 ? alpha : 1.0f);
        //glColor4f(0.2f, 0.2f, 0.2f, alpha >= 0.0 ? alpha : 1.0f);
        glPushMatrix();
        glTranslatef(0, y, 0);
        Helper::drawPlane();
        glPopMatrix();
        glFrontFace(GL_CCW);

        // Draw "top" of floor.
        glColor4f(0.0f, 0.3f, 0.3f, alpha >= 0.0 ? alpha : 1.0f);
        //glColor4f(0.2f, 0.2f, 0.2f, alpha >= 0.0 ? alpha : 1.0f);
        glPushMatrix();
        glTranslatef(0, y, 0);
        Helper::drawPlane();
        glPopMatrix();

        // Draw grid on floor
        /*
        glDisable(GL_DEPTH_TEST);
        glPushMatrix();
        glTranslatef(0, y, 0);
        glRotatef(90, 1, 0, 0);
        drawGrid2d();
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);
        */
    }

    void drawAxis()
    {
        glLineWidth(4.0f);
        glBegin(GL_LINES);
        glColor4f(1, 0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);

        glColor4f(0, 1, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1, 0);

        glColor4f(0, 0, 1, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
        glEnd();
        glLineWidth(1.0f);

        glColor4f(1, 1, 1, 1);
    }

    namespace
    {
        inline void line2d(float x0, float y0, float x1, float y1)
        {
            glVertex2f(x0, y0);
            glVertex2f(x1, y1);
        }

        inline void line3d(glm::vec3 const& x0, glm::vec3 const x1)
        {
            glVertex3fv(&x0[0]);
            glVertex3fv(&x1[0]);
        }
    }

    void drawGrid2d()
    {
        glLineWidth(1.0f);
        glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
        glBegin(GL_LINES);
        for (int i = -5; i <= 5; i++)
        {
            line2d((float)i, -5.0f, (float)i, 5.0f);
            line2d(-5.0f, (float)i, 5.0f, (float)i);
        }
        glEnd();
        glColor4f(1,1,1,1);
    }

    void drawPlane(glm::vec3 const& p, glm::vec3 const& n, bool bDrawNormal)
    {
        glm::vec3 up(0, 1, 0);
        if (fabs(glm::dot(n, up)) > 0.9) up = glm::vec3(1, 0, 0);
        if (fabs(glm::dot(n, up)) > 0.9) up = glm::vec3(0, 0, 1);

        glm::vec3 t1 = glm::normalize(glm::cross(n, up));
        glm::vec3 t2 = glm::normalize(glm::cross(n, t1));

        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (int i = -5; i <= 5; i++)
        {
            line3d(p + t1* (float)i - t2 * 5.0f, p + t1 * (float)i + t2 * 5.0f);
            line3d(p - t1 * 5.0f + t2 * (float)i, p + t1 * 5.0f + t2 * (float)i);
        }

        if (bDrawNormal)
        {
            line3d(p, p + n);
        }
        glEnd();

    }


    void drawLine2d(float x0, float y0, float x1, float y1)
    {
        glBegin(GL_LINES);
        line2d(x0, y0, x1, y1);
        glEnd();
    }

    void drawLine(glm::vec3 const& x0, glm::vec3 const& x1)
    {
        glBegin(GL_LINES);
        glVertex3fv(&x0[0]);
        glVertex3fv(&x1[0]);
        glEnd();
    }

    void drawPoint(glm::vec3 const& p)
    {
        glBegin(GL_POINTS);
        glVertex3fv(&p[0]);
        glEnd();
    }

    void drawCircle2d(glm::vec3 const& p, float radius, int discretization)
    {
        glBegin(GL_LINES);
        for (int i = 0; i <= discretization; i++)
        {
            float angle1 = (float)(i) / (float)discretization * 2.0f * glm::pi<float>();
            float angle2 = (float)(i + 1) / (float)discretization * 2.0f * glm::pi<float>();
            glVertex3f(p.x + cosf(angle1) * radius, p.y + sinf(angle1) * radius, p.z);
            glVertex3f(p.x + cosf(angle2) * radius, p.y + sinf(angle2) * radius, p.z);
        }
        glEnd();
    }

    void drawBox2d(float x0, float y0, float x1, float y1)
    {
        glBegin(GL_LINES);
        glVertex2f(x0, y0); glVertex2f(x1, y0);
        glVertex2f(x1, y0); glVertex2f(x1, y1);
        glVertex2f(x1, y1); glVertex2f(x0, y1);
        glVertex2f(x0, y1); glVertex2f(x0, y0);
        glEnd();
    }

    void drawCube(glm::vec3 const& x0, glm::vec3 const& x1)
    {
        glLineWidth(1.0f);
        glBegin(GL_LINES);

        line3d(glm::vec3(x0.x, x0.y, x0.z), glm::vec3(x1.x, x0.y, x0.z));
        line3d(glm::vec3(x1.x, x0.y, x0.z), glm::vec3(x1.x, x1.y, x0.z));
        line3d(glm::vec3(x1.x, x1.y, x0.z), glm::vec3(x0.x, x1.y, x0.z));
        line3d(glm::vec3(x0.x, x1.y, x0.z), glm::vec3(x0.x, x0.y, x0.z));

        line3d(glm::vec3(x0.x, x0.y, x1.z), glm::vec3(x1.x, x0.y, x1.z));
        line3d(glm::vec3(x1.x, x0.y, x1.z), glm::vec3(x1.x, x1.y, x1.z));
        line3d(glm::vec3(x1.x, x1.y, x1.z), glm::vec3(x0.x, x1.y, x1.z));
        line3d(glm::vec3(x0.x, x1.y, x1.z), glm::vec3(x0.x, x0.y, x1.z));

        line3d(glm::vec3(x0.x, x0.y, x0.z), glm::vec3(x0.x, x0.y, x1.z));
        line3d(glm::vec3(x1.x, x0.y, x0.z), glm::vec3(x1.x, x0.y, x1.z));
        line3d(glm::vec3(x1.x, x1.y, x0.z), glm::vec3(x1.x, x1.y, x1.z));
        line3d(glm::vec3(x0.x, x1.y, x0.z), glm::vec3(x0.x, x1.y, x1.z));

        glEnd();
    }


    void drawSphere(glm::vec3 const& x0, float radius)
    {
        glPushMatrix();
        int nSlices = 10;
        for (int i = 0; i < nSlices; i++)
        {
            glPushMatrix();
            glTranslatef(x0.x, x0.y, x0.z);
            glRotatef(180.0f / (float)nSlices * (float)i, 0, 1, 0);
            drawCircle2d(glm::vec3(0), radius, 20);
            glPopMatrix();
        }
        glPopMatrix();
    }


}
