#pragma once

#include "../Misc/Globals/GIncludes.hpp"

enum class Currency { BIT, MALBIT };
inline Currency currentLogicMode = Currency::BIT;

struct Logic {
    std::string name;
    std::string desc;
    long double baseCost;
    long double currentCost;
    long double bps;
    int ver = 0;
    Currency costType;
    sf::RectangleShape rect;

    Logic(std::string n, std::string d, long double bc, long double bps, Currency type = Currency::BIT)
        : name(n), desc(d), baseCost(bc), currentCost(bc), bps(bps), costType(type) {
        rect.setSize({ 160.f, 90.f });
        rect.setFillColor(sf::Color::Black);
        rect.setOutlineColor(sf::Color(50, 50, 50));
        rect.setOutlineThickness(1.f);
    }
};

inline std::vector<Logic> logicGateList;
inline const float lgWidth = 350.f;
inline const float lgHeight = 70.f;
inline const float lgSpacing = 10.f;

inline void initLogicGates() {
    // 1.0.0 Standard
    logicGateList.emplace_back("not_gate", "Inverts existence into non-existence.", 1e1L, 1e0L, Currency::BIT);
    logicGateList.emplace_back("bit_flipper", "Forces ones into zeros.", 6e1L, 4e0L, Currency::BIT);
    logicGateList.emplace_back("shift_register", "Pushes data into the abyss.", 2.5e2L, 1.5e1L, Currency::BIT);
    logicGateList.emplace_back("null_comparator", "Validates the lack of data.", 1.2e3L, 6.5e1L, Currency::BIT);
    logicGateList.emplace_back("bus_sniffer", "Captures and deletes stray signals.", 5e3L, 2.4e2L, Currency::BIT);
    logicGateList.emplace_back("stack_siphon", "Drains the system's memory reserves.", 2e4L, 9e2L, Currency::BIT);
    logicGateList.emplace_back("parity_checker", "Corrects reality until it's empty.", 8.5e4L, 3.5e3L, Currency::BIT);
    // 1.0.2 Standard
    logicGateList.emplace_back("instruction_bleeder", "Slowly leaks the CPU's logic.", 3.5e5L, 1.3e4L, Currency::BIT);
    logicGateList.emplace_back("heap_corruptor", "Destroys the structure of bits.", 1.2e6L, 4.2e4L, Currency::BIT);
    logicGateList.emplace_back("packet_dropper", "Ensures nothing ever arrives.", 5e6L, 6e4L, Currency::BIT);
    logicGateList.emplace_back("buffer_overflow", "Spills data into unallocated voids.", 1.5e7L, 4.5e5L, Currency::BIT);
    logicGateList.emplace_back("logic_bomb", "A timed explosion of nothingness.", 4e7L, 1.1e6L, Currency::BIT);
    logicGateList.emplace_back("hardware_interrupt", "Halts the flow of reality.", 1e8L, 2.6e6L, Currency::BIT);
    // 1.0.7 Standard
    logicGateList.emplace_back("deadlock_thread", "Infinite waiting for zero output.", 2.2e8L, 5.5e6L, Currency::BIT);
    logicGateList.emplace_back("kernel_overload", "Total system failure for profit.", 4.5e8L, 1.05e7L, Currency::BIT);
    logicGateList.emplace_back("write_blocker", "Prevents any new data creation.", 1e9L, 2.2e7L, Currency::BIT);
    logicGateList.emplace_back("entropy_driver", "Accelerates the decay of code.", 2.5e9L, 5e7L, Currency::BIT);
    logicGateList.emplace_back("negative_bus", "Transports bits that shouldn't exist.", 6e9L, 1.15e8L, Currency::BIT);
    logicGateList.emplace_back("ghost_process", "Executes without consuming resources.", 1.5e10L, 2.75e8L, Currency::BIT);
    logicGateList.emplace_back("root_nullifier", "The final line of code... for now.", 5e10L, 8.5e8L, Currency::BIT);
    // 1.1.0 Standard
    logicGateList.emplace_back("v_memory_swap", "Swapping reality for a virtual void.", 1.25e11L, 2.1e9L, Currency::BIT);
    logicGateList.emplace_back("cold_boot_wipe", "Data loss through thermal reset.", 3e11L, 5.5e9L, Currency::BIT);
    logicGateList.emplace_back("cache_poison", "Contaminating the logic pipeline.", 7.5e11L, 1.4e10L, Currency::BIT);
    logicGateList.emplace_back("dma_hijack", "Direct memory access to the abyss.", 1.8e12L, 3.8e10L, Currency::BIT);
    logicGateList.emplace_back("bit_rot_driver", "Natural decay of digital certainty.", 4.5e12L, 9.5e10L, Currency::BIT);
    logicGateList.emplace_back("zombie_process", "A thread that refuses to exist or die.", 1e13L, 2.2e11L, Currency::BIT);
    logicGateList.emplace_back("ring_zero_nuke", "Complete privilege escalation to nothing.", 2.5e13L, 5.8e11L, Currency::BIT);
    logicGateList.emplace_back("microcode_void", "The processor's sub-logic is missing.", 6.5e13L, 1.6e12L, Currency::BIT);
    logicGateList.emplace_back("clock_pulse_kill", "Desynchronizing the heartbeat of data.", 1.5e14L, 4.2e12L, Currency::BIT);
    logicGateList.emplace_back("system_halt_ex", "Total cessation of all digital activity.", 4e14L, 1.1e13L, Currency::BIT);
    // 1.1.0 Malicious
    logicGateList.emplace_back("overflow_exploit", "Weaponizing the excess bits.", 1e1L, 5e10L, Currency::MALBIT);
    logicGateList.emplace_back("backdoor_gate", "Secret entry to the bitstream.", 6e1L, 1.25e11L, Currency::MALBIT);
    logicGateList.emplace_back("rootkit_process", "Invisible theft of system logic.", 2.5e2L, 3e11L, Currency::MALBIT);
    logicGateList.emplace_back("logic_shredder", "Violently tearing code from the heap.", 1.2e3L, 7.5e11L, Currency::MALBIT);
    logicGateList.emplace_back("zero_day_tunnel", "A hole in reality with no patch.", 5e3L, 1.8e12L, Currency::MALBIT);
    logicGateList.emplace_back("botnet_node", "Enlisting ghost cycles for the cause.", 2e4L, 4.5e12L, Currency::MALBIT);
    logicGateList.emplace_back("sql_injector", "Feeding the database toxic commands.", 8.5e4L, 1e13L, Currency::MALBIT);
    logicGateList.emplace_back("buffer_poison", "Corrupting the logic from the inside out.", 3.5e5L, 2.5e13L, Currency::MALBIT);
    logicGateList.emplace_back("payload_dropper", "Delivering the final logic-killer.", 1.2e6L, 6.5e13L, Currency::MALBIT);
    logicGateList.emplace_back("trojan_host", "A gift that deletes reality upon opening.", 5e6L, 1.5e14L, Currency::MALBIT);
    // 1.1.4 Malicious
    //logicGateList.emplace_back("logic_ransom", "Pay in bits, or lose the system.", x, x, Currency::MALBIT);
    //logicGateList.emplace_back("kernel_stalker", "Watching the CPU from the dark.", x, x, Currency::MALBIT);
    //logicGateList.emplace_back("shadow_exploit", "An attack that leaves no system logs.", x, x, Currency::MALBIT);
    //logicGateList.emplace_back("zero_point_hack", "Exploiting the gap between 0 and 1.", x, x, Currency::MALBIT);
    //logicGateList.emplace_back("os_executioner", "Terminating the parent process of reality.", x, x, Currency::MALBIT);
}

inline void updateLogicGateUI(sf::RenderWindow& window, long double allBits) {
    const float margin = 40.f;
    float startX = window.getSize().x - lgWidth - margin;
    float startY = 105.f;

    int visibleCount = 0;
    for (size_t i = 0; i < logicGateList.size(); ++i) {
        if (logicGateList[i].ver > 0 || allBits >= logicGateList[i].baseCost) {
            float yPos = startY + (static_cast<float>(visibleCount) * (lgHeight + lgSpacing));
            logicGateList[i].rect.setPosition({ startX, yPos });
            visibleCount++;
        }
    }
}

inline void to_json(json& j, const Logic& lg) {
    j = json{ {"n", lg.name}, {"v", lg.ver} };
}

inline void from_json(const json& j, Logic& lg) {
    j.at("n").get_to(lg.name);
    j.at("v").get_to(lg.ver);
}