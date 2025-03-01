// agent.cpp
#include "agent.h"
#include <utility>

namespace ai_framework {

Agent::Agent(so_5::environment_t& env, std::string id)
    : so_5::agent_t(env), m_id(std::move(id)) {
}

std::string Agent::GetId() const {
    return m_id;
}

void Agent::so_define_agent() {
    // Base implementation - does nothing by default
}

void Agent::so_evt_start() {
    // Base implementation - does nothing by default
}

void Agent::so_evt_finish() {
    // Base implementation - does nothing by default
}

} // namespace ai_framework