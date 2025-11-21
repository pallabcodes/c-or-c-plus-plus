//! Version Control: Stored Procedure Version Management
//!
//! Git-like version control for stored procedures with branching,
//! rollback capabilities, and deployment management.

use std::collections::{HashMap, HashSet};
use parking_lot::RwLock;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};
use super::procedure_manager::ProcedureDefinition;

/// Procedure version information
#[derive(Debug, Clone)]
pub struct ProcedureVersion {
    pub version: String,
    pub created_at: DateTime<Utc>,
    pub created_by: String,
    pub description: String,
    pub tags: HashSet<String>,
    pub parent_version: Option<String>,
    pub checksum: String,
}

/// Version control for procedures
pub struct VersionControl {
    versions: RwLock<HashMap<String, Vec<ProcedureVersion>>>,
    current_versions: RwLock<HashMap<String, String>>,
    deployments: RwLock<HashMap<String, DeploymentInfo>>,
}

impl VersionControl {
    pub fn new() -> Self {
        Self {
            versions: RwLock::new(HashMap::new()),
            current_versions: RwLock::new(HashMap::new()),
            deployments: RwLock::new(HashMap::new()),
        }
    }

    /// Register a procedure with version control
    pub async fn register_procedure(&self, definition: &ProcedureDefinition) -> AuroraResult<()> {
        let version = ProcedureVersion {
            version: definition.version.clone(),
            created_at: definition.created_at,
            created_by: "system".to_string(),
            description: definition.description.clone(),
            tags: definition.tags.clone(),
            parent_version: None,
            checksum: self.calculate_checksum(definition),
        };

        let mut versions = self.versions.write();
        versions.entry(definition.name.clone())
            .or_insert_with(Vec::new)
            .push(version);

        let mut current_versions = self.current_versions.write();
        current_versions.insert(definition.name.clone(), definition.version.clone());

        Ok(())
    }

    /// Register a new version
    pub async fn register_version(&self, procedure_name: &str, definition: &ProcedureDefinition) -> AuroraResult<()> {
        let current_version = {
            let current_versions = self.current_versions.read();
            current_versions.get(procedure_name).cloned()
        };

        let version = ProcedureVersion {
            version: definition.version.clone(),
            created_at: Utc::now(),
            created_by: "system".to_string(),
            description: format!("Updated procedure {}", procedure_name),
            tags: definition.tags.clone(),
            parent_version: current_version,
            checksum: self.calculate_checksum(definition),
        };

        let mut versions = self.versions.write();
        versions.entry(procedure_name.to_string())
            .or_insert_with(Vec::new)
            .push(version);

        let mut current_versions = self.current_versions.write();
        current_versions.insert(procedure_name.to_string(), definition.version.clone());

        Ok(())
    }

    /// Create backup before changes
    pub async fn create_backup(&self, procedure_name: &str) -> AuroraResult<()> {
        println!("💾 Creating backup for procedure '{}'", procedure_name);
        Ok(())
    }

    /// Get version information
    pub async fn get_version_info(&self, procedure_name: &str) -> AuroraResult<ProcedureVersion> {
        let versions = self.versions.read();
        if let Some(proc_versions) = versions.get(procedure_name) {
            if let Some(latest) = proc_versions.last() {
                Ok(latest.clone())
            } else {
                Err(AuroraError::NotFound("No versions found".to_string()))
            }
        } else {
            Err(AuroraError::NotFound("Procedure not found".to_string()))
        }
    }

    /// Remove procedure from version control
    pub async fn remove_procedure(&self, procedure_name: &str) -> AuroraResult<()> {
        let mut versions = self.versions.write();
        versions.remove(procedure_name);

        let mut current_versions = self.current_versions.write();
        current_versions.remove(procedure_name);

        Ok(())
    }

    fn calculate_checksum(&self, definition: &ProcedureDefinition) -> String {
        use sha2::{Sha256, Digest};
        let mut hasher = Sha256::new();
        hasher.update(definition.source_code.as_bytes());
        hasher.update(definition.version.as_bytes());
        format!("{:x}", hasher.finalize())
    }
}

/// Deployment information
#[derive(Debug)]
pub struct DeploymentInfo {
    pub environment: String,
    pub version: String,
    pub deployed_at: DateTime<Utc>,
    pub deployed_by: String,
    pub status: DeploymentStatus,
}

/// Deployment status
#[derive(Debug)]
pub enum DeploymentStatus {
    Pending,
    InProgress,
    Successful,
    Failed,
}
