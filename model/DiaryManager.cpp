#include "DiaryManager.h"
#include <algorithm> // Required for std::fill (used in secure deletion)

// ==================================================================================
// HELPER METHODS
// ==================================================================================

// Internal Helper: FindEntryById
// WHY: We need to find entries in both read-only (const) and modifiable contexts.
// avoiding code duplication reduces bugs.
// PERFORMANCE: This is a linear search O(N). For < 1000 entries, this is faster
// than a HashMap due to CPU cache locality (vectors are contiguous in RAM).
DiaryEntry* DiaryManager::findEntryById(const std::string& id) {
    for (auto& entry : entries) {
        if (entry.id == id) return &entry;
    }
    return nullptr;
}

// ==================================================================================
// CORE FEATURES
// ==================================================================================

// 1. Create Entry
// GOAL: Add a new entry to the RAM database safely and efficiently.
std::string DiaryManager::createEntry(const std::string &title, const std::string &content) {
    // VALIDATION: Fail fast if data is invalid.
    if (title.empty() || content.empty()) return "";

    // PERFORMANCE: emplace_back vs push_back
    // 'push_back' creates a temporary object, copies it into the vector, then destroys the temp.
    // 'emplace_back' builds the object DIRECTLY inside the vector's memory.
    // This saves 1 construction + 1 destruction cycle.
    DiaryEntry& entry = entries.emplace_back(title, content);

    // LOGIC: Set Metadata
    // In a real app, generateUUID() would create a unique string like "550e8400-e29b..."
    entry.id = "UUID_PLACEHOLDER";
    entry.createdAt = 1000;        // Placeholder timestamp
    entry.updatedAt = 1000;

    // SAFETY CRITICAL: Return the ID, not the Pointer!
    // If we returned '&entry', and the vector later resized (grew), that pointer
    // would point to deleted memory (Dangling Pointer).
    // The ID (std::string) is a safe "Handle" to find the data later.
    return entry.id;
}

// 2. Read Summaries
// GOAL: Provide a lightweight list for the UI (ListView).
// The UI doesn't need the full content (which could be huge), just the title/date.
std::vector<DiaryEntrySummary> DiaryManager::readEntrySummaries() const{
    std::vector<DiaryEntrySummary> summaries;

    // PERFORMANCE: Reserve Memory
    // We know exactly how many items we need. 'reserve' allocates all memory at once.
    // Without this, the vector might resize multiple times as we add items, which is slow.
    summaries.reserve(entries.size());

    for (const auto& entry : entries) {
        // SAFETY: We copy data into a new 'Summary' struct.
        // We use std::string (Deep Copy) instead of string_view (Pointer).
        // Why? If the main 'entries' vector is modified while the UI is reading this summary,
        // a string_view would point to garbage. std::string ensures the UI has its own safe copy.
        summaries.push_back({entry.id, entry.title, entry.createdAt, entry.updatedAt, entry.bookmarked});
    }
    return summaries;
}

// 3. Read Single Entry
// GOAL: Get the full data for the Editor.
const DiaryEntry* DiaryManager::readEntry(const std::string& id) const noexcept {
    // REUSE: Use our helper to find the entry.
    // We explicitly cast away const-ness internally, but return a const pointer
    // so the user cannot accidentally modify the entry directly.
    // (In this specific implementation, we just duplicate the loop for const-correctness simplicity)
    for (const auto& entry : entries) {
        if (entry.id == id) {
            return &entry;
        }
    }
    return nullptr;
}

// 4. Update Entry
// GOAL: Modify an existing entry.
DiaryError DiaryManager::updateEntry(const std::string& id, const std::string& title, const std::string& content) {
    if (id.empty()) return DiaryError::EmptyId;
    if (title.empty()) return DiaryError::EmptyTitle;

    // LOCATE: Find the object in RAM.
    DiaryEntry* entry = findEntryById(id);

    if (entry) {
        // UPDATE: Modify the fields.
        entry->title = title;
        entry->content = content;
        entry->updatedAt = 2000; // Mock timestamp update
        return DiaryError::None;
    }

    return DiaryError::EntryNotFound;
}

// 5. Delete Entry
// GOAL: Remove an entry and ensuring the data is truly gone (Sanitization).
DiaryError DiaryManager::deleteEntry(const std::string &id) {
    if (id.empty()) return DiaryError::EmptyId;

    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->id == id) {
            // SECURITY: "Manual Zeroing" (The Scrubbing)
            // Problem: When you delete a string, the OS marks the memory as "free"
            // but does NOT wipe the data. A hacker dumping RAM could see the deleted diary text.

            // Solution: We force-overwrite the memory with zeros before deletion.
            // 'volatile' tells the compiler: "Do not optimize this away, actually do it!"
            volatile char* p = (volatile char*)it->content.data();
            std::fill(p, p + it->content.size(), 0);

            // CLEANUP: Now remove the empty shell from the vector.
            entries.erase(it);
            return DiaryError::None;
        }
    }
    return DiaryError::EntryNotFound;
}
