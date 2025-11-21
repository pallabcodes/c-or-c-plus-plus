//! Page Management and Buffer Pool Implementation

use crate::core::*;
use std::collections::{HashMap, VecDeque};
use std::sync::Arc;
use parking_lot::RwLock;

/// Page in memory representation
#[derive(Debug, Clone)]
pub struct Page {
    pub id: PageId,
    pub data: Vec<u8>,
    pub is_dirty: bool,
    pub pin_count: usize,
    pub last_access: u64,
    pub ref_count: usize,
}

/// Buffer pool manager with LRU replacement
pub struct BufferPool {
    frames: Vec<RwLock<Option<Page>>>,
    page_table: RwLock<HashMap<PageId, usize>>,
    free_frames: RwLock<VecDeque<usize>>,
    lru_list: RwLock<VecDeque<PageId>>,
    config: BufferPoolConfig,
    stats: Arc<RwLock<BufferPoolStats>>,
}

/// Buffer pool configuration
#[derive(Debug, Clone)]
pub struct BufferPoolConfig {
    pub num_frames: usize,
    pub page_size: usize,
    pub max_dirty_pages: usize,
    pub enable_prefetch: bool,
    pub prefetch_distance: usize,
}

/// Buffer pool statistics
#[derive(Debug, Clone, Default)]
pub struct BufferPoolStats {
    pub total_reads: u64,
    pub total_writes: u64,
    pub cache_hits: u64,
    pub cache_misses: u64,
    pub evictions: u64,
    pub flushes: u64,
    pub prefetches: u64,
}

impl BufferPool {
    pub fn new(config: BufferPoolConfig) -> Self {
        let mut frames = Vec::with_capacity(config.num_frames);
        let mut free_frames = VecDeque::with_capacity(config.num_frames);

        for i in 0..config.num_frames {
            frames.push(RwLock::new(None));
            free_frames.push_back(i);
        }

        Self {
            frames,
            page_table: RwLock::new(HashMap::new()),
            free_frames: RwLock::new(free_frames),
            lru_list: RwLock::new(VecDeque::new()),
            config,
            stats: Arc::new(RwLock::new(BufferPoolStats::default())),
        }
    }

    pub async fn get_page(&self, page_id: PageId) -> AuroraResult<Arc<RwLock<Page>>> {
        if let Some(frame_id) = self.page_table.read().get(&page_id).cloned() {
            let frame = &self.frames[frame_id];
            let mut page_guard = frame.write();

            if let Some(ref mut page) = *page_guard {
                page.last_access = self.current_timestamp();
                page.ref_count += 1;

                let mut stats = self.stats.write();
                stats.cache_hits += 1;

                self.update_lru(page_id);

                return Ok(Arc::new(RwLock::new(page.clone())));
            }
        }

        self.load_page(page_id).await
    }

    async fn load_page(&self, page_id: PageId) -> AuroraResult<Arc<RwLock<Page>>> {
        let mut stats = self.stats.write();
        stats.cache_misses += 1;

        let frame_id = self.find_free_frame().await?;
        let page_data = self.load_page_from_disk(page_id).await?;

        let page = Page {
            id: page_id,
            data: page_data,
            is_dirty: false,
            pin_count: 1,
            last_access: self.current_timestamp(),
            ref_count: 1,
        };

        {
            let frame = &self.frames[frame_id];
            let mut frame_guard = frame.write();
            *frame_guard = Some(page.clone());
        }

        self.page_table.write().insert(page_id, frame_id);
        self.lru_list.write().push_back(page_id);

        Ok(Arc::new(RwLock::new(page)))
    }

    async fn find_free_frame(&self) -> AuroraResult<usize> {
        if let Some(frame_id) = self.free_frames.write().pop_front() {
            return Ok(frame_id);
        }

        self.evict_page().await
    }

    async fn evict_page(&self) -> AuroraResult<usize> {
        let mut lru_list = self.lru_list.write();
        let mut page_table = self.page_table.write();

        while let Some(page_id) = lru_list.front().cloned() {
            if let Some(frame_id) = page_table.get(&page_id).cloned() {
                let frame = &self.frames[frame_id];
                let page_guard = frame.read();

                if let Some(ref page) = *page_guard {
                    if page.pin_count == 0 {
                        lru_list.pop_front();

                        if page.is_dirty {
                            self.flush_page(page_id).await?;
                        }

                        page_table.remove(&page_id);

                        let mut stats = self.stats.write();
                        stats.evictions += 1;

                        return Ok(frame_id);
                    }
                }
            }
            lru_list.pop_front();
        }

        Err(AuroraError::Storage("No evictable pages found".to_string()))
    }

    async fn load_page_from_disk(&self, _page_id: PageId) -> AuroraResult<Vec<u8>> {
        Ok(vec![0u8; self.config.page_size])
    }

    async fn flush_page(&self, page_id: PageId) -> AuroraResult<()> {
        let mut stats = self.stats.write();
        stats.flushes += 1;
        Ok(())
    }

    fn update_lru(&self, page_id: PageId) {
        let mut lru_list = self.lru_list.write();

        if let Some(pos) = lru_list.iter().position(|&id| id == page_id) {
            lru_list.remove(pos);
        }

        lru_list.push_back(page_id);
    }

    fn current_timestamp(&self) -> u64 {
        std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs()
    }

    pub fn stats(&self) -> BufferPoolStats {
        self.stats.read().clone()
    }
}