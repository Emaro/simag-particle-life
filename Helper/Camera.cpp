#include "Camera.h"

#include <glad/gl.h>

#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/intersect.hpp>

#include "GameCameraState.h"

Camera::Camera(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow)
    : m_sizeFramebuffer(sizeFramebuffer)
    , m_sizeWindow(sizeWindow)
    , m_is2d(false)
    , m_fov(0.785398163f)
    , m_near(0.1f)
    , m_far(200.0f)
    , m_position(0.0f, 0.0f, -5.0f)
    , m_lookAt(0)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_projection(1.0f)
    , m_view(1.0f)
    , m_floorY(0.0f)
    , m_bMirror(false)
{
    updateView();
}

Camera::~Camera()
{
}

void Camera::setResolution(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow)
{
    m_sizeFramebuffer = sizeFramebuffer;
    m_sizeWindow = sizeWindow;

    updateView(); 
}

float Camera::getFov() const
{
    return m_fov;
}

void Camera::setFovY(float f)
{
    m_fov = f;
}

void Camera::setNear(float inNear)
{
    m_near = inNear;
}

glm::vec2 Camera::SSToWorld2d(glm::vec2 pos)
{
    const int& width = m_sizeWindow.x;
    const int& height = m_sizeWindow.y;
    glm::vec2 vm11(pos.x / width * 2.0f - 1.0f, (height-pos.y) / height * 2.0f - 1.0f);
    return glm::inverse(m_view) * glm::vec4(vm11,0,1);
}

glm::vec2 Camera::worldToSS2d(glm::vec2 pos)
{
    const int& width = m_sizeWindow.x;
    const int& height = m_sizeWindow.y;
    glm::vec2 vm11 = m_view * glm::vec4(pos,0,1);
    vm11 = vm11 * 0.5f + 0.5f;
    vm11 *= glm::vec2(width, height);
    vm11.y = height - vm11.y;
    return vm11;
}

glm::vec3 Camera::SSToWorld3d(glm::vec2 posSS, glm::vec3 const& planeP)
{
    const int& width = m_sizeWindow.x;
    const int& height = m_sizeWindow.y;
    glm::vec2 vm11(posSS.x / width * 2.0f - 1.0f, (height - posSS.y) / height * 2.0f - 1.0f);
    glm::vec4 posWS_depth1_v4 = glm::inverse(getViewProjection())* glm::vec4(vm11, 1, 1);
    glm::vec3 posWS_depth1_v3 = glm::vec3(posWS_depth1_v4) / posWS_depth1_v4.w;
    glm::vec3 lineDir = glm::normalize(posWS_depth1_v3 - m_position);
    float dist = 0;
    if (intersectRayPlane(m_position, lineDir, planeP, -lineDir, dist))
    {
        if (dist > 0.0f) { return m_position + dist * lineDir; }
    }
    return glm::vec3(0.0f);
}

//glm::vec2 Camera::worldToSS3d(glm::vec3 posWS)
//{
//    return glm::vec2(0); // todo
//}

glm::mat4 Camera::get2dMatrixScreenResolution() const
{
    const int& width = m_sizeWindow.x;
    const int& height = m_sizeWindow.y;
    return glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
}

void Camera::move(GameCameraState& gcs)
{
    if (!gcs.isCameraMoving) return;

    const int& width = m_sizeWindow.x;
    const int& height = m_sizeWindow.y;

    glm::vec3 viewDir = gcs.lookAtDirectionOld;

    if (m_is2d)
    {
        // 2d
        const float speed2d = 0.2f;

        // translate with keyboard
        m_position += (getRight() * gcs.moveRight * -1.0f + getUp() * gcs.moveFwd * -1.0f) *
            gcs.speedTranslation * gcs.speedFactorTranslate * gcs.speedTranslationFactor * gcs.dt * speed2d;

        // scale
        if (m_scale2d != gcs.scale2d)
        {
            m_scale2d = gcs.scale2d;
        }

        // translate with mouse
        glm::vec2 dxOffsWorld = glm::vec2(1.0f / (float)width, 1.0f / (float)width) * 2.0f; // viewport (-1..1,-aspectRatio..aspectRatio) ==> (width,width) & *2.0f

        glm::vec2 posSS(gcs.rotatePosSSactual.x, height-gcs.rotatePosSSactual.y);
        glm::vec2 posSS_dx(gcs.rotatePosSSactual.x - gcs.rotatePosSSold.x, - (gcs.rotatePosSSactual.y - gcs.rotatePosSSold.y));

        m_position = gcs.posOld+glm::vec3(posSS_dx * dxOffsWorld,0.0f);
        m_position += glm::vec3( (gcs.rotatePosWSold * gcs.scaleOld) - (gcs.rotatePosWSold * m_scale2d), 0.0f);
    }
    else
    {
        // 3d camera

        // rotate
        glm::vec2 dx = gcs.rotatePosSSactual - gcs.rotatePosSSold;

        glm::vec3 up = gcs.upOld;
        glm::vec3 right = glm::normalize(glm::cross(gcs.upOld, viewDir));
        
        glm::vec3 rotAxis = right * dx.y + up * dx.x * -1.0f;
        
        float angle = 0.0f;

        if ((fabs(dx.x) >= 2.0f) || (fabs(dx.y) >= 2.0f))
        {
            angle = glm::length(dx) * gcs.speedRotation;
            //std::cerr << angle << ":" << abs(glm::dot(up, getLookVector())) << "\n";

            // rotate view dir by angle
            viewDir = glm::normalize(glm::rotate(gcs.lookAtDirectionOld, angle, rotAxis));

            // determine up-vector
            m_up = glm::normalize(glm::rotate(gcs.upOld, angle, rotAxis));
            // stabilize up-vector
            glm::vec3 newRight = glm::normalize(glm::cross(viewDir, m_up));
            newRight.y = 0; // force right to be in xz-plane
            newRight = glm::normalize(newRight);
            m_up = glm::normalize(glm::cross(newRight, viewDir));

        }

        // translate
        m_position += (getFwd() * gcs.moveFwd + getRight() * gcs.moveRight + getUp() * gcs.moveUp) *
            gcs.speedTranslation * gcs.speedFactorTranslate * gcs.speedTranslationFactor * gcs.dt;
    }

    // determine new lookAt with same distance as before
    glm::vec3 newLookAt = getPos() + gcs.lookAtDistOld * viewDir;
    setLookAt(newLookAt);

    updateView(false);
}


void Camera::updateView(bool bSetViewport)
{
    m_view = glm::lookAt(m_position, m_lookAt, m_up);

    const int& width = m_sizeWindow.x;
    const int& height = m_sizeWindow.y;

    if (m_bMirror)
    {
        float floorY = -m_floorY;
        glm::mat4 Mscale = glm::mat4(glm::scale(glm::mat4(1), glm::vec3(1, -1, 1)));
        glm::mat4 Mtrans = glm::mat4(glm::translate(glm::mat4(1), glm::vec3(0, floorY, 0)));
        glm::mat4 MtransInv = glm::inverse(Mtrans);
        m_view = m_view * (MtransInv * Mscale * Mtrans); // mirror xz
    }

    float ratio = height>0 ? (float)width / (float)height : 1.0f;
    m_projection = glm::perspective(m_fov, ratio, m_near, m_far);

    if (m_is2d)
    {
        ratio = width > 0 ? (float)height / (float)width : 1.0f;
        m_view = glm::ortho<float>(-1.0f, 1.0f, -ratio, ratio, -10.0, 10.0f);
        glm::vec3 t(m_position.x, m_position.y, 0.0f);
        glm::mat4 trans = glm::translate(t);
        glm::mat4 scale = glm::scale(glm::vec3(m_scale2d));
        m_view = m_view * (trans * scale);
        m_projection = glm::mat4(1);
    }

    //if (m_bMirror) m_projection= glm::mat4(glm::scale(glm::mat4(1), glm::vec3(-1,1,1))) * m_projection; // mirror x
    if (bSetViewport) glViewport(0, 0, m_sizeFramebuffer.x, m_sizeFramebuffer.y);
}

void Camera::mirrorCameraOnFloor(bool mirror, float floorY)
{
    m_bMirror = mirror;
    m_floorY = floorY;
    /*
    glm::vec3 posCam = getTransform().getTranslation();
    glm::vec3 posOnFloor = glm::vec3(0,floorY,0);
    glm::vec3 diff = posCam-posOnFloor;
    glm::vec3 floorNormal(0,1,0);
    glm::vec3 n = glm::normalize( glm::vec3(floorNormal) );
    glm::vec3 posMirror = posCam - glm::dot(n,diff) * n * 2.0f;
    */
    //m_lookAt.y = floorY-m_lookAt.y;

    //getTransform().setTranslation( posMirror );
    //m_up*=-1;
}

void Camera::setLookAt(glm::vec3 newLookAt)
{
    m_lookAt = newLookAt;
}

glm::vec3 Camera::getLookAt() const
{
    return m_lookAt;
}

glm::mat4 const& Camera::getProjection() const
{
    return m_projection;
}

glm::mat4 const& Camera::getView() const
{
    return m_view;
}

glm::mat4 Camera::getViewProjection() const
{
    return getProjection() * getView();
}

glm::mat4 Camera::getViewProjectionInv() const
{
    return glm::inverse(getViewProjection());
}

glm::vec3 Camera::getFwd() const
{
    return glm::normalize(m_lookAt - m_position);
}

float Camera::getLookAtDist() const
{
    return glm::length(m_lookAt - m_position);
}

glm::vec3 const& Camera::getUp() const
{
    return m_up;
}

glm::vec3 Camera::getRight() const
{
    return glm::normalize(glm::cross(getFwd(), getUp()));
}

glm::vec3 const& Camera::getPos() const
{
    return m_position;
}

void Camera::determineSSBBfromWSBB(glm::vec3 aabb0, glm::vec3 aabb1, glm::vec3& ssMinX, glm::vec3& ssMinY, glm::vec3& ssMinZ, glm::vec3& ssMaxX, glm::vec3& ssMaxY, glm::vec3& ssMaxZ) const
{
    ssMinX = glm::vec3(9999999);
    ssMaxX = glm::vec3(-9999999);
    ssMinY = glm::vec3(9999999);
    ssMaxY = glm::vec3(-9999999);
    ssMinZ = glm::vec3(9999999);
    ssMaxZ = glm::vec3(-9999999);

    for (int z = 0; z < 2; z++) for (int y = 0; y < 2; y++) for (int x = 0; x < 2; x++)
    {
        glm::vec3 x0((float)x, (float)y, (float)z);
        glm::vec3 x1((float)(1 - x), (float)(1 - y), (float)(1 - z));
        glm::vec3 p = x0 * aabb0 + x1 * aabb1;
        glm::vec4 v4pSS = (this->getViewProjection() * glm::vec4(p, 1));
        glm::vec3 pSS = glm::vec3(v4pSS.x, v4pSS.y, v4pSS.z) / v4pSS.w;

        if (pSS.x < ssMinX.x) ssMinX = pSS;
        if (pSS.y < ssMinY.y) ssMinY = pSS;
        if (pSS.z < ssMinZ.z) ssMinZ = pSS;

        if (pSS.x > ssMaxX.x) ssMaxX = pSS;
        if (pSS.y > ssMaxY.y) ssMaxY = pSS;
        if (pSS.z > ssMaxZ.z) ssMaxZ = pSS;
    }
}

// project v1ToProj onto proj and determines angle between dir and the new vector
float _determineAngleProjected(glm::vec3 dir, glm::vec3 v1ToProj, glm::vec3 proj, glm::vec3 camPos)
{
    glm::vec3  vProj = glm::proj(v1ToProj - camPos, dir) + glm::proj(v1ToProj - camPos, proj);
    float result = glm::abs(glm::angle(dir, glm::normalize(vProj)));
    //std::cerr << "ANGLE: " << result << "\n";
    //VxDebugRenderSngl::get()->beginStaticDebug("FOVY");
    //VxDebugRenderSngl::get()->addSphere( camPos + vProj , 0.1);
    //VxDebugRenderSngl::get()->addSphere( v1ToProj , 0.03);
    //VxDebugRenderSngl::get()->addText( camPos + vProj, nullptr, "Angle: %f", result);
    //VxDebugRenderSngl::get()->addLine(camPos, camPos + vProj);
    return result;
}

void Camera::adaptFovYtoLookAtAABBFullScreen_NOT_POSITION(glm::vec3 aabb0, glm::vec3 aabb1)
{
    const int& width = m_sizeWindow.x;
    const int& height = m_sizeWindow.y;

    float epsPerc01 = 0.02f; //02f; // exact m_view is slightly extended [ i.e.: fovY * (1+epsPerc01) ]

    // needs some steps to converge, dont know why...
    // int itSteps = 3;
    for (int i = 0; i < 3; i++) {
        // find center in SS
        glm::vec3 ssMinX, ssMaxX, ssMinY, ssMaxY, ssMinZ, ssMaxZ;
        determineSSBBfromWSBB(aabb0, aabb1, ssMinX, ssMinY, ssMinZ, ssMaxX, ssMaxY, ssMaxZ);

        glm::vec3 centerSS((ssMaxX.x + ssMinX.x) * 0.5f, (ssMaxY.y + ssMinY.y) * 0.5f, (ssMaxZ.z + ssMinZ.z) * 0.5f);
        glm::vec4 v4center = glm::inverse(this->getViewProjection()) * glm::vec4(centerSS, 1);
        glm::vec3 centerWS = glm::vec3(v4center.x, v4center.y, v4center.z) / v4center.w;
        setLookAt(centerWS);
        updateView();
    }

    // change fov
    // now find maximum angle between viewDir & corner of boxes / permute through all 8 points
    glm::vec3 viewDir = glm::normalize(getFwd());
    glm::vec3 camPos = this->getPos();

    // PURELY WS
    float angleX = -9999;
    float angleY = -9999;
    for (int z = 0; z < 2; z++) for (int y = 0; y < 2; y++) for (int x = 0; x < 2; x++)
    {
        glm::vec3 x0((float)x, (float)y, (float)z);
        glm::vec3 x1((float)(1 - x), (float)(1 - y), (float)(1 - z));
        glm::vec3 p = x0 * aabb0 + x1 * aabb1;

        angleX = glm::max(angleX, _determineAngleProjected(viewDir, p, getRight(), camPos));
        glm::vec3 correctUp = glm::normalize(glm::cross(getRight(), viewDir));
        angleY = glm::max(angleY, _determineAngleProjected(viewDir, p, correctUp, camPos));
    }
    //VxDebugRenderSngl::get()->addBoxAABB(aabb0, aabb1);

    setFovY(angleY * 2 * (1.0f + epsPerc01));

    updateView();
    bool needFovX = false;

    // now check if x coords are inside frustrum/m_view / since we used FovY atm
    for (int z = 0; z < 2; z++) for (int y = 0; y < 2; y++) for (int x = 0; x < 2; x++)
    {
        glm::vec3 x0((float)x, (float)y, (float)z);
        glm::vec3 x1((float)(1 - x), (float)(1 - y), (float)(1 - z));
        glm::vec3 p = x0 * aabb0 + x1 * aabb1;

        glm::vec4 pSS4 = (this->getViewProjection() * glm::vec4(p, 1.0f));
        glm::vec3 pSS(pSS4.x, pSS4.y, pSS4.z);
        pSS *= (1.0f / pSS4.w);
        if (pSS.x > (1.0f)) needFovX = true;
        if (pSS.x < (-1.0f)) needFovX = true;
        //if ( pSS.y>(1.0f ) ) needFovX=true;
        //if ( pSS.y<(-1.0f ) ) needFovX=true;

        //std::cerr << "BOX CORNERS SS: " << glm::to_string(pSS) << "\n";
    }
    //std::cerr << "USE FovX: " << (needFovX?"true":"false") << "\n";
    //std::cerr << "-------\n";
    //std::cerr << "ANGLE X: " << angleX << "\n";

    //if (needFovX) setFovY( 2 * angleX / (width / (float)height) *(1.0f+epsPerc01) );
    if (needFovX)
    {
        float ang = tan(angleX);
        setFovY(2 * atan(ang / (width / (float)height) * (1.0f + epsPerc01)));
    }

}

void Camera::moveFwd(float dx)
{
    glm::vec3 dv = dx * getFwd();
    m_position += dv;
    m_lookAt += dv;
    updateView();
}

std::shared_ptr<Camera> Camera::createCamera2d(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow)
{
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(sizeFramebuffer, sizeWindow);
    camera->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    camera->setFovY(60.0f / 180.0f * 3.141f);
    camera->setLookAt(glm::vec3(0, 0.0f, -1.0f));
    camera->set2d();
    camera->updateView();
    return camera;
}

std::shared_ptr<Camera> Camera::createCamera3d(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow)
{
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(sizeFramebuffer, sizeWindow);
    camera->setPosition(glm::vec3(0.0f, 5.0f, 5.0f));
    camera->setFovY(60.0f / 180.0f * 3.141f);
    camera->setLookAt(glm::vec3(0, 0.0f, 0.0f));
    camera->updateView();
    return camera;
}

void Camera::drawOrientation() const
{
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(getRight().x, getRight().y, getRight().z);
    glColor3f(0, 1, 0);
    glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(getUp().x, getUp().y, getUp().z);
    glColor3f(0, 0, 1);
    glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(getFwd().x, getFwd().y, getFwd().z);
    glEnd();
    glLineWidth(1.0f);
    glColor3f(1, 1, 1);
}
