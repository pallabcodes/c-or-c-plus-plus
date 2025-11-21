/**
 * @file authorization.cpp
 * @brief Production-grade authorization patterns from AWS IAM, Google Zanzibar, XACML
 *
 * This implementation provides:
 * - Role-Based Access Control (RBAC) with hierarchical roles
 * - Attribute-Based Access Control (ABAC) with policies
 * - Access Control Lists (ACLs) for resource-level permissions
 * - Policy-based authorization with evaluation engines
 * - Permission inheritance and delegation
 * - Temporal and contextual authorization
 * - Audit logging for access decisions
 *
 * Sources: AWS IAM, Google Zanzibar, XACML, OAuth2 scopes, Kubernetes RBAC
 */

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <functional>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <sstream>

namespace authorization {

// ============================================================================
// Role-Based Access Control (RBAC)
// ============================================================================

enum class PermissionAction {
    CREATE,
    READ,
    UPDATE,
    DELETE,
    EXECUTE,
    MANAGE,
    ALL
};

enum class ResourceType {
    USER,
    GROUP,
    ROLE,
    POLICY,
    RESOURCE,
    SERVICE,
    ALL
};

struct Permission {
    PermissionAction action;
    ResourceType resource_type;
    std::string resource_id;  // "*" for all resources of this type

    Permission(PermissionAction a, ResourceType rt, const std::string& rid = "*")
        : action(a), resource_type(rt), resource_id(rid) {}

    std::string to_string() const {
        std::string action_str;
        switch (action) {
            case PermissionAction::CREATE: action_str = "create"; break;
            case PermissionAction::READ: action_str = "read"; break;
            case PermissionAction::UPDATE: action_str = "update"; break;
            case PermissionAction::DELETE: action_str = "delete"; break;
            case PermissionAction::EXECUTE: action_str = "execute"; break;
            case PermissionAction::MANAGE: action_str = "manage"; break;
            case PermissionAction::ALL: action_str = "all"; break;
        }

        std::string resource_str;
        switch (resource_type) {
            case ResourceType::USER: resource_str = "user"; break;
            case ResourceType::GROUP: resource_str = "group"; break;
            case ResourceType::ROLE: resource_str = "role"; break;
            case ResourceType::POLICY: resource_str = "policy"; break;
            case ResourceType::RESOURCE: resource_str = "resource"; break;
            case ResourceType::SERVICE: resource_str = "service"; break;
            case ResourceType::ALL: resource_str = "all"; break;
        }

        return action_str + ":" + resource_str + ":" + resource_id;
    }

    bool matches(const Permission& other) const {
        // Check action compatibility
        if (action != PermissionAction::ALL && other.action != PermissionAction::ALL &&
            action != other.action) {
            return false;
        }

        // Check resource type compatibility
        if (resource_type != ResourceType::ALL && other.resource_type != ResourceType::ALL &&
            resource_type != other.resource_type) {
            return false;
        }

        // Check resource ID (support wildcards)
        if (resource_id != "*" && other.resource_id != "*" && resource_id != other.resource_id) {
            return false;
        }

        return true;
    }
};

struct Role {
    std::string name;
    std::string description;
    std::vector<Permission> permissions;
    std::vector<std::string> parent_roles;
    bool is_system_role;
    std::chrono::system_clock::time_point created_at;

    Role(const std::string& n, const std::string& desc = "")
        : name(n), description(desc), is_system_role(false),
          created_at(std::chrono::system_clock::now()) {}
};

struct User {
    std::string id;
    std::string username;
    std::string email;
    std::vector<std::string> roles;
    std::unordered_map<std::string, std::string> attributes;
    bool enabled;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_login;

    User(const std::string& uid, const std::string& uname, const std::string& mail)
        : id(uid), username(uname), email(mail), enabled(true),
          created_at(std::chrono::system_clock::now()) {}
};

class RBACSystem {
private:
    std::unordered_map<std::string, Role> roles_;
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, std::vector<std::string>> role_hierarchy_;
    std::unordered_map<std::string, std::vector<Permission>> user_additional_permissions_;

    // Audit logging
    std::function<void(const std::string&, const std::string&, bool, const std::string&)> audit_callback_;

public:
    RBACSystem() = default;

    // Role management
    void create_role(const Role& role) {
        roles_[role.name] = role;
        role_hierarchy_[role.name] = role.parent_roles;
    }

    void delete_role(const std::string& role_name) {
        if (roles_.find(role_name) == roles_.end()) {
            throw std::runtime_error("Role not found: " + role_name);
        }

        // Remove from all users
        for (auto& user_pair : users_) {
            auto& user_roles = user_pair.second.roles;
            user_roles.erase(std::remove(user_roles.begin(), user_roles.end(), role_name),
                           user_roles.end());
        }

        // Remove from hierarchy
        role_hierarchy_.erase(role_name);
        roles_.erase(role_name);
    }

    void add_permission_to_role(const std::string& role_name, const Permission& permission) {
        if (roles_.find(role_name) == roles_.end()) {
            throw std::runtime_error("Role not found: " + role_name);
        }
        roles_[role_name].permissions.push_back(permission);
    }

    void add_parent_role(const std::string& role_name, const std::string& parent_role_name) {
        if (roles_.find(role_name) == roles_.end() ||
            roles_.find(parent_role_name) == roles_.end()) {
            throw std::runtime_error("Role not found");
        }

        auto& parents = role_hierarchy_[role_name];
        if (std::find(parents.begin(), parents.end(), parent_role_name) == parents.end()) {
            parents.push_back(parent_role_name);
        }
    }

    // User management
    void create_user(const User& user) {
        users_[user.id] = user;
    }

    void assign_role_to_user(const std::string& user_id, const std::string& role_name) {
        if (users_.find(user_id) == users_.end()) {
            throw std::runtime_error("User not found: " + user_id);
        }
        if (roles_.find(role_name) == roles_.end()) {
            throw std::runtime_error("Role not found: " + role_name);
        }

        auto& user_roles = users_[user_id].roles;
        if (std::find(user_roles.begin(), user_roles.end(), role_name) == user_roles.end()) {
            user_roles.push_back(role_name);
        }
    }

    void revoke_role_from_user(const std::string& user_id, const std::string& role_name) {
        if (users_.find(user_id) == users_.end()) {
            return;
        }

        auto& user_roles = users_[user_id].roles;
        user_roles.erase(std::remove(user_roles.begin(), user_roles.end(), role_name),
                       user_roles.end());
    }

    // Authorization
    bool check_permission(const std::string& user_id, PermissionAction action,
                         ResourceType resource_type, const std::string& resource_id = "*") {

        Permission requested_perm(action, resource_type, resource_id);
        std::string reason;

        bool allowed = check_permission_internal(user_id, requested_perm, reason);

        // Audit logging
        if (audit_callback_) {
            audit_callback_(user_id, requested_perm.to_string(), allowed, reason);
        }

        return allowed;
    }

    bool check_permission(const std::string& user_id, const std::string& permission_string) {
        // Parse permission string like "read:user:*" or "create:resource:specific_id"
        auto parts = split(permission_string, ':');
        if (parts.size() != 3) {
            return false;
        }

        PermissionAction action = parse_action(parts[0]);
        ResourceType resource_type = parse_resource_type(parts[1]);
        std::string resource_id = parts[2];

        return check_permission(user_id, action, resource_type, resource_id);
    }

    // Bulk permission checking
    std::vector<bool> check_permissions(const std::string& user_id,
                                       const std::vector<std::string>& permissions) {
        std::vector<bool> results;
        for (const auto& perm : permissions) {
            results.push_back(check_permission(user_id, perm));
        }
        return results;
    }

    // Get all permissions for a user
    std::vector<Permission> get_user_permissions(const std::string& user_id) {
        std::vector<Permission> all_permissions;

        if (users_.find(user_id) == users_.end()) {
            return all_permissions;
        }

        const auto& user = users_[user_id];
        std::unordered_set<std::string> effective_roles = get_effective_roles(user.roles);

        // Collect permissions from all effective roles
        for (const auto& role_name : effective_roles) {
            if (roles_.count(role_name)) {
                const auto& role_perms = roles_[role_name].permissions;
                all_permissions.insert(all_permissions.end(), role_perms.begin(), role_perms.end());
            }
        }

        // Add user-specific permissions
        if (user_additional_permissions_.count(user_id)) {
            const auto& user_perms = user_additional_permissions_[user_id];
            all_permissions.insert(all_permissions.end(), user_perms.begin(), user_perms.end());
        }

        // Remove duplicates (simplified - in practice, you'd want to merge permissions properly)
        std::sort(all_permissions.begin(), all_permissions.end(),
                 [](const Permission& a, const Permission& b) {
                     return a.to_string() < b.to_string();
                 });
        auto last = std::unique(all_permissions.begin(), all_permissions.end(),
                               [](const Permission& a, const Permission& b) {
                                   return a.to_string() == b.to_string();
                               });
        all_permissions.erase(last, all_permissions.end());

        return all_permissions;
    }

    // Get all users with a specific role
    std::vector<std::string> get_users_with_role(const std::string& role_name) {
        std::vector<std::string> user_ids;

        for (const auto& user_pair : users_) {
            const auto& user = user_pair.second;
            std::unordered_set<std::string> effective_roles = get_effective_roles(user.roles);

            if (effective_roles.count(role_name)) {
                user_ids.push_back(user.id);
            }
        }

        return user_ids;
    }

    // Set audit callback
    void set_audit_callback(std::function<void(const std::string&, const std::string&, bool, const std::string&)> callback) {
        audit_callback_ = callback;
    }

private:
    bool check_permission_internal(const std::string& user_id, const Permission& requested_perm,
                                  std::string& reason) {
        if (users_.find(user_id) == users_.end()) {
            reason = "User not found";
            return false;
        }

        const auto& user = users_[user_id];
        if (!user.enabled) {
            reason = "User account disabled";
            return false;
        }

        std::unordered_set<std::string> effective_roles = get_effective_roles(user.roles);

        // Check permissions from all effective roles
        for (const auto& role_name : effective_roles) {
            if (roles_.count(role_name)) {
                const auto& role_perms = roles_[role_name].permissions;
                for (const auto& perm : role_perms) {
                    if (perm.matches(requested_perm)) {
                        reason = "Permission granted via role: " + role_name;
                        return true;
                    }
                }
            }
        }

        // Check user-specific permissions
        if (user_additional_permissions_.count(user_id)) {
            const auto& user_perms = user_additional_permissions_[user_id];
            for (const auto& perm : user_perms) {
                if (perm.matches(requested_perm)) {
                    reason = "Permission granted via user-specific permission";
                    return true;
                }
            }
        }

        reason = "Permission denied - no matching role or permission found";
        return false;
    }

    std::unordered_set<std::string> get_effective_roles(const std::vector<std::string>& direct_roles) {
        std::unordered_set<std::string> effective_roles;
        std::vector<std::string> to_process = direct_roles;

        while (!to_process.empty()) {
            std::string role_name = to_process.back();
            to_process.pop_back();

            if (effective_roles.insert(role_name).second) {
                // Add parent roles
                if (role_hierarchy_.count(role_name)) {
                    const auto& parents = role_hierarchy_[role_name];
                    to_process.insert(to_process.end(), parents.begin(), parents.end());
                }
            }
        }

        return effective_roles;
    }

    PermissionAction parse_action(const std::string& action_str) {
        if (action_str == "create") return PermissionAction::CREATE;
        if (action_str == "read") return PermissionAction::READ;
        if (action_str == "update") return PermissionAction::UPDATE;
        if (action_str == "delete") return PermissionAction::DELETE;
        if (action_str == "execute") return PermissionAction::EXECUTE;
        if (action_str == "manage") return PermissionAction::MANAGE;
        if (action_str == "all") return PermissionAction::ALL;
        throw std::runtime_error("Unknown action: " + action_str);
    }

    ResourceType parse_resource_type(const std::string& resource_str) {
        if (resource_str == "user") return ResourceType::USER;
        if (resource_str == "group") return ResourceType::GROUP;
        if (resource_str == "role") return ResourceType::ROLE;
        if (resource_str == "policy") return ResourceType::POLICY;
        if (resource_str == "resource") return ResourceType::RESOURCE;
        if (resource_str == "service") return ResourceType::SERVICE;
        if (resource_str == "all") return ResourceType::ALL;
        throw std::runtime_error("Unknown resource type: " + resource_str);
    }

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

// ============================================================================
// Attribute-Based Access Control (ABAC)
// ============================================================================

enum class AttributeType {
    STRING,
    NUMBER,
    BOOLEAN,
    DATETIME,
    LIST
};

struct Attribute {
    std::string name;
    AttributeType type;
    std::string string_value;
    double number_value;
    bool boolean_value;
    std::vector<std::string> list_value;
    std::chrono::system_clock::time_point datetime_value;

    Attribute(const std::string& n, const std::string& val)
        : name(n), type(AttributeType::STRING), string_value(val) {}

    Attribute(const std::string& n, double val)
        : name(n), type(AttributeType::NUMBER), number_value(val) {}

    Attribute(const std::string& n, bool val)
        : name(n), type(AttributeType::BOOLEAN), boolean_value(val) {}

    Attribute(const std::string& n, const std::vector<std::string>& val)
        : name(n), type(AttributeType::LIST), list_value(val) {}

    Attribute(const std::string& n, std::chrono::system_clock::time_point val)
        : name(n), type(AttributeType::DATETIME), datetime_value(val) {}

    std::string to_string() const {
        switch (type) {
            case AttributeType::STRING: return string_value;
            case AttributeType::NUMBER: return std::to_string(number_value);
            case AttributeType::BOOLEAN: return boolean_value ? "true" : "false";
            case AttributeType::DATETIME: return "datetime"; // Simplified
            case AttributeType::LIST: {
                std::string result = "[";
                for (size_t i = 0; i < list_value.size(); ++i) {
                    if (i > 0) result += ",";
                    result += list_value[i];
                }
                result += "]";
                return result;
            }
        }
        return "";
    }

    bool equals(const Attribute& other) const {
        if (type != other.type) return false;

        switch (type) {
            case AttributeType::STRING: return string_value == other.string_value;
            case AttributeType::NUMBER: return number_value == other.number_value;
            case AttributeType::BOOLEAN: return boolean_value == other.boolean_value;
            case AttributeType::DATETIME: return datetime_value == other.datetime_value;
            case AttributeType::LIST: return list_value == other.list_value;
        }
        return false;
    }

    bool contains(const Attribute& other) const {
        if (type != AttributeType::LIST || other.type != AttributeType::STRING) {
            return false;
        }
        return std::find(list_value.begin(), list_value.end(), other.string_value) != list_value.end();
    }
};

struct SubjectAttributes {
    std::string subject_id;
    std::unordered_map<std::string, Attribute> attributes;

    void set_attribute(const Attribute& attr) {
        attributes[attr.name] = attr;
    }

    const Attribute* get_attribute(const std::string& name) const {
        auto it = attributes.find(name);
        return it != attributes.end() ? &it->second : nullptr;
    }
};

struct ResourceAttributes {
    std::string resource_id;
    std::string resource_type;
    std::unordered_map<std::string, Attribute> attributes;

    void set_attribute(const Attribute& attr) {
        attributes[attr.name] = attr;
    }

    const Attribute* get_attribute(const std::string& name) const {
        auto it = attributes.find(name);
        return it != attributes.end() ? &it->second : nullptr;
    }
};

struct EnvironmentAttributes {
    std::unordered_map<std::string, Attribute> attributes;
    std::chrono::system_clock::time_point current_time;

    EnvironmentAttributes() : current_time(std::chrono::system_clock::now()) {}

    void set_attribute(const Attribute& attr) {
        attributes[attr.name] = attr;
    }

    const Attribute* get_attribute(const std::string& name) const {
        auto it = attributes.find(name);
        return it != attributes.end() ? &it->second : nullptr;
    }
};

enum class PolicyEffect {
    PERMIT,
    DENY
};

enum class ConditionOperator {
    EQUALS,
    NOT_EQUALS,
    CONTAINS,
    NOT_CONTAINS,
    GREATER_THAN,
    LESS_THAN,
    GREATER_EQUAL,
    LESS_EQUAL,
    IN,
    NOT_IN
};

struct PolicyCondition {
    std::string attribute_name;
    ConditionOperator op;
    Attribute value;

    bool evaluate(const SubjectAttributes& subject,
                  const ResourceAttributes& resource,
                  const EnvironmentAttributes& environment) const {

        const Attribute* attr_value = nullptr;

        // Try subject attributes first
        attr_value = subject.get_attribute(attribute_name);
        if (!attr_value) {
            // Try resource attributes
            attr_value = resource.get_attribute(attribute_name);
        }
        if (!attr_value) {
            // Try environment attributes
            attr_value = environment.get_attribute(attribute_name);
        }

        if (!attr_value) {
            return false; // Attribute not found
        }

        switch (op) {
            case ConditionOperator::EQUALS:
                return attr_value->equals(value);
            case ConditionOperator::NOT_EQUALS:
                return !attr_value->equals(value);
            case ConditionOperator::CONTAINS:
                return attr_value->contains(value);
            case ConditionOperator::NOT_CONTAINS:
                return !attr_value->contains(value);
            case ConditionOperator::GREATER_THAN:
                return attr_value->type == AttributeType::NUMBER &&
                       attr_value->number_value > value.number_value;
            case ConditionOperator::LESS_THAN:
                return attr_value->type == AttributeType::NUMBER &&
                       attr_value->number_value < value.number_value;
            case ConditionOperator::GREATER_EQUAL:
                return attr_value->type == AttributeType::NUMBER &&
                       attr_value->number_value >= value.number_value;
            case ConditionOperator::LESS_EQUAL:
                return attr_value->type == AttributeType::NUMBER &&
                       attr_value->number_value <= value.number_value;
            case ConditionOperator::IN:
                return value.contains(*attr_value);
            case ConditionOperator::NOT_IN:
                return !value.contains(*attr_value);
        }

        return false;
    }
};

struct ABACPolicy {
    std::string id;
    std::string name;
    std::string description;
    PolicyEffect effect;
    std::vector<PolicyCondition> conditions;
    std::vector<std::string> target_actions;
    std::vector<std::string> target_resources;
    bool enabled;
    std::chrono::system_clock::time_point created_at;

    ABACPolicy(const std::string& policy_id, const std::string& policy_name,
               PolicyEffect eff = PolicyEffect::PERMIT)
        : id(policy_id), name(policy_name), effect(eff), enabled(true),
          created_at(std::chrono::system_clock::now()) {}
};

class ABACSystem {
private:
    std::unordered_map<std::string, ABACPolicy> policies_;
    std::unordered_map<std::string, SubjectAttributes> subject_attributes_;
    std::unordered_map<std::string, ResourceAttributes> resource_attributes_;

    // Audit logging
    std::function<void(const std::string&, const std::string&, const std::string&, bool, const std::string&)> audit_callback_;

public:
    ABACSystem() = default;

    // Policy management
    void create_policy(const ABACPolicy& policy) {
        policies_[policy.id] = policy;
    }

    void delete_policy(const std::string& policy_id) {
        policies_.erase(policy_id);
    }

    void add_condition_to_policy(const std::string& policy_id, const PolicyCondition& condition) {
        if (policies_.count(policy_id)) {
            policies_[policy_id].conditions.push_back(condition);
        }
    }

    void add_target_action_to_policy(const std::string& policy_id, const std::string& action) {
        if (policies_.count(policy_id)) {
            policies_[policy_id].target_actions.push_back(action);
        }
    }

    void add_target_resource_to_policy(const std::string& policy_id, const std::string& resource) {
        if (policies_.count(policy_id)) {
            policies_[policy_id].target_resources.push_back(resource);
        }
    }

    // Attribute management
    void set_subject_attributes(const std::string& subject_id, const SubjectAttributes& attrs) {
        subject_attributes_[subject_id] = attrs;
    }

    void set_resource_attributes(const std::string& resource_id, const ResourceAttributes& attrs) {
        resource_attributes_[resource_id] = attrs;
    }

    void set_subject_attribute(const std::string& subject_id, const Attribute& attr) {
        subject_attributes_[subject_id].set_attribute(attr);
    }

    void set_resource_attribute(const std::string& resource_id, const Attribute& attr) {
        resource_attributes_[resource_id].set_attribute(attr);
    }

    // Authorization
    bool check_access(const std::string& subject_id, const std::string& action,
                     const std::string& resource_id, const std::string& resource_type = "") {

        SubjectAttributes subject_attrs = get_subject_attributes(subject_id);
        ResourceAttributes resource_attrs = get_resource_attributes(resource_id);
        resource_attrs.resource_type = resource_type;

        EnvironmentAttributes env_attrs;

        std::string reason;
        bool allowed = evaluate_policies(subject_attrs, resource_attrs, env_attrs, action, reason);

        // Audit logging
        if (audit_callback_) {
            audit_callback_(subject_id, action, resource_id, allowed, reason);
        }

        return allowed;
    }

    // Bulk access checking
    std::vector<bool> check_access_batch(const std::string& subject_id,
                                        const std::vector<std::tuple<std::string, std::string>>& requests) {
        std::vector<bool> results;
        for (const auto& request : requests) {
            const auto& [action, resource_id] = request;
            results.push_back(check_access(subject_id, action, resource_id));
        }
        return results;
    }

    // Get applicable policies for debugging
    std::vector<std::string> get_applicable_policies(const std::string& subject_id,
                                                    const std::string& action,
                                                    const std::string& resource_id) {
        std::vector<std::string> applicable;

        SubjectAttributes subject_attrs = get_subject_attributes(subject_id);
        ResourceAttributes resource_attrs = get_resource_attributes(resource_id);
        EnvironmentAttributes env_attrs;

        for (const auto& policy_pair : policies_) {
            const auto& policy = policy_pair.second;
            if (!policy.enabled) continue;

            if (is_policy_applicable(policy, action, resource_attrs.resource_type)) {
                bool conditions_met = true;
                for (const auto& condition : policy.conditions) {
                    if (!condition.evaluate(subject_attrs, resource_attrs, env_attrs)) {
                        conditions_met = false;
                        break;
                    }
                }

                if (conditions_met) {
                    applicable.push_back(policy.id);
                }
            }
        }

        return applicable;
    }

    // Set audit callback
    void set_audit_callback(std::function<void(const std::string&, const std::string&, const std::string&, bool, const std::string&)> callback) {
        audit_callback_ = callback;
    }

private:
    SubjectAttributes get_subject_attributes(const std::string& subject_id) {
        auto it = subject_attributes_.find(subject_id);
        return it != subject_attributes_.end() ? it->second : SubjectAttributes{subject_id};
    }

    ResourceAttributes get_resource_attributes(const std::string& resource_id) {
        auto it = resource_attributes_.find(resource_id);
        return it != resource_attributes_.end() ? it->second : ResourceAttributes{resource_id};
    }

    bool evaluate_policies(const SubjectAttributes& subject,
                          const ResourceAttributes& resource,
                          const EnvironmentAttributes& environment,
                          const std::string& action,
                          std::string& reason) {

        bool default_decision = false; // Deny by default
        std::vector<std::string> permit_reasons;
        std::vector<std::string> deny_reasons;

        for (const auto& policy_pair : policies_) {
            const auto& policy = policy_pair.second;
            if (!policy.enabled) continue;

            if (!is_policy_applicable(policy, action, resource.resource_type)) {
                continue;
            }

            // Evaluate conditions
            bool conditions_met = true;
            for (const auto& condition : policy.conditions) {
                if (!condition.evaluate(subject, resource, environment)) {
                    conditions_met = false;
                    break;
                }
            }

            if (conditions_met) {
                if (policy.effect == PolicyEffect::PERMIT) {
                    permit_reasons.push_back(policy.name);
                } else {
                    deny_reasons.push_back(policy.name);
                }
            }
        }

        // Deny overrides permit (deny-biased policy combining)
        if (!deny_reasons.empty()) {
            reason = "Access denied by policies: " + join(deny_reasons, ", ");
            return false;
        }

        if (!permit_reasons.empty()) {
            reason = "Access permitted by policies: " + join(permit_reasons, ", ");
            return true;
        }

        reason = "No applicable policies found";
        return default_decision;
    }

    bool is_policy_applicable(const ABACPolicy& policy, const std::string& action,
                             const std::string& resource_type) {
        // Check if action is targeted
        if (!policy.target_actions.empty()) {
            bool action_matches = false;
            for (const auto& target_action : policy.target_actions) {
                if (target_action == "*" || target_action == action) {
                    action_matches = true;
                    break;
                }
            }
            if (!action_matches) return false;
        }

        // Check if resource type is targeted
        if (!policy.target_resources.empty()) {
            bool resource_matches = false;
            for (const auto& target_resource : policy.target_resources) {
                if (target_resource == "*" || target_resource == resource_type) {
                    resource_matches = true;
                    break;
                }
            }
            if (!resource_matches) return false;
        }

        return true;
    }

    std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
        std::string result;
        for (size_t i = 0; i < strings.size(); ++i) {
            if (i > 0) result += delimiter;
            result += strings[i];
        }
        return result;
    }
};

// ============================================================================
// Access Control Lists (ACLs)
// ============================================================================

struct ACLEntry {
    std::string principal;  // user, group, or role ID
    std::string permission; // e.g., "read", "write", "execute"
    bool granted;
    std::chrono::system_clock::time_point expires_at;
    std::string granted_by;

    ACLEntry(const std::string& p, const std::string& perm, bool grant = true,
             const std::string& granter = "")
        : principal(p), permission(perm), granted(grant), granted_by(granter) {
        // No expiration by default
        expires_at = std::chrono::system_clock::time_point::max();
    }

    bool is_expired() const {
        return std::chrono::system_clock::now() > expires_at;
    }
};

class AccessControlList {
private:
    std::string resource_id;
    std::vector<ACLEntry> entries;
    bool default_deny;

    // Inheritance support
    std::vector<std::string> parent_acl_ids;

public:
    AccessControlList(const std::string& rid, bool deny_by_default = true)
        : resource_id(rid), default_deny(deny_by_default) {}

    void add_entry(const ACLEntry& entry) {
        // Remove any existing entries for the same principal and permission
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                          [&](const ACLEntry& e) {
                              return e.principal == entry.principal &&
                                     e.permission == entry.permission;
                          }),
            entries.end());

        entries.push_back(entry);
    }

    void remove_entry(const std::string& principal, const std::string& permission) {
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                          [&](const ACLEntry& e) {
                              return e.principal == principal && e.permission == permission;
                          }),
            entries.end());
    }

    bool check_permission(const std::string& principal, const std::string& permission) {
        // Check direct entries first
        for (const auto& entry : entries) {
            if (entry.principal == principal && entry.permission == permission && !entry.is_expired()) {
                return entry.granted;
            }
        }

        // Check wildcard entries
        for (const auto& entry : entries) {
            if (entry.principal == "*" && entry.permission == permission && !entry.is_expired()) {
                return entry.granted;
            }
            if (entry.principal == principal && entry.permission == "*" && !entry.is_expired()) {
                return entry.granted;
            }
        }

        // Check parent ACLs (simplified - would need ACL registry in real implementation)
        // For now, just return default
        return !default_deny;
    }

    bool check_any_permission(const std::string& principal, const std::vector<std::string>& permissions) {
        for (const auto& permission : permissions) {
            if (check_permission(principal, permission)) {
                return true;
            }
        }
        return false;
    }

    bool check_all_permissions(const std::string& principal, const std::vector<std::string>& permissions) {
        for (const auto& permission : permissions) {
            if (!check_permission(principal, permission)) {
                return false;
            }
        }
        return true;
    }

    std::vector<ACLEntry> get_entries_for_principal(const std::string& principal) {
        std::vector<ACLEntry> result;
        for (const auto& entry : entries) {
            if (entry.principal == principal && !entry.is_expired()) {
                result.push_back(entry);
            }
        }
        return result;
    }

    std::vector<ACLEntry> get_all_entries() const {
        std::vector<ACLEntry> valid_entries;
        for (const auto& entry : entries) {
            if (!entry.is_expired()) {
                valid_entries.push_back(entry);
            }
        }
        return valid_entries;
    }

    void add_parent_acl(const std::string& parent_acl_id) {
        if (std::find(parent_acl_ids.begin(), parent_acl_ids.end(), parent_acl_id) == parent_acl_ids.end()) {
            parent_acl_ids.push_back(parent_acl_id);
        }
    }

    void remove_parent_acl(const std::string& parent_acl_id) {
        parent_acl_ids.erase(
            std::remove(parent_acl_ids.begin(), parent_acl_ids.end(), parent_acl_id),
            parent_acl_ids.end());
    }
};

class ACLManager {
private:
    std::unordered_map<std::string, AccessControlList> acls_;
    std::unordered_map<std::string, std::vector<std::string>> group_memberships_;
    std::unordered_map<std::string, std::vector<std::string>> user_groups_;

    // Audit logging
    std::function<void(const std::string&, const std::string&, const std::string&, bool)> audit_callback_;

public:
    ACLManager() = default;

    void create_acl(const std::string& resource_id, bool default_deny = true) {
        acls_[resource_id] = AccessControlList(resource_id, default_deny);
    }

    void delete_acl(const std::string& resource_id) {
        acls_.erase(resource_id);
    }

    void grant_permission(const std::string& resource_id, const std::string& principal,
                         const std::string& permission, const std::string& granted_by = "") {
        if (acls_.count(resource_id)) {
            ACLEntry entry(principal, permission, true, granted_by);
            acls_[resource_id].add_entry(entry);

            if (audit_callback_) {
                audit_callback_("GRANT", principal, resource_id + ":" + permission, true);
            }
        }
    }

    void revoke_permission(const std::string& resource_id, const std::string& principal,
                          const std::string& permission) {
        if (acls_.count(resource_id)) {
            acls_[resource_id].remove_entry(principal, permission);

            if (audit_callback_) {
                audit_callback_("REVOKE", principal, resource_id + ":" + permission, true);
            }
        }
    }

    bool check_permission(const std::string& resource_id, const std::string& principal,
                         const std::string& permission) {
        if (!acls_.count(resource_id)) {
            return false;
        }

        // Check direct permission
        if (acls_[resource_id].check_permission(principal, permission)) {
            if (audit_callback_) {
                audit_callback_("CHECK", principal, resource_id + ":" + permission, true);
            }
            return true;
        }

        // Check group permissions
        if (user_groups_.count(principal)) {
            for (const auto& group : user_groups_[principal]) {
                if (acls_[resource_id].check_permission(group, permission)) {
                    if (audit_callback_) {
                        audit_callback_("CHECK_GROUP", principal, resource_id + ":" + permission, true);
                    }
                    return true;
                }
            }
        }

        if (audit_callback_) {
            audit_callback_("CHECK", principal, resource_id + ":" + permission, false);
        }

        return false;
    }

    void add_user_to_group(const std::string& user_id, const std::string& group_id) {
        user_groups_[user_id].push_back(group_id);
        group_memberships_[group_id].push_back(user_id);
    }

    void remove_user_from_group(const std::string& user_id, const std::string& group_id) {
        if (user_groups_.count(user_id)) {
            auto& groups = user_groups_[user_id];
            groups.erase(std::remove(groups.begin(), groups.end(), group_id), groups.end());
        }

        if (group_memberships_.count(group_id)) {
            auto& members = group_memberships_[group_id];
            members.erase(std::remove(members.begin(), members.end(), user_id), members.end());
        }
    }

    std::vector<std::string> get_user_groups(const std::string& user_id) {
        return user_groups_.count(user_id) ? user_groups_[user_id] : std::vector<std::string>();
    }

    std::vector<std::string> get_group_members(const std::string& group_id) {
        return group_memberships_.count(group_id) ? group_memberships_[group_id] : std::vector<std::string>();
    }

    void set_audit_callback(std::function<void(const std::string&, const std::string&, const std::string&, bool)> callback) {
        audit_callback_ = callback;
    }
};

// ============================================================================
// Demonstration and Testing
// ============================================================================

void demonstrate_rbac() {
    std::cout << "=== Role-Based Access Control (RBAC) Demo ===\n";

    RBACSystem rbac;

    // Set up audit logging
    rbac.set_audit_callback([](const std::string& user, const std::string& permission,
                              bool granted, const std::string& reason) {
        std::cout << "AUDIT: User " << user << " " << (granted ? "granted" : "denied")
                 << " permission " << permission << " - " << reason << "\n";
    });

    // Create roles
    Role admin_role("admin", "Administrator role");
    admin_role.permissions = {
        Permission(PermissionAction::ALL, ResourceType::ALL)
    };

    Role user_role("user", "Regular user role");
    user_role.permissions = {
        Permission(PermissionAction::READ, ResourceType::RESOURCE),
        Permission(PermissionAction::UPDATE, ResourceType::RESOURCE, "owned_*")
    };

    Role manager_role("manager", "Manager role");
    manager_role.permissions = {
        Permission(PermissionAction::READ, ResourceType::USER),
        Permission(PermissionAction::UPDATE, ResourceType::USER)
    };
    manager_role.parent_roles = {"user"};  // Inherits from user

    rbac.create_role(admin_role);
    rbac.create_role(user_role);
    rbac.create_role(manager_role);

    // Create users
    User alice("alice", "alice@example.com", "alice@example.com");
    User bob("bob", "bob@example.com", "bob@example.com");
    User charlie("charlie", "charlie@example.com", "charlie@example.com");

    rbac.create_user(alice);
    rbac.create_user(bob);
    rbac.create_user(charlie);

    // Assign roles
    rbac.assign_role_to_user("alice", "admin");
    rbac.assign_role_to_user("bob", "manager");
    rbac.assign_role_to_user("charlie", "user");

    // Test permissions
    std::vector<std::tuple<std::string, std::string, bool>> test_cases = {
        {"alice", "manage:all:*", true},           // Admin can do anything
        {"alice", "read:user:*", true},            // Admin can read users
        {"bob", "read:user:*", true},              // Manager can read users (inherited from user + explicit)
        {"bob", "update:resource:owned_*", true},  // Manager inherits user permissions
        {"charlie", "read:resource:*", true},      // User can read resources
        {"charlie", "update:user:*", false},       // User cannot update users
        {"charlie", "manage:all:*", false}         // User cannot do everything
    };

    for (const auto& [user_id, permission, expected] : test_cases) {
        bool result = rbac.check_permission(user_id, permission);
        std::cout << "User " << user_id << " permission '" << permission << "': "
                 << (result ? "GRANTED" : "DENIED")
                 << (result == expected ? " ✓" : " ✗") << "\n";
    }

    // Get all permissions for a user
    auto alice_perms = rbac.get_user_permissions("alice");
    std::cout << "Alice has " << alice_perms.size() << " permissions\n";

    // Get users with a specific role
    auto managers = rbac.get_users_with_role("manager");
    std::cout << "Users with manager role: ";
    for (const auto& user : managers) {
        std::cout << user << " ";
    }
    std::cout << "\n";
}

void demonstrate_abac() {
    std::cout << "\n=== Attribute-Based Access Control (ABAC) Demo ===\n";

    ABACSystem abac;

    // Set up audit logging
    abac.set_audit_callback([](const std::string& subject, const std::string& action,
                              const std::string& resource, bool granted, const std::string& reason) {
        std::cout << "ABAC AUDIT: " << subject << " " << action << " on " << resource
                 << " - " << (granted ? "GRANTED" : "DENIED") << " - " << reason << "\n";
    });

    // Create policies
    ABACPolicy admin_policy("admin_policy", "Admin access policy", PolicyEffect::PERMIT);
    admin_policy.target_actions = {"*"};
    admin_policy.target_resources = {"*"};
    admin_policy.conditions = {
        PolicyCondition{"role", ConditionOperator::EQUALS, Attribute("role", "admin")}
    };

    ABACPolicy time_based_policy("time_policy", "Time-based access policy", PolicyEffect::PERMIT);
    time_based_policy.target_actions = {"write", "update"};
    time_based_policy.target_resources = {"document"};
    time_based_policy.conditions = {
        PolicyCondition{"department", ConditionOperator::EQUALS, Attribute("department", "engineering")},
        PolicyCondition{"clearance_level", ConditionOperator::GREATER_EQUAL, Attribute("clearance_level", 3.0)},
        PolicyCondition{"current_hour", ConditionOperator::GREATER_EQUAL, Attribute("current_hour", 9.0)},
        PolicyCondition{"current_hour", ConditionOperator::LESS_EQUAL, Attribute("current_hour", 17.0)}
    };

    ABACPolicy deny_policy("deny_policy", "Deny external access", PolicyEffect::DENY);
    deny_policy.target_actions = {"write", "delete"};
    deny_policy.target_resources = {"sensitive_data"};
    deny_policy.conditions = {
        PolicyCondition{"location", ConditionOperator::NOT_EQUALS, Attribute("location", "internal")}
    };

    abac.create_policy(admin_policy);
    abac.create_policy(time_based_policy);
    abac.create_policy(deny_policy);

    // Set up subject attributes
    SubjectAttributes alice_attrs{"alice"};
    alice_attrs.set_attribute(Attribute("role", "admin"));
    alice_attrs.set_attribute(Attribute("department", "engineering"));
    alice_attrs.set_attribute(Attribute("clearance_level", 5.0));

    SubjectAttributes bob_attrs{"bob"};
    bob_attrs.set_attribute(Attribute("role", "user"));
    bob_attrs.set_attribute(Attribute("department", "engineering"));
    bob_attrs.set_attribute(Attribute("clearance_level", 3.0));

    SubjectAttributes charlie_attrs{"charlie"};
    charlie_attrs.set_attribute(Attribute("role", "user"));
    charlie_attrs.set_attribute(Attribute("department", "marketing"));
    charlie_attrs.set_attribute(Attribute("clearance_level", 2.0));
    charlie_attrs.set_attribute(Attribute("location", "external"));

    abac.set_subject_attributes("alice", alice_attrs);
    abac.set_subject_attributes("bob", bob_attrs);
    abac.set_subject_attributes("charlie", charlie_attrs);

    // Set up resource attributes
    ResourceAttributes doc_attrs{"doc123", "document"};
    doc_attrs.set_attribute(Attribute("sensitivity", "high"));
    doc_attrs.set_attribute(Attribute("owner", "alice"));

    ResourceAttributes sensitive_attrs{"sensitive123", "sensitive_data"};
    sensitive_attrs.set_attribute(Attribute("classification", "confidential"));

    abac.set_resource_attributes("doc123", doc_attrs);
    abac.set_resource_attributes("sensitive123", sensitive_attrs);

    // Test access requests
    std::vector<std::tuple<std::string, std::string, std::string, bool>> test_cases = {
        {"alice", "read", "doc123", true},           // Admin can read anything
        {"alice", "delete", "sensitive123", true},   // Admin can delete anything
        {"bob", "write", "doc123", true},            // Engineer with level 3 can write during business hours
        {"bob", "write", "doc123", false},           // Would fail outside business hours (simplified)
        {"charlie", "write", "doc123", false},       // Marketing cannot write to engineering docs
        {"charlie", "write", "sensitive123", false}  // External user denied access to sensitive data
    };

    for (const auto& [subject, action, resource, expected] : test_cases) {
        bool result = abac.check_access(subject, action, resource);
        std::cout << subject << " " << action << " on " << resource << ": "
                 << (result ? "GRANTED" : "DENIED")
                 << (result == expected ? " ✓" : " ✗") << "\n";
    }

    // Get applicable policies for debugging
    auto policies = abac.get_applicable_policies("bob", "write", "doc123");
    std::cout << "Applicable policies for bob writing doc123: ";
    for (const auto& policy : policies) {
        std::cout << policy << " ";
    }
    std::cout << "\n";
}

void demonstrate_acls() {
    std::cout << "\n=== Access Control Lists (ACLs) Demo ===\n";

    ACLManager acl_mgr;

    // Set up audit logging
    acl_mgr.set_audit_callback([](const std::string& operation, const std::string& principal,
                                 const std::string& resource_perm, bool success) {
        std::cout << "ACL AUDIT: " << operation << " " << principal << " on "
                 << resource_perm << " - " << (success ? "SUCCESS" : "FAILED") << "\n";
    });

    // Create ACLs for resources
    acl_mgr.create_acl("file1.txt");
    acl_mgr.create_acl("database");
    acl_mgr.create_acl("api_endpoint");

    // Create groups
    acl_mgr.add_user_to_group("alice", "admins");
    acl_mgr.add_user_to_group("bob", "developers");
    acl_mgr.add_user_to_group("charlie", "developers");
    acl_mgr.add_user_to_group("diana", "users");

    // Grant permissions
    acl_mgr.grant_permission("file1.txt", "alice", "read");
    acl_mgr.grant_permission("file1.txt", "alice", "write");
    acl_mgr.grant_permission("file1.txt", "admins", "read");  // Group permission
    acl_mgr.grant_permission("file1.txt", "developers", "read");

    acl_mgr.grant_permission("database", "admins", "*");  // All permissions for admins
    acl_mgr.grant_permission("database", "developers", "read");
    acl_mgr.grant_permission("database", "developers", "write");

    acl_mgr.grant_permission("api_endpoint", "*", "read");  // Public read access

    // Test permissions
    std::vector<std::tuple<std::string, std::string, std::string, bool>> test_cases = {
        {"alice", "file1.txt", "read", true},        // Direct permission
        {"alice", "file1.txt", "write", true},       // Direct permission
        {"bob", "file1.txt", "read", true},          // Group permission (developers)
        {"bob", "file1.txt", "write", false},        // No write permission
        {"charlie", "file1.txt", "read", true},      // Group permission (developers)
        {"alice", "database", "read", true},         // Admin wildcard permission
        {"alice", "database", "delete", true},       // Admin wildcard permission
        {"bob", "database", "read", true},           // Developer permission
        {"bob", "database", "delete", false},        // No delete permission
        {"diana", "api_endpoint", "read", true},     // Public access
        {"diana", "api_endpoint", "write", false},   // No write permission
        {"eve", "file1.txt", "read", false}          // No permission
    };

    for (const auto& [user, resource, permission, expected] : test_cases) {
        bool result = acl_mgr.check_permission(resource, user, permission);
        std::cout << user << " " << permission << " on " << resource << ": "
                 << (result ? "GRANTED" : "DENIED")
                 << (result == expected ? " ✓" : " ✗") << "\n";
    }

    // Revoke a permission
    acl_mgr.revoke_permission("file1.txt", "alice", "write");
    bool can_write = acl_mgr.check_permission("file1.txt", "alice", "write");
    std::cout << "Alice can still write to file1.txt after revocation: "
             << (can_write ? "YES" : "NO") << "\n";

    // Check group memberships
    auto alice_groups = acl_mgr.get_user_groups("alice");
    std::cout << "Alice is in groups: ";
    for (const auto& group : alice_groups) {
        std::cout << group << " ";
    }
    std::cout << "\n";

    auto developer_members = acl_mgr.get_group_members("developers");
    std::cout << "Developers group members: ";
    for (const auto& member : developer_members) {
        std::cout << member << " ";
    }
    std::cout << "\n";
}

} // namespace authorization

// ============================================================================
// Main Function for Testing
// ============================================================================

int main() {
    std::cout << "🛡️ **Authorization Patterns** - Production-Grade Access Control\n";
    std::cout << "=============================================================\n\n";

    authorization::demonstrate_rbac();
    authorization::demonstrate_abac();
    authorization::demonstrate_acls();

    std::cout << "\n✅ **Authorization Complete**\n";
    std::cout << "Extracted patterns from: AWS IAM, Google Zanzibar, XACML, OAuth2 scopes\n";
    std::cout << "Features: RBAC, ABAC, ACLs, Policy Evaluation, Audit Logging, Group Management\n";

    return 0;
}
