/*
 * =============================================================================
 * God Modded: Validation
 * Composable validation rules for struct fields
 * =============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>

struct PaymentInput {
    std::string currency;
    int amount_cents;
    std::string merchant;
};

using Rule = std::function<std::string(const PaymentInput&)>;

std::vector<std::string> validate(const PaymentInput& p, const std::vector<Rule>& rules) {
    std::vector<std::string> errors;
    for (const auto& r : rules) {
        std::string e = r(p); if (!e.empty()) errors.push_back(e);
    }
    return errors;
}

void demo_validation() {
    std::cout << "\n=== GOD MODDED: VALIDATION ===" << std::endl;
    PaymentInput p{"USD", 5000, "MERCHANT_1"};
    std::vector<Rule> rules{
        [](const PaymentInput& x){ return x.currency.size()==3? std::string{} : std::string{"currency must be 3 chars"}; },
        [](const PaymentInput& x){ return x.amount_cents>0? std::string{} : std::string{"amount must be positive"}; },
        [](const PaymentInput& x){ return x.merchant.empty()? std::string{"merchant required"} : std::string{}; }
    };
    auto errs = validate(p, rules);
    if (errs.empty()) std::cout << "valid" << std::endl; else for (auto& e: errs) std::cout << e << std::endl;
}

int main() {
    try { demo_validation(); std::cout << "\n=== VALIDATION COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
