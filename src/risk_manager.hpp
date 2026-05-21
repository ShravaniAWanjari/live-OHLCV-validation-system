#pragma once
#include "validator.hpp"
#include <iostream>
#include <string>
#include <atomic>

enum class PipelineState{
    ACTIVE,
    HALTED
};

class RiskManager {
public:
    RiskManager(int max_non_critical_errors = 3)
        : state_(PipelineState::ACTIVE),
          non_critical_error_count_(0),
          max_non_critical_errors_(max_non_critical_errors){}
    
    bool is_active() const{
        return state_.load(std::memory_order_acquire) == PipelineState::ACTIVE;
    }

    void handle_validation_result(uint8_t validation_flags, const std::string& context = ""){
        if (validation_flags == 0) return;

        if (state_.load(std::memory_order_relaxed) == PipelineState::HALTED){
            return;
        }

        bool critical = Validator::is_critical(validation_flags);

        if (critical){
            trigger_halt("Critical validation failure(Flags: " + std::to_string(validation_flags) + ")" + context);
        } else {
            non_critical_error_count_++;
            std::cerr << "[RiskManager] Non-critical anomaly detected (" << context
                      << "). Count : " << non_critical_error_count_
                      << "/" << max_non_critical_errors_ << std::endl;
            
            if (non_critical_error_count_ >= max_non_critical_errors_){
                trigger_halt("Exceeded maximum non-critical validation errors limit.");
            }
        }
    }

    void reset(){
        state_.store(PipelineState::ACTIVE, std::memory_order_release);
        non_critical_error_count_ = 0;
    }

private:
    void trigger_halt(const std::string& reason){
        state_.store(PipelineState::HALTED, std::memory_order_release);
        
        std::cerr << "\n################################################################" << std::endl;
        std::cerr << "!!! RISK MANAGER CRITICAL ALERT : SYSTEM HALTING !!!" << std::endl;
        std::cerr << "Reason: " << reason << std::endl;
        std::cerr << "ACTION: PULLING ALL ACTIVE ORDERS FROM EXCHANGE IMMEDIATELY!" << std::endl;
        std::cerr << "STATUS: DOWNSTREAM TRADING STRATEGY DISCONNECTED." << std::endl;
        std::cerr << "################################################################\n" << std::endl;
    }

    std::atomic<PipelineState> state_;
    int non_critical_error_count_;
    int max_non_critical_errors_;
};
