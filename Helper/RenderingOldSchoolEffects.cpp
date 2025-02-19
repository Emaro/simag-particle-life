#include "RenderingOldSchoolEffects.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
    void determineShadowMatrix(
        glm::mat4& shadowMat,
        glm::vec3 const& lightPos,
        glm::vec3 const& planeP,
        glm::vec3 const& planeN)
    {
        float planeDistRoot = -glm::dot(planeP, planeN);
        glm::vec4 planeHesse(planeN, planeDistRoot);
        glm::vec4 lightPosV4(lightPos, 1.0f);

        float dot = glm::dot(planeHesse, lightPosV4);

        shadowMat[0][0] = dot - lightPosV4.x * planeHesse.x;
        shadowMat[1][0] = 0.f - lightPosV4.x * planeHesse.y;
        shadowMat[2][0] = 0.f - lightPosV4.x * planeHesse.z;
        shadowMat[3][0] = 0.f - lightPosV4.x * planeHesse.w;

        shadowMat[0][1] = 0.f - lightPosV4.y * planeHesse.x;
        shadowMat[1][1] = dot - lightPosV4.y * planeHesse.y;
        shadowMat[2][1] = 0.f - lightPosV4.y * planeHesse.z;
        shadowMat[3][1] = 0.f - lightPosV4.y * planeHesse.w;

        shadowMat[0][2] = 0.f - lightPosV4.z * planeHesse.x;
        shadowMat[1][2] = 0.f - lightPosV4.z * planeHesse.y;
        shadowMat[2][2] = dot - lightPosV4.z * planeHesse.z;
        shadowMat[3][2] = 0.f - lightPosV4.z * planeHesse.w;

        shadowMat[0][3] = 0.f - lightPosV4.w * planeHesse.x;
        shadowMat[1][3] = 0.f - lightPosV4.w * planeHesse.y;
        shadowMat[2][3] = 0.f - lightPosV4.w * planeHesse.z;
        shadowMat[3][3] = dot - lightPosV4.w * planeHesse.w;
    }
}

void RenderingOldSchoolEffects::beginShadowProjection(
    glm::vec3 const& lightPos, glm::vec3 const& planeP, glm::vec3 const& planeN,
    std::function<void()> const& drawFloor,
    std::function<void()> const& drawFloorAlpha,
    std::function<void()> const& drawScene
)
{
    // mirror
    beginMirror(lightPos, planeP, planeN, drawFloor, drawScene);

    // shadow
    glm::mat4 mShadow;
    determineShadowMatrix(mShadow, lightPos, planeP, planeN);

    // Draw the floor with stencil value 3. This helps us only draw the shadow once per floor pixel(and only on the floor pixels).
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 3, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // blending, to blend in reflection
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_DEPTH_TEST); // Debug mirror
    glDepthFunc(GL_ALWAYS);  // Always pass, but disable DEPTH_TEST
    //glDepthMask(false);; // Debug mirror
    drawFloorAlpha();
    //glDepthMask(true);; // Debug mirror
    glDepthFunc(GL_LESS); // Restore default value
    //glEnable(GL_DEPTH_TEST); // Debug mirror !!!!!!!
    glDisable(GL_BLEND);

    // Scene is rendered after shadows are finished
    // drawScene();

    // Render projected shadows

    // Now, only render where stencil is set above 2 (ie, 3 where the top floor is).Update stencil with 2 where the shadow 
    // gets drawn so we don't redraw (and accidently reblend) the shadow).
    glStencilFunc(GL_LESS, 2, 0xffffffff);  // draw if ==1
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    // To eliminate depth buffer artifacts, we use polygon offset to raise the depth of the projected shadow slightly so 
    // that it does not depth buffer alias with the floor.
    glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(-2.0f, -1.0f);
    glPolygonOffset(-0.1f, -0.002f);


    // Render 50% black shadow color on top of whatever the floor appareance is.
    // Force 50% black.

    // missuse material and light for rendering monochrome shadows

    glColor4f(0.0, 0.0, 0.0, 0.5);
    float alpha = 0.4f;
    float colAmbient[4]{ 0.0f,0.0f,0.0f,0.00f };
    float colDiffuseAlpha[4]{ 0.0f,0.0f,0.0f,alpha };
    float colZero[4]{ 0.0f,0.0f,0.0f,0.0f };
    float colOne[4]{ 1.0f,1.0f,1.0f,1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, colOne);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, colZero);
    glLightfv(GL_LIGHT0, GL_SPECULAR, colZero);

    // OopenGl Docs:
    // While the ambient, diffuse, specular and emission material parameters all have alpha components, 
    // only the diffuse alpha component is used in the lighting computation.
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colDiffuseAlpha);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colZero);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colZero);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_LIGHTING);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Project the shadow.
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(mShadow));
    //glLoadMatrixf(glm::value_ptr(mShadow));
    glDisable(GL_DEPTH_TEST);
    drawScene();
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_STENCIL_TEST);

    drawScene();
}



void RenderingOldSchoolEffects::endShadowProjection()
{

}

void RenderingOldSchoolEffects::beginMirror(
    glm::vec3 const& lightPos, glm::vec3 const& planeP, glm::vec3 const& planeN,
    std::function<void()> const& drawFloor,
    std::function<void()> const& drawScene
)
{
    // Don't mirror objects below floor => clipPlane
    float dist = glm::dot(glm::normalize(planeN), planeP);
    const GLdouble equation[] = { (double)-planeN.x, (double)-planeN.y, (double)-planeN.z, (double)dist }; // Ax + By + Cz + D = 0
    glClipPlane(GL_CLIP_PLANE0, equation);
    glEnable(GL_CLIP_PLANE0);

    // Don't update color or depth.
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // Draw 1 into the stencil buffer.
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

    // Now render floor; floor pixels just get their stencil set to 1.
    drawFloor();

    // Re-enable update of color and depth.
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    // Now, only render where stencil is set to 1.
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);  // draw if ==1
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glPushMatrix();

    // The critical reflection step: Reflect dinosaur through the floor
    // (the Y=0 plane) to make a relection.
    //glScalef(1.0, -1.0, 1.0);
    
    float floorY = glm::dot(planeP, planeN);
    glTranslatef(0, floorY, 0);
    glScalef(1.0, -1.0, 1.0);
    glTranslatef(0, -floorY, 0);

    // Reflect the light position. 
    // glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // To avoid our normals getting reversed and hence botched lighting
    // on the reflection, turn on normalize.
    // glEnable(GL_NORMALIZE);
    // glCullFace(GL_FRONT);

    drawScene();

    // Disable noramlize again and re-enable back face culling.
    //glDisable(GL_NORMALIZE);
    //glCullFace(GL_BACK);

    glPopMatrix();

    /* Switch back to the unreflected light position. */
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glDisable(GL_STENCIL_TEST);

    glDisable(GL_CLIP_PLANE0);
}






