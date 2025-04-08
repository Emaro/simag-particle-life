#include "TaskLect03_Billiard.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Helper/HelperDraw.h"

// ----------------------------------------------------------------------------

namespace
{
    // Helper: Random number from -1 to 1
    float rand_m11() { return (float)rand() / (float)RAND_MAX * 2.0f - 1.0f; }
    // Helper: Random number from 0 to 1
    float rand_01() { return (float)rand() / (float)RAND_MAX; }
}

void TaskLect03_Billiard::addBall(glm::vec3 const& pos, glm::vec3 const& vel, float radius, float mass)
{
    particleSystem(0).add(pos);
    m_balls.emplace_back(Ball(particleSystem(0), (int)particleSystem(0).size()-1, pos, vel, radius, mass));
}

void TaskLect03_Billiard::createScene()
{
    particleSystem(0).clear();
    m_balls.clear();

    const float r = 0.49f;
    const float mass = 1.0f;
    const glm::vec3 vel0(0, 0, 0);
    const glm::vec3 velStart(-8.0f, 0.0f, 0.0f);

    // Create simple billiard scene:
    // 6 balls in a triangle on the left (velocities=[vel0])
    // 1 ball on the right (velocities=[velStart])
    // Example to add a ball:
    // addBall(glm::vec3(0, 0, 0), vel0, r, mass);

    addBall(glm::vec3(3, 0, 0), velStart, r, mass);

    addBall(glm::vec3(-1, 0, 0), vel0, r, mass);
    addBall(glm::vec3(-2, .5, 0), vel0, r, mass);
    addBall(glm::vec3(-2, -.5, 0), vel0, r, mass);
    addBall(glm::vec3(-3, 0, 0), vel0, r, mass);
    addBall(glm::vec3(-3, 1, 0), vel0, r, mass);
    addBall(glm::vec3(-3, -1, 0), vel0, r, mass);
}

void TaskLect03_Billiard::createScene2()
{
    // todo students
}

void TaskLect03_Billiard::setRandomVelocities()
{
    float maxSpeed = 10.0f;

    // Set random velocities for all balls
    // See helper methods above:
    // rand_m11() => return random number from -1..1
    // rand_01()  => return random number from 0..1
    float r1;
    float r2;

    for (auto &b : m_balls)
    {
        do {
            r1 = rand_m11();
            r2 = rand_m11();
        } while (r1*r1 + r2*r2 > 1);

        b.vel() = glm::vec3(r1, r2, 0) * maxSpeed;
    }
}

void TaskLect03_Billiard::setRandomRadius()
{
    float maxRadius = 0.8f;
    float minRadius = 0.2f;

    for (auto &b : m_balls)
        b.radius() = (rand_01() * (maxRadius - minRadius)) + minRadius;
}

void TaskLect03_Billiard::handleBallBallCollision(int i, int j)
{
    float restitution = m_restitution;

    auto& ball1 = m_balls[i];
    auto& ball2 = m_balls[j];

    auto r2 = ball1.radius() + ball2.radius();
    auto pdiff = ball1.pos() - ball2.pos();

    if (r2 > glm::length(pdiff)) {
        auto d2 = (r2 - glm::length(pdiff)) / 2;
        auto dn = glm::normalize(pdiff);

        ball1.pos() += dn * d2;
        ball2.pos() -= dn * d2;

        auto pj1 = dn * glm::dot(ball1.vel(), dn);
        auto pj2 = dn * glm::dot(ball2.vel(), dn);

        auto m2 = ball1.mass() + ball2.mass();
        auto md = ball1.mass() - ball2.mass();

        // Preserve momentum / energy
        auto v1 = pj1 * md / m2
            + pj2 * (2 * ball2.mass()) / m2;

        auto v2 = pj1 * (2 * ball1.mass()) / m2
            + pj2 * -md / m2;

        ball1.vel() -= pj1 - v1;
        ball2.vel() -= pj2 - v2;
    }
}

void TaskLect03_Billiard::handleBallWallCollision(int i)
{
    if (i > (int)m_balls.size()) return;

    Ball& ball = m_balls[i];
    glm::vec3& p = ball.pos();
    glm::vec3& v = ball.vel();
    float r = ball.radius();
    
    glm::vec3 const& wallMin = m_wallMin;
    glm::vec3 const& wallMax = m_wallMax;

    if (p.x + r > wallMax.x)
    {
        p.x = wallMax.x - r;
        v.x = -v.x;
    }

    if (p.y + r > wallMax.y)
    {
        p.y = wallMax.y - r;
        v.y = -v.y;
    }

    if (p.x - r < wallMin.x)
    {
        p.x = wallMin.x + r;
        v.x = -v.x;
    }

    if (p.y - r < wallMin.y)
    {
        p.y = wallMin.y + r;
        v.y = -v.y;
    }
}

// ----------------------------------------------------------------------------

void TaskLect03_Billiard::setForces()
{
    int nBalls = (int)particleSystem(0).size();
    if (nBalls != (int)m_balls.size()) return;

    // Ball-Ball Collision
    if (nBalls > 1)
    {
        for (int i = 0; i < nBalls - 1; i++)
        {
            for (int j = i+1; j < nBalls; j++)
            {

                handleBallBallCollision(i, j);
                
            }
        }
    }

    // Ball-Wall Collision
    for (int i = 0; i < nBalls; i++)
    {
        handleBallWallCollision(i);
    }

    // Ball-Line Collision
    if (useLineSegments) handleBallLineCollision();

    // Simple Damping by reducing velocity per frame
    auto& vel = particleSystem(0).velocities();
    for (int i = 0; i < nBalls; i++)
    {
        vel[i] *= m_friction;
    }

}

void TaskLect03_Billiard::doWork() 
{

}

void TaskLect03_Billiard::draw() const
{
    int nBalls = (int)particleSystem(0).size();
    if (nBalls != (int)m_balls.size()) return;

    // Draw Balls
    glColor3f(1, 1, 1);
    glLineWidth(3.0f);
    for (int i = 0; i < nBalls; i++)
    {
        Helper::drawCircle2d(particleSystem(0).positions()[i], m_balls[i].radius(), 20);
    }
    glLineWidth(1.0f);

    // Draw Wall
    glColor3f(0, 1, 0);
    glLineWidth(3.0f);
    Helper::drawBox2d(m_wallMin.x, m_wallMin.y, m_wallMax.x, m_wallMax.y);
    glLineWidth(1.0f);
    glColor3f(1, 1, 1);

    // Draw Line-Segments
    if (useLineSegments) drawLineSegments();
}

void TaskLect03_Billiard::imGui()
{
    if (ImGui::Button("Create Scene##Billiard")) { createScene(); }
    if (ImGui::Button("Create Scene 2##Billiard")) { createScene2(); }
    if (ImGui::Button("Set Random Velocities##Billiard")) { setRandomVelocities(); }
    if (ImGui::Button("Set Random Radius##Billiard")) { setRandomRadius(); }
    ImGui::SliderFloat("Restitution##Billiard", &m_restitution, 0.1f, 1.0f);
    ImGui::SliderFloat("Friction##Billiard", &m_friction, 0.99f, 1.0f);

    if (useLineSegments)
    {
        ImGui::Separator();
        if (ImGui::Button("Clear Lines##Billiard")) { clearLines(); }
        if (ImGui::Button("Create Lines0##Billiard")) { createLines0(); }
        if (ImGui::Button("Create Lines1##Billiard")) { createLines1(); }
        if (ImGui::Button("Create Lines2##Billiard")) { createLines2(); }

        if (ImGui::Button("Copy Lines to PS1##Billiard")) { copyLinesToPS1(); }
        if (ImGui::Button("Copy Lines from PS1##Billiard")) { copyLinesFromPS1(); }
    }
}

const char* TaskLect03_Billiard::toString() const
{
    std::stringstream ss;
    ss << "Billiard simulation in 2D\n";
    ss << "Using conservation of momentum\n";
    m_string = ss.str();
    return m_string.c_str();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void TaskLect03_Billiard::copyLinesToPS1()
{
    auto& ps = particleSystem(1);
    ps.clear();
    for (auto const& it : m_lines)
    {
        ps.add(it.l0());
        ps.add(it.l1());
    }
}

void TaskLect03_Billiard::copyLinesFromPS1()
{
    auto& ps = particleSystem(1);
    if (ps.size() != m_lines.size() * 2) return;
    int cnt = 0;
    for (auto& it : m_lines)
    {
        it.setLine(ps.positions()[cnt], ps.positions()[cnt + 1]);
        ps.add(it.l1());
        cnt += 2;
    }
}

void TaskLect03_Billiard::drawLineSegments() const
{
    glColor3f(1, 1, 0);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
    for (auto const& it : m_lines)
    {
        glVertex3fv(glm::value_ptr(it.l0()));
        glVertex3fv(glm::value_ptr(it.l1()));
    }
    glEnd();
    glLineWidth(1.0f);
}

void TaskLect03_Billiard::createLines0()
{
    Line l(glm::vec3(-5, -3, 0), glm::vec3(5, 0, 0));
    m_lines.push_back(l);
}

void TaskLect03_Billiard::createLines1()
{
    float bottom = -2;
    glm::vec3 pPrev(-5, bottom, 0);
    for (int i = -4; i <= 5; i++)
    {
        glm::vec3 pNext((float)i, bottom + rand_m11(), 0);
        Line l(pPrev, pNext);
        m_lines.push_back(l);
        pPrev = pNext;
    }
}

void TaskLect03_Billiard::createLines2()
{
    float pi = glm::pi<float>();
    float bottom = -2;
    glm::vec3 pPrev(-5, bottom, 0);
    for (float i = -5; i <= 5; i += 0.2)
    {
        // really? *exactly* like shown?
        auto k = ((i + 5) * 4 * pi / 10 + pi / 2);
        glm::vec3 pNext(i, bottom + (glm::sin(k) - 1) / 2, 0);
        Line l(pPrev, pNext);
        m_lines.push_back(l);
        pPrev = pNext;
    }
}

void TaskLect03_Billiard::handleBallLineCollision()
{
    int nBalls = (int)particleSystem(0).size();
    if (nBalls != (int)m_balls.size()) return;

    for (int i = 0; i < nBalls; i++)
    {
        for (auto const& l : m_lines)
        {
            handleBallLineCollision(i, l);
        }
    }
}

void TaskLect03_Billiard::handleBallLineCollision(int i, Line const& line)
{
    if (i > (int)m_balls.size()) return;

    Ball& ball = m_balls[i];
    glm::vec3& p = ball.pos();
    glm::vec3& v = ball.vel();
    float r = ball.radius();

    glm::vec3 pc = p - line.center();

    if (!line.isPointWithinLineRange(p)) return;

    glm::vec3 rp = glm::rotateZ(pc, -line.rotation()); // rotate vector v by
    auto rv = glm::rotateZ(v, -line.rotation());
    
    if (rp.y - r < 0)
    {
        rp.y = r;
        rv.y = -rv.y;
    }

    p = line.center() + glm::rotateZ(rp, line.rotation());
    v = glm::rotateZ(rv, line.rotation());

}
