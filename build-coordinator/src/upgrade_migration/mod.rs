//! Upgrade & Migration: UNIQUENESS Zero-Downtime Upgrades
//!
//! Research-backed upgrade management for distributed coordination:
//! - **Rolling Upgrades**: Zero-downtime cluster upgrades with rollback capability
//! - **Schema Migration**: Automated data schema evolution
//! - **Configuration Migration**: Safe configuration updates across versions
//! - **API Versioning**: Backward-compatible API evolution
//! - **Feature Flags**: Runtime feature toggling for gradual rollouts
//! - **Migration Testing**: Automated validation of upgrade procedures

pub mod rolling_upgrades;
pub mod schema_migration;
pub mod config_migration;
pub mod api_versioning;
pub mod feature_flags;
pub mod migration_testing;

pub use rolling_upgrades::RollingUpgradeManager;
pub use schema_migration::SchemaMigrator;
pub use config_migration::ConfigMigrator;
pub use api_versioning::APIVersionManager;
pub use feature_flags::FeatureFlagManager;
pub use migration_testing::MigrationTester;

// UNIQUENESS Research Citations:
// - **Rolling Upgrades**: Netflix, Google deployment strategies
// - **Schema Evolution**: Database schema migration research
// - **Feature Flags**: LaunchDarkly, Facebook feature flag research
// - **API Versioning**: REST API evolution research papers
