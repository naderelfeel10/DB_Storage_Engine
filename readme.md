Minimalistic Disk-Based Storage Manager
A high-performance C++ storage engine implementing a Slotted-Page architecture with a Buffer Pool Manager (BPM) to bridge the gap between volatile memory and persistent disk storage.

# Key Features
Slotted-Page Structure: Efficiently manages variable-length tuples within fixed-size 4KB pages.

Buffer Pool Management: Implements a frame-based cache to minimize Disk I/O, utilizing pin_count and is_dirty flags for data integrity.

Disk Manager: Handles physical file serialization, metadata persistence, and page-to-offset mapping.

Schema Flexibility: Supports multiple data types (INT, FLOAT, STRING, BOOL) using a compact union-based Field class.

# System Architecture
The storage manager is built in four distinct layers:

## Fields → Tuples → Slotted-Pages → Disk Management → Buffer Management.


1. The Data Layer (Field, Tuple):
   
  Field: A variant-like class using a union to store data. It supports serialization into raw byte buffers.

  Tuple: A collection of Fields. It tracks its own size and deletion status.

2. The Page Layer (Page)
Uses the Slotted-Page contains the Tuples design to handle fragmentation

Header: Stores metadata like free_space_pointer and num_tuples.

Slots: An array at the start of the page pointing to the actual data at the end of the page. This allows tuples to be moved or resized within a page without changing their SlotID.

3. The Disk Layer (DiskManager)
Manages the .db file on disk.

Page Directory: Maps logical page_id to physical file offsets.

Space Management: Tracks deleted pages and reuses them for new allocations to prevent file bloat.


4. The Buffer Layer (BufferPoolManager)
The "Brain" of the memory system.

Frames: A fixed array of memory buffers

Replacement Policy: Provides the infrastructure for LRU eviction.

Dirty Tracking: Ensures modified pages are flushed back to the DiskManager only when necessary.


# Development
## Prerequisites
C++17 or higher

Standard Library (STL)

## Future Roadmap
[ ] LRU Eviction Policy: Full implementation of the findVictim() logic.

[ ] Concurrency: Multi-threaded access to the Buffer Pool using std::mutex.

[ ] B+ Tree Indexing: To allow O(logn) searching instead of full table scans.
