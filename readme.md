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


1. [The Data Layer (Field, Tuple)](https://github.com/naderelfeel10/DB_storage_manager/blob/main/src/storage/Tuple.c%2B%2B):
   
     Field: A variant-like class using a union to store data. It supports serialization into raw byte buffers.

     Tuple: A collection of Fields. It tracks its own size and deletion status.


2. [The Page Layer (Page)](https://github.com/naderelfeel10/DB_storage_manager/blob/main/src/storage/page.c%2B%2B):
   
   Uses the Slotted-Page contains the Tuples design to handle fragmentation

   Header: Stores metadata like free_space_pointer and num_tuples.

   Slots: An array at the start of the page pointing to the actual data at the end of the page. This allows tuples to be moved or resized within a page            without changing their SlotID.


4. [The Disk Layer (DiskManager)](https://github.com/naderelfeel10/DB_storage_manager/blob/main/src/storage/DiskManager.c%2B%2B):
   
   Manages the .db file on disk.

   Page Directory: Maps logical page_id to physical file offsets.

   Space Management: Tracks deleted pages and reuses them for new allocations to prevent file bloat.


4. [The Buffer Layer (BufferPoolManager)](https://github.com/naderelfeel10/DB_storage_manager/blob/main/src/storage/BufferPoolManager.c%2B%2B):
   
   The "Brain" of the memory system.

   Frames: A fixed array of memory buffers

   Replacement Policy: Provides the infrastructure for LRU Replacement to evict the least used Frame.

   Dirty Tracking: Ensures modified pages are flushed back to the DiskManager only when necessary.



5. [LRU Replacement](https://github.com/naderelfeel10/DB_storage_manager/blob/main/src/storage/LRU_replacement.c%2B%2B):
   Algorithm: Implements O(1) Least Recently Used policy using a std::unordered_map and a Doubly Linked List.
   
   Buffer Integration: Manages Frame ID eviction for the BufferPoolManager to balance memory and disk I/O.Safety: Uses Sentinel Nodes (D_head/D_tail) to       eliminate null pointer crashes during high-frequency pointer re-linking.
   
   Efficiency: Optimized for in-place updates, avoiding expensive heap re-allocations when "hitting" existing frames.
   
   API: Provides put_frame, get_frame, evict_frame, and remove_frame for full lifecycle management of buffer slots.


# Development

## Prerequisites
C++17 or higher

Standard Library (STL)

## Future Roadmap

[ ] Concurrency: Multi-threaded access to the Buffer Pool using std::mutex.

[ ] B+ Tree Indexing: To allow O(logn) searching instead of full table scans.
