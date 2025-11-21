/*
 * Threat Modeling Patterns
 *
 * Source: Microsoft STRIDE, PASTA, OCTAVE, MITRE ATT&CK
 * Algorithm: Systematic threat identification, analysis, and mitigation
 *
 * What Makes It Ingenious:
 * - Structured threat enumeration (STRIDE)
 * - Attack tree modeling with probabilities
 * - Risk scoring and prioritization
 * - Countermeasure effectiveness analysis
 * - Threat intelligence integration
 * - Automated threat modeling
 *
 * When to Use:
 * - System design and architecture reviews
 * - Security requirements gathering
 * - Risk assessment and compliance
 * - Vulnerability management
 * - Incident response planning
 *
 * Real-World Usage:
 * - Microsoft Security Development Lifecycle (SDL)
 * - OWASP Threat Modeling
 * - NIST Cybersecurity Framework
 * - ISO 27001 risk assessments
 * - Financial institution security assessments
 * - Government system accreditation (FedRAMP)
 *
 * Time Complexity: O(n*m) for threat enumeration, O(t log t) for risk prioritization
 * Space Complexity: O(t + c + m) for threats, countermeasures, and mitigations
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <algorithm>
#include <queue>
#include <stack>
#include <set>
#include <cmath>
#include <chrono>
#include <random>

// Forward declarations
class ThreatModel;
class AttackTree;
class RiskAssessment;
class Countermeasure;
class ThreatIntelligence;

// STRIDE threat categories
enum class STRIDE_Category {
    SPOOFING,           // Impersonation of users or systems
    TAMPERING,          // Unauthorized modification of data
    REPUDIATION,        // Denying actions or transactions
    INFORMATION_DISCLOSURE,  // Exposure of sensitive information
    DENIAL_OF_SERVICE,  // Making system unavailable
    ELEVATION_OF_PRIVILEGE    // Gaining unauthorized access
};

// Threat severity levels
enum class ThreatSeverity {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

// Threat likelihood levels
enum class ThreatLikelihood {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    VERY_HIGH = 4
};

// Risk levels
enum class RiskLevel {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

// Asset types
enum class AssetType {
    DATA,
    PROCESS,
    EXTERNAL_ENTITY,
    TRUST_BOUNDARY,
    DATA_FLOW
};

// Threat actor types
enum class ThreatActor {
    SCRIPT_KIDDIE,
    CYBERCRIMINAL,
    INSIDER_THREAT,
    APT_GROUP,
    NATION_STATE
};

// Threat definition
struct Threat {
    std::string id;
    std::string name;
    std::string description;
    STRIDE_Category category;
    ThreatSeverity severity;
    ThreatLikelihood likelihood;
    ThreatActor actor;
    std::string affected_asset;
    std::string attack_vector;
    std::vector<std::string> prerequisites;
    std::vector<std::string> consequences;
    std::vector<std::string> mitigation_references;

    // Risk score calculation
    double risk_score() const {
        return static_cast<double>(severity) * static_cast<double>(likelihood);
    }

    RiskLevel risk_level() const {
        double score = risk_score();
        if (score >= 12) return RiskLevel::CRITICAL;
        if (score >= 8) return RiskLevel::HIGH;
        if (score >= 4) return RiskLevel::MEDIUM;
        return RiskLevel::LOW;
    }
};

// Countermeasure definition
struct Countermeasure {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> addressed_threats;
    double effectiveness;  // 0.0 to 1.0
    double cost;          // Implementation cost factor
    std::string implementation_complexity;
    std::vector<std::string> dependencies;

    // Cost-effectiveness ratio
    double cost_effectiveness() const {
        return effectiveness / cost;
    }
};

// System asset
struct Asset {
    std::string id;
    std::string name;
    std::string description;
    AssetType type;
    double value;  // Business value/criticality (1-10)
    std::vector<std::string> data_classification;
    std::vector<std::string> security_requirements;
};

// Data flow
struct DataFlow {
    std::string id;
    std::string name;
    std::string source_asset;
    std::string destination_asset;
    std::string data_type;
    std::vector<std::string> protocols;
    bool encrypted = false;
    std::vector<std::string> trust_boundaries;
};

// Attack tree node
struct AttackTreeNode {
    std::string id;
    std::string description;
    bool is_leaf = false;
    double probability = 0.0;  // Probability of success (0.0-1.0)
    std::vector<std::string> children;  // Child node IDs
    std::vector<std::string> countermeasures;  // Applicable countermeasures

    // Calculate success probability
    double success_probability(const std::unordered_map<std::string, AttackTreeNode>& all_nodes) const {
        if (is_leaf) return probability;

        // Assume AND relationship for children (all must succeed)
        double prob = 1.0;
        for (const auto& child_id : children) {
            auto it = all_nodes.find(child_id);
            if (it != all_nodes.end()) {
                prob *= it->second.success_probability(all_nodes);
            }
        }
        return prob;
    }
};

// STRIDE threat modeling
class STRIDE_Model {
public:
    STRIDE_Model() = default;

    // Add system elements
    void add_asset(const Asset& asset) {
        assets_[asset.id] = asset;
    }

    void add_data_flow(const DataFlow& flow) {
        data_flows_[flow.id] = flow;
    }

    // Generate threats using STRIDE
    std::vector<Threat> generate_threats() {
        std::vector<Threat> threats;

        // Analyze each asset for STRIDE threats
        for (const auto& [asset_id, asset] : assets_) {
            auto asset_threats = analyze_asset(asset);
            threats.insert(threats.end(), asset_threats.begin(), asset_threats.end());
        }

        // Analyze data flows
        for (const auto& [flow_id, flow] : data_flows_) {
            auto flow_threats = analyze_data_flow(flow);
            threats.insert(threats.end(), flow_threats.begin(), flow_threats.end());
        }

        return threats;
    }

private:
    std::vector<Threat> analyze_asset(const Asset& asset) {
        std::vector<Threat> threats;

        // Spoofing threats
        if (asset.type == AssetType::EXTERNAL_ENTITY) {
            threats.push_back({
                "spoof_" + asset.id,
                "User/Account Spoofing",
                "Attacker impersonates legitimate user or system",
                STRIDE_Category::SPOOFING,
                ThreatSeverity::HIGH,
                ThreatLikelihood::MEDIUM,
                ThreatActor::CYBERCRIMINAL,
                asset.id,
                "Authentication bypass",
                {"Weak authentication", "No MFA"},
                {"Unauthorized access", "Data breach"},
                {"Implement MFA", "Use strong auth"}
            });
        }

        // Tampering threats
        if (asset.type == AssetType::DATA) {
            threats.push_back({
                "tamper_" + asset.id,
                "Data Tampering",
                "Attacker modifies data in transit or at rest",
                STRIDE_Category::TAMPERING,
                ThreatSeverity::HIGH,
                ThreatLikelihood::MEDIUM,
                ThreatActor::CYBERCRIMINAL,
                asset.id,
                "Man-in-the-middle attack",
                {"Unencrypted communication"},
                {"Data corruption", "Wrong decisions based on bad data"},
                {"Use TLS", "Implement integrity checks"}
            });
        }

        // Repudiation threats
        if (asset.type == AssetType::PROCESS) {
            threats.push_back({
                "repud_" + asset.id,
                "Action Repudiation",
                "User denies performing an action",
                STRIDE_Category::REPUDIATION,
                ThreatSeverity::MEDIUM,
                ThreatLikelihood::LOW,
                ThreatActor::INSIDER_THREAT,
                asset.id,
                "Insufficient logging",
                {"No audit trails"},
                {"Cannot prove actions", "Legal issues"},
                {"Implement comprehensive logging", "Digital signatures"}
            });
        }

        // Information disclosure
        if (asset.value >= 7) {  // High-value assets
            threats.push_back({
                "disclose_" + asset.id,
                "Information Disclosure",
                "Sensitive information exposed to unauthorized parties",
                STRIDE_Category::INFORMATION_DISCLOSURE,
                ThreatSeverity::CRITICAL,
                ThreatLikelihood::MEDIUM,
                ThreatActor::APT_GROUP,
                asset.id,
                "Data leakage through various channels",
                {"Weak access controls", "Unencrypted storage"},
                {"Privacy violation", "Regulatory fines", "Brand damage"},
                {"Encrypt sensitive data", "Implement access controls"}
            });
        }

        // Denial of Service
        if (asset.type == AssetType::PROCESS) {
            threats.push_back({
                "dos_" + asset.id,
                "Denial of Service",
                "System becomes unavailable to legitimate users",
                STRIDE_Category::DENIAL_OF_SERVICE,
                ThreatSeverity::HIGH,
                ThreatLikelihood::HIGH,
                ThreatActor::SCRIPT_KIDDIE,
                asset.id,
                "Resource exhaustion attacks",
                {"No rate limiting", "Single point of failure"},
                {"Service disruption", "Financial loss"},
                {"Implement rate limiting", "Redundancy", "Load balancing"}
            });
        }

        // Elevation of Privilege
        threats.push_back({
            "elevate_" + asset.id,
            "Elevation of Privilege",
            "Attacker gains higher privileges than authorized",
            STRIDE_Category::ELEVATION_OF_PRIVILEGE,
            ThreatSeverity::CRITICAL,
            ThreatLikelihood::LOW,
            ThreatActor::INSIDER_THREAT,
            asset.id,
            "Privilege escalation exploits",
            {"Weak separation of privileges", "Buffer overflows"},
            {"Complete system compromise", "Data destruction"},
            {"Principle of least privilege", "Input validation", "Regular patching"}
        });

        return threats;
    }

    std::vector<Threat> analyze_data_flow(const DataFlow& flow) {
        std::vector<Threat> threats;

        // Check for unencrypted data flows
        if (!flow.encrypted) {
            threats.push_back({
                "intercept_" + flow.id,
                "Data Interception",
                "Attacker intercepts unencrypted data in transit",
                STRIDE_Category::INFORMATION_DISCLOSURE,
                ThreatSeverity::HIGH,
                ThreatLikelihood::HIGH,
                ThreatActor::CYBERCRIMINAL,
                flow.id,
                "Network sniffing, man-in-the-middle",
                {"Unencrypted communication"},
                {"Data exposure", "Session hijacking"},
                {"Implement TLS/SSL", "Use VPN"}
            });
        }

        // Check trust boundaries
        if (!flow.trust_boundaries.empty()) {
            threats.push_back({
                "boundary_" + flow.id,
                "Trust Boundary Violation",
                "Data crosses security boundaries without validation",
                STRIDE_Category::ELEVATION_OF_PRIVILEGE,
                ThreatSeverity::HIGH,
                ThreatLikelihood::MEDIUM,
                ThreatActor::INSIDER_THREAT,
                flow.id,
                "Bypassing security controls",
                {"Weak boundary controls"},
                {"Unauthorized access to sensitive areas"},
                {"Implement boundary validation", "Access controls"}
            });
        }

        return threats;
    }

    std::unordered_map<std::string, Asset> assets_;
    std::unordered_map<std::string, DataFlow> data_flows_;
};

// Attack tree modeling
class AttackTree {
public:
    AttackTree(const std::string& root_goal) : root_goal_(root_goal) {}

    void add_node(const AttackTreeNode& node) {
        nodes_[node.id] = node;
    }

    // Calculate attack success probability
    double calculate_success_probability() {
        if (nodes_.empty()) return 0.0;

        // Find root node (assuming it's the first added or has specific ID)
        for (const auto& [id, node] : nodes_) {
            if (node.children.empty() || node.id == "root") {
                return node.success_probability(nodes_);
            }
        }

        return 0.0;
    }

    // Find most vulnerable paths
    std::vector<std::vector<std::string>> find_vulnerable_paths() {
        std::vector<std::vector<std::string>> paths;

        // Simplified: return all leaf-to-root paths
        std::function<void(const std::string&, std::vector<std::string>&)> dfs =
            [&](const std::string& node_id, std::vector<std::string>& path) {
                path.push_back(node_id);

                auto it = nodes_.find(node_id);
                if (it != nodes_.end()) {
                    if (it->second.children.empty()) {
                        // Leaf node - add path
                        paths.push_back(path);
                    } else {
                        // Internal node - recurse
                        for (const auto& child : it->second.children) {
                            dfs(child, path);
                        }
                    }
                }

                path.pop_back();
            };

        for (const auto& [id, node] : nodes_) {
            if (node.is_leaf) {
                std::vector<std::string> path;
                dfs(id, path);
            }
        }

        return paths;
    }

    // Suggest countermeasures for high-probability paths
    std::vector<std::string> suggest_countermeasures() {
        std::vector<std::string> suggestions;

        auto paths = find_vulnerable_paths();
        double max_prob = 0.0;

        for (const auto& path : paths) {
            double prob = calculate_path_probability(path);
            if (prob > max_prob) {
                max_prob = prob;
                // Get countermeasures for this path
                for (const auto& node_id : path) {
                    auto it = nodes_.find(node_id);
                    if (it != nodes_.end()) {
                        suggestions.insert(suggestions.end(),
                                         it->second.countermeasures.begin(),
                                         it->second.countermeasures.end());
                    }
                }
            }
        }

        // Remove duplicates
        std::sort(suggestions.begin(), suggestions.end());
        auto last = std::unique(suggestions.begin(), suggestions.end());
        suggestions.erase(last, suggestions.end());

        return suggestions;
    }

private:
    double calculate_path_probability(const std::vector<std::string>& path) {
        double prob = 1.0;
        for (const auto& node_id : path) {
            auto it = nodes_.find(node_id);
            if (it != nodes_.end()) {
                prob *= it->second.probability;
            }
        }
        return prob;
    }

    std::string root_goal_;
    std::unordered_map<std::string, AttackTreeNode> nodes_;
};

// Risk assessment engine
class RiskAssessment {
public:
    struct RiskScore {
        double inherent_risk;
        double residual_risk;
        RiskLevel level;
        std::vector<std::string> top_threats;
        std::vector<std::string> recommended_countermeasures;
    };

    RiskAssessment() = default;

    void add_threat(const Threat& threat) {
        threats_[threat.id] = threat;
    }

    void add_countermeasure(const Countermeasure& countermeasure) {
        countermeasures_[countermeasure.id] = countermeasure;
    }

    // Calculate overall risk score
    RiskScore assess_risks() {
        RiskScore score;

        // Calculate inherent risk (without countermeasures)
        double total_risk = 0.0;
        std::vector<std::pair<double, std::string>> threat_risks;

        for (const auto& [id, threat] : threats_) {
            double risk = threat.risk_score();
            total_risk += risk;
            threat_risks.emplace_back(risk, threat.name);
        }

        score.inherent_risk = total_risk / threats_.size();

        // Sort threats by risk
        std::sort(threat_risks.rbegin(), threat_risks.rend());
        for (size_t i = 0; i < std::min(size_t(5), threat_risks.size()); ++i) {
            score.top_threats.push_back(threat_risks[i].second);
        }

        // Calculate residual risk with countermeasures
        double residual_total = 0.0;
        for (const auto& [id, threat] : threats_) {
            double threat_risk = threat.risk_score();

            // Apply countermeasures effectiveness
            for (const auto& countermeasure_id : threat.mitigation_references) {
                auto it = countermeasures_.find(countermeasure_id);
                if (it != countermeasures_.end()) {
                    threat_risk *= (1.0 - it->second.effectiveness);
                }
            }

            residual_total += threat_risk;
        }

        score.residual_risk = residual_total / threats_.size();

        // Determine risk level
        if (score.residual_risk >= 12) score.level = RiskLevel::CRITICAL;
        else if (score.residual_risk >= 8) score.level = RiskLevel::HIGH;
        else if (score.residual_risk >= 4) score.level = RiskLevel::MEDIUM;
        else score.level = RiskLevel::LOW;

        // Recommend countermeasures
        score.recommended_countermeasures = recommend_countermeasures();

        return score;
    }

    // Cost-benefit analysis of countermeasures
    struct CostBenefitAnalysis {
        std::vector<std::string> high_impact_low_cost;
        std::vector<std::string> high_impact_high_cost;
        double total_cost;
        double total_risk_reduction;
    };

    CostBenefitAnalysis analyze_cost_benefit() {
        CostBenefitAnalysis analysis;

        for (const auto& [id, counter] : countermeasures_) {
            double impact = counter.effectiveness;
            double cost = counter.cost;

            if (impact >= 0.7) {  // High impact
                if (cost <= 0.3) {  // Low cost
                    analysis.high_impact_low_cost.push_back(counter.name);
                } else {
                    analysis.high_impact_high_cost.push_back(counter.name);
                }
            }

            analysis.total_cost += cost;
            analysis.total_risk_reduction += impact;
        }

        return analysis;
    }

private:
    std::vector<std::string> recommend_countermeasures() {
        std::vector<std::pair<double, std::string>> scored_counters;

        for (const auto& [id, counter] : countermeasures_) {
            double score = counter.cost_effectiveness();
            scored_counters.emplace_back(score, counter.name);
        }

        // Sort by cost-effectiveness (highest first)
        std::sort(scored_counters.rbegin(), scored_counters.rend());

        std::vector<std::string> recommendations;
        for (size_t i = 0; i < std::min(size_t(10), scored_counters.size()); ++i) {
            recommendations.push_back(scored_counters[i].second);
        }

        return recommendations;
    }

    std::unordered_map<std::string, Threat> threats_;
    std::unordered_map<std::string, Countermeasure> countermeasures_;
};

// Threat intelligence integration
class ThreatIntelligence {
public:
    struct ThreatIndicator {
        std::string id;
        std::string type;  // IP, domain, hash, etc.
        std::string value;
        ThreatSeverity severity;
        std::string description;
        std::chrono::system_clock::time_point last_seen;
        std::vector<std::string> tags;
    };

    ThreatIntelligence() = default;

    void add_indicator(const ThreatIndicator& indicator) {
        indicators_[indicator.id] = indicator;
        if (indicator.type == "ip") {
            ip_indicators_[indicator.value] = indicator;
        } else if (indicator.type == "domain") {
            domain_indicators_[indicator.value] = indicator;
        }
    }

    // Check if an indicator matches known threats
    std::optional<ThreatIndicator> check_indicator(const std::string& type,
                                                  const std::string& value) {
        if (type == "ip") {
            auto it = ip_indicators_.find(value);
            if (it != ip_indicators_.end()) {
                return it->second;
            }
        } else if (type == "domain") {
            auto it = domain_indicators_.find(value);
            if (it != domain_indicators_.end()) {
                return it->second;
            }
        }

        return std::nullopt;
    }

    // Update indicators from threat feeds
    void update_from_feed(const std::vector<ThreatIndicator>& new_indicators) {
        for (const auto& indicator : new_indicators) {
            add_indicator(indicator);
        }
        last_update_ = std::chrono::system_clock::now();
    }

    std::chrono::system_clock::time_point last_update() const { return last_update_; }

private:
    std::unordered_map<std::string, ThreatIndicator> indicators_;
    std::unordered_map<std::string, ThreatIndicator> ip_indicators_;
    std::unordered_map<std::string, ThreatIndicator> domain_indicators_;
    std::chrono::system_clock::time_point last_update_;
};

// Automated threat modeling system
class AutomatedThreatModeler {
public:
    AutomatedThreatModeler() : stride_(), risk_assessment_(), threat_intel_() {}

    // Model a system automatically
    ThreatModel model_system(const std::vector<Asset>& assets,
                           const std::vector<DataFlow>& data_flows) {
        ThreatModel model;

        // Add assets to STRIDE model
        for (const auto& asset : assets) {
            stride_.add_asset(asset);
        }

        // Add data flows
        for (const auto& flow : data_flows) {
            stride_.add_data_flow(flow);
        }

        // Generate threats
        auto threats = stride_.generate_threats();

        // Add threats to risk assessment
        for (const auto& threat : threats) {
            risk_assessment_.add_threat(threat);
        }

        // Generate attack trees for high-risk threats
        for (const auto& threat : threats) {
            if (threat.risk_level() >= RiskLevel::HIGH) {
                auto attack_tree = generate_attack_tree(threat);
                model.add_attack_tree(threat.id, attack_tree);
            }
        }

        return model;
    }

    // Generate countermeasures for identified threats
    std::vector<Countermeasure> generate_countermeasures(const std::vector<Threat>& threats) {
        std::vector<Countermeasure> countermeasures;

        for (const auto& threat : threats) {
            switch (threat.category) {
                case STRIDE_Category::SPOOFING:
                    countermeasures.push_back({
                        "mfa_" + threat.id,
                        "Multi-Factor Authentication",
                        "Implement MFA to prevent spoofing attacks",
                        {threat.id},
                        0.9,  // 90% effective
                        0.4,  // Medium cost
                        "Medium",
                        {"Authentication system"}
                    });
                    break;

                case STRIDE_Category::TAMPERING:
                    countermeasures.push_back({
                        "integrity_" + threat.id,
                        "Data Integrity Checks",
                        "Implement cryptographic integrity verification",
                        {threat.id},
                        0.8,
                        0.3,
                        "Low",
                        {"Cryptography library"}
                    });
                    break;

                case STRIDE_Category::INFORMATION_DISCLOSURE:
                    countermeasures.push_back({
                        "encryption_" + threat.id,
                        "Data Encryption",
                        "Encrypt sensitive data at rest and in transit",
                        {threat.id},
                        0.95,
                        0.5,
                        "Medium",
                        {"Cryptography library", "Key management"}
                    });
                    break;

                case STRIDE_Category::DENIAL_OF_SERVICE:
                    countermeasures.push_back({
                        "ratelimit_" + threat.id,
                        "Rate Limiting",
                        "Implement rate limiting and throttling",
                        {threat.id},
                        0.7,
                        0.2,
                        "Low",
                        {"Load balancer"}
                    });
                    break;

                case STRIDE_Category::ELEVATION_OF_PRIVILEGE:
                    countermeasures.push_back({
                        "least_privilege_" + threat.id,
                        "Least Privilege Principle",
                        "Implement principle of least privilege",
                        {threat.id},
                        0.85,
                        0.6,
                        "High",
                        {"Authorization system", "Access control"}
                    });
                    break;

                default:
                    countermeasures.push_back({
                        "audit_" + threat.id,
                        "Security Auditing",
                        "Implement comprehensive security auditing",
                        {threat.id},
                        0.6,
                        0.3,
                        "Medium",
                        {"Logging system"}
                    });
                    break;
            }
        }

        return countermeasures;
    }

private:
    AttackTree generate_attack_tree(const Threat& threat) {
        AttackTree tree("Compromise " + threat.affected_asset);

        // Create root node
        AttackTreeNode root{"root", "Successfully " + threat.description, false, 0.0};

        // Create child nodes based on threat type
        if (threat.category == STRIDE_Category::SPOOFING) {
            root.children = {"gain_credentials", "bypass_auth"};

            tree.add_node({"gain_credentials", "Obtain valid credentials", false, 0.6,
                          {"phishing", "keylogger"}, {"mfa"}});
            tree.add_node({"phishing", "Successful phishing attack", true, 0.3, {}, {"security_awareness"}});
            tree.add_node({"keylogger", "Install keylogger malware", true, 0.4, {}, {"antivirus"}});
            tree.add_node({"bypass_auth", "Bypass authentication system", true, 0.2, {}, {"strong_auth"}});

        } else if (threat.category == STRIDE_Category::INFORMATION_DISCLOSURE) {
            root.children = {"intercept_network", "access_storage"};

            tree.add_node({"intercept_network", "Intercept unencrypted traffic", true, 0.7, {}, {"encryption"}});
            tree.add_node({"access_storage", "Access unencrypted storage", true, 0.5, {}, {"encryption"}});
        }

        tree.add_node(root);

        return tree;
    }

    STRIDE_Model stride_;
    RiskAssessment risk_assessment_;
    ThreatIntelligence threat_intel_;
};

// Threat modeling report generator
class ThreatModelReport {
public:
    static void generate_report(const ThreatModel& model,
                              const std::vector<Threat>& threats,
                              const std::vector<Countermeasure>& countermeasures,
                              const RiskAssessment::RiskScore& risk_score) {
        std::cout << "========================================" << std::endl;
        std::cout << "      THREAT MODELING REPORT" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;

        // Executive Summary
        std::cout << "EXECUTIVE SUMMARY" << std::endl;
        std::cout << "=================" << std::endl;
        std::cout << "Total Threats Identified: " << threats.size() << std::endl;
        std::cout << "Inherent Risk Score: " << std::fixed << std::setprecision(2)
                  << risk_score.inherent_risk << std::endl;
        std::cout << "Residual Risk Score: " << risk_score.residual_risk << std::endl;
        std::cout << "Overall Risk Level: ";
        switch (risk_score.level) {
            case RiskLevel::LOW: std::cout << "LOW"; break;
            case RiskLevel::MEDIUM: std::cout << "MEDIUM"; break;
            case RiskLevel::HIGH: std::cout << "HIGH"; break;
            case RiskLevel::CRITICAL: std::cout << "CRITICAL"; break;
        }
        std::cout << std::endl << std::endl;

        // Threats by Category
        std::cout << "THREATS BY CATEGORY" << std::endl;
        std::cout << "===================" << std::endl;
        std::unordered_map<STRIDE_Category, int> category_counts;
        for (const auto& threat : threats) {
            category_counts[threat.category]++;
        }

        for (const auto& [category, count] : category_counts) {
            std::string category_name;
            switch (category) {
                case STRIDE_Category::SPOOFING: category_name = "Spoofing"; break;
                case STRIDE_Category::TAMPERING: category_name = "Tampering"; break;
                case STRIDE_Category::REPUDIATION: category_name = "Repudiation"; break;
                case STRIDE_Category::INFORMATION_DISCLOSURE: category_name = "Information Disclosure"; break;
                case STRIDE_Category::DENIAL_OF_SERVICE: category_name = "Denial of Service"; break;
                case STRIDE_Category::ELEVATION_OF_PRIVILEGE: category_name = "Elevation of Privilege"; break;
            }
            std::cout << category_name << ": " << count << std::endl;
        }
        std::cout << std::endl;

        // Top Threats
        std::cout << "TOP THREATS" << std::endl;
        std::cout << "===========" << std::endl;
        for (size_t i = 0; i < risk_score.top_threats.size(); ++i) {
            std::cout << (i + 1) << ". " << risk_score.top_threats[i] << std::endl;
        }
        std::cout << std::endl;

        // Recommended Countermeasures
        std::cout << "RECOMMENDED COUNTERMEASURES" << std::endl;
        std::cout << "===========================" << std::endl;
        for (size_t i = 0; i < risk_score.recommended_countermeasures.size(); ++i) {
            std::cout << (i + 1) << ". " << risk_score.recommended_countermeasures[i] << std::endl;
        }
        std::cout << std::endl;

        // Detailed Threat Analysis
        std::cout << "DETAILED THREAT ANALYSIS" << std::endl;
        std::cout << "========================" << std::endl;
        for (const auto& threat : threats) {
            if (threat.risk_level() >= RiskLevel::HIGH) {
                std::cout << "Threat: " << threat.name << std::endl;
                std::cout << "  Category: ";
                switch (threat.category) {
                    case STRIDE_Category::SPOOFING: std::cout << "Spoofing"; break;
                    case STRIDE_Category::TAMPERING: std::cout << "Tampering"; break;
                    case STRIDE_Category::REPUDIATION: std::cout << "Repudiation"; break;
                    case STRIDE_Category::INFORMATION_DISCLOSURE: std::cout << "Information Disclosure"; break;
                    case STRIDE_Category::DENIAL_OF_SERVICE: std::cout << "Denial of Service"; break;
                    case STRIDE_Category::ELEVATION_OF_PRIVILEGE: std::cout << "Elevation of Privilege"; break;
                }
                std::cout << std::endl;
                std::cout << "  Risk Level: ";
                switch (threat.risk_level()) {
                    case RiskLevel::LOW: std::cout << "LOW"; break;
                    case RiskLevel::MEDIUM: std::cout << "MEDIUM"; break;
                    case RiskLevel::HIGH: std::cout << "HIGH"; break;
                    case RiskLevel::CRITICAL: std::cout << "CRITICAL"; break;
                }
                std::cout << " (Score: " << threat.risk_score() << ")" << std::endl;
                std::cout << "  Affected Asset: " << threat.affected_asset << std::endl;
                std::cout << "  Description: " << threat.description << std::endl;
                std::cout << std::endl;
            }
        }
    }
};

// Threat model container
class ThreatModel {
public:
    void add_asset(const Asset& asset) { assets_.push_back(asset); }
    void add_data_flow(const DataFlow& flow) { data_flows_.push_back(flow); }
    void add_threat(const Threat& threat) { threats_.push_back(threat); }
    void add_countermeasure(const Countermeasure& countermeasure) {
        countermeasures_.push_back(countermeasure);
    }
    void add_attack_tree(const std::string& threat_id, const AttackTree& tree) {
        attack_trees_[threat_id] = tree;
    }

    const std::vector<Asset>& assets() const { return assets_; }
    const std::vector<DataFlow>& data_flows() const { return data_flows_; }
    const std::vector<Threat>& threats() const { return threats_; }
    const std::vector<Countermeasure>& countermeasures() const { return countermeasures_; }
    const std::unordered_map<std::string, AttackTree>& attack_trees() const { return attack_trees_; }

private:
    std::vector<Asset> assets_;
    std::vector<DataFlow> data_flows_;
    std::vector<Threat> threats_;
    std::vector<Countermeasure> countermeasures_;
    std::unordered_map<std::string, AttackTree> attack_trees_;
};

// Demo application
int main() {
    std::cout << "Threat Modeling Patterns Demo\n";
    std::cout << "=============================\n\n";

    // Create system model
    std::vector<Asset> assets = {
        {"web_server", "Web Server", "Main web application server", AssetType::PROCESS, 8.0,
         {"confidential"}, {"encryption", "authentication"}},
        {"database", "Customer Database", "Stores customer PII", AssetType::DATA, 9.0,
         {"confidential", "pii"}, {"encryption", "access_control"}},
        {"user_auth", "User Authentication", "Handles user login", AssetType::EXTERNAL_ENTITY, 7.0,
         {"authentication"}, {"mfa", "strong_passwords"}},
        {"api_gateway", "API Gateway", "Routes API requests", AssetType::PROCESS, 6.0,
         {"business_logic"}, {"rate_limiting", "authentication"}}
    };

    std::vector<DataFlow> data_flows = {
        {"login_flow", "User Login", "user_auth", "web_server", "credentials",
         {"http"}, false, {"internet"}},
        {"api_flow", "API Calls", "web_server", "api_gateway", "requests",
         {"https"}, true, {"internal_network"}},
        {"db_flow", "Database Queries", "api_gateway", "database", "queries",
         {"sql"}, false, {"internal_network"}}
    };

    // Automated threat modeling
    AutomatedThreatModeler modeler;

    std::cout << "Analyzing system with " << assets.size() << " assets and "
              << data_flows.size() << " data flows...\n\n";

    auto threat_model = modeler.model_system(assets, data_flows);
    auto threats = threat_model.threats();

    std::cout << "Identified " << threats.size() << " potential threats:\n";
    for (const auto& threat : threats) {
        std::cout << "- " << threat.name << " (" << threat.risk_score() << " risk score)\n";
    }
    std::cout << "\n";

    // Generate countermeasures
    auto countermeasures = modeler.generate_countermeasures(threats);

    std::cout << "Generated " << countermeasures.size() << " countermeasures:\n";
    for (const auto& counter : countermeasures) {
        std::cout << "- " << counter.name << " (effectiveness: "
                  << (counter.effectiveness * 100) << "%)\n";
    }
    std::cout << "\n";

    // Risk assessment
    RiskAssessment risk_assessment;

    for (const auto& threat : threats) {
        risk_assessment.add_threat(threat);
    }

    for (const auto& counter : countermeasures) {
        risk_assessment.add_countermeasure(counter);
    }

    auto risk_score = risk_assessment.assess_risks();

    // Cost-benefit analysis
    auto cost_benefit = risk_assessment.analyze_cost_benefit();

    std::cout << "Cost-Benefit Analysis:\n";
    std::cout << "High Impact, Low Cost countermeasures: "
              << cost_benefit.high_impact_low_cost.size() << "\n";
    std::cout << "Total implementation cost: " << cost_benefit.total_cost << "\n";
    std::cout << "Total risk reduction: " << cost_benefit.total_risk_reduction << "\n\n";

    // Attack tree analysis
    std::cout << "Attack Tree Analysis:\n";
    for (const auto& [threat_id, attack_tree] : threat_model.attack_trees()) {
        double success_prob = attack_tree.calculate_success_probability();
        std::cout << "Attack tree for " << threat_id << ":\n";
        std::cout << "  Success probability: " << (success_prob * 100) << "%\n";

        auto suggestions = attack_tree.suggest_countermeasures();
        if (!suggestions.empty()) {
            std::cout << "  Suggested countermeasures:\n";
            for (const auto& suggestion : suggestions) {
                std::cout << "    - " << suggestion << "\n";
            }
        }
        std::cout << "\n";
    }

    // Threat intelligence
    ThreatIntelligence threat_intel;

    threat_intel.add_indicator({
        "malicious_ip_1", "ip", "192.168.1.100", ThreatSeverity::HIGH,
        "Known malicious IP address", std::chrono::system_clock::now(),
        {"malware", "c2_server"}
    });

    // Check indicators
    auto indicator = threat_intel.check_indicator("ip", "192.168.1.100");
    if (indicator) {
        std::cout << "Threat Intelligence Alert:\n";
        std::cout << "IP " << indicator->value << " is flagged as "
                  << indicator->description << "\n\n";
    }

    // Generate comprehensive report
    ThreatModelReport::generate_report(threat_model, threats, countermeasures, risk_score);

    std::cout << "\nDemo completed!\n";

    return 0;
}

/*
 * Key Features Demonstrated:
 *
 * 1. STRIDE Threat Modeling:
 *    - Systematic threat enumeration for system assets
 *    - Six categories: Spoofing, Tampering, Repudiation, Information Disclosure, DoS, Elevation
 *    - Context-aware threat generation based on asset types
 *
 * 2. Attack Tree Analysis:
 *    - Hierarchical modeling of attack paths
 *    - Probability calculations for attack success
 *    - Countermeasure effectiveness analysis
 *
 * 3. Risk Assessment:
 *    - Quantitative risk scoring (severity × likelihood)
 *    - Risk prioritization and ranking
 *    - Cost-benefit analysis of countermeasures
 *
 * 4. Automated Threat Modeling:
 *    - System asset analysis
 *    - Data flow examination
 *    - Countermeasure generation
 *    - Report generation
 *
 * 5. Threat Intelligence Integration:
 *    - Indicator of compromise (IOC) matching
 *    - Threat feed updates
 *    - Real-time threat detection
 *
 * Real-World Applications:
 * - Microsoft SDL (Security Development Lifecycle)
 * - OWASP Threat Modeling projects
 * - NIST Cybersecurity Framework implementation
 * - Financial institution security assessments
 * - Government system accreditation (FedRAMP/DIACAP)
 * - Automotive security (SAE J3061)
 */
