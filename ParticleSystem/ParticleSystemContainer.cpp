#include "ParticleSystemContainer.h"

#include <sstream>

ParticleSystemContainer::ParticleSystemContainer()
{
    m_psStates.resize(cMaxNoOfPS);

    // Disable 'simulate' for all PS except PS0
    if (cMaxNoOfPS > 1)
    {
        for (int i = 1; i < cMaxNoOfPS; i++)
        {
            m_psStates[i].setSimulate(false);
        }
    }
}

void ParticleSystemContainer::update()
{
    for (auto& it : m_psStates)
    {
        it.update();
    }
}

void ParticleSystemContainer::setAllForcesToZero()
{
    for (auto& it : m_psStates)
    {
        if (it.simulate())
        {
            it.particleSystem().setAllForcesToZero();
        }
    }
}

void ParticleSystemContainer::setAllForcesToZeroForStaticParticles()
{
    for (auto& it : m_psStates)
    {
        if (it.simulate())
        {
            it.particleSystem().setAllForcesToZeroForStaticParticles();
        }
    }

}

void ParticleSystemContainer::step(int passNumber, float dt)
{
    for (auto& it : m_psStates)
    {
        if (it.simulate())
        {
            it.particleSystem().step(passNumber, dt);
        }
    }
}


void ParticleSystemContainer::draw() const
{
    for (auto const& it : m_psStates)
    {
        it.drawPsWithCurrentStyle();
    }
}

void ParticleSystemContainer::imGui()
{
    for (int i = 0; i < cMaxNoOfPS; i++)
    {
        const std::string pre = "PS" + std::to_string(i) + ": ";
        m_psStates[i].imGui(pre);
    }
}

std::string ParticleSystemContainer::toString() const
{
    std::stringstream ss;
    ss << "Max PS: " << cMaxNoOfPS << "\n";
	return ss.str();
}

