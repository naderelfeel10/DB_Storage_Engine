# Database Engine

A disk-based relational database engine implemented in modern C++, designed to simulate the internal architecture of real-world DBMS systems like PostgreSQL. The engine implements core database components including a Query Executer, Disk Manager, Buffer Pool Manager with LRU replacement, slotted-page storage, persistent table heaps, and tuple serialization. It supports CRUD operations, sequential scans, hash indexing for O(1) lookups, B+ Tree indexing for efficient O(log n) queries and range scans, and external merge sort for sorting datasets that exceed available memory. All tables, metadata, and indexes are fully persistent on disk and can be reconstructed after restarting the system. The project demonstrates how modern databases optimize storage, minimize disk I/O, and accelerate query execution through caching and indexing techniques. Built from scratch — no third-party database libraries.

---

## Features at a Glance

| Feature | Details |
|---|---|
| **Storage** | Slotted-page layout, 4KB pages, disk-backed |
| **Buffer Management** | LRU replacement policy, configurable pool size |
| **Indexing** | Static Hash Index `O(1)` + B+ Tree Index `O(log n)` |
| **Query Execution** | Volcano-style iterator model for (seq scan, select, project) |
| **Join Algorithms** | Nested Loop, Indexed Nested Loop, Hash Join, Merge-Sort Join |
| **Sorting** | External Merge Sort - sorts datasets larger than memory |
| **Aggregation** | sort based aggregation - groups table rows into one representitive row |
| **Persistence** | Full crash recovery - tables, indexes, metadata survive restarts |

---

## 🏗️ System Architecture

```
┌──────────────────────────────────────────────────┐
│              Query Execution Engine               │
│   SeqScan │ Select │ Project │ Join │ ExMergeSort |
|            | Aggregation                          |
└────────────────────┬─────────────────────────────┘
                     │
┌────────────────────▼─────────────────────────────┐
│                  TableHeap                        │
│         (Table-level CRUD + Iterators)            │
└────────────────────┬─────────────────────────────┘
                     │
┌────────────────────▼─────────────────────────────┐
│             Buffer Pool Manager                   │
│           (LRU Replacement Policy)                │
└────────────────────┬─────────────────────────────┘
                     │
┌────────────────────▼─────────────────────────────┐
│               Disk Manager                        │
│     (Page I/O, Metadata, Directory Entries)       │
└──────────────────────────────────────────────────┘
```
##  Project Structure

```
├── src/
│   ├── Buffer/
│   │   ├── BufferPoolManager.c++
│   │   └── LRU_replacement.c++
│   ├── Storage/
│   │   ├── Disk/
│   │   │   └── DiskManager.c++
│   │   ├── Page/
│   │   │   ├── Field.c++
│   │   │   ├── page.c++
│   │   │   └── Tuple.c++
│   │   ├── Table/
│   │   │   ├── TableHeap.c++
│   │   │   ├── TableIterator.c++
│   │   │   ├── Column.c++
│   │   │   └── RID.c++
│   │   └── Indexing/
│   │       ├── static_hash_index.c++
│   │       ├── StaticHashIndexWrapper.c++
│   │       ├── BPlusTreeIndex.c++
│   │       └── BPlusTreeIndexWrapper.c++
│   └── Q_Execution/
│       ├── AbstractPredicate.h
│       ├── ComplexPredicate.cpp
│       ├── AggregationExecuter.cpp
│       ├── ExternalMergeSortExecuter.cpp
│       ├── MergeJoinExecuter.cpp
│       ├── hash_join.cpp
│       ├── IndexedNested_loop_join.cpp
│       ├── Nested_loop_join.cpp
│       ├── Predicate.cpp
│       ├── Projection_operator.cpp
│       ├── select_operator.cpp
│       ├── seq_scan_operator.cpp
└── test/
    ├── test_multiple_tables.c++
    ├── test_loading_DB.c++
    └── test_table_load_store.c++
```

---

##  Component Breakdown

### 1.  Disk Manager [`src/Storage/Disk`](src/Storage/Disk)

Handles all persistent I/O against the database file.

- Manages database header (`DBHeader`) and page directory (`DirectoryEntry`)
- Maps `page_id → file offset` for random-access reads/writes
- Allocates and reclaims disk pages

```
writePage(page_id, data)   readPage(page_id, data)
allocatePage()             deletePage(page_id)
saveMetaData()             loadMetaData()
```

---

### 2.  Buffer Pool Manager [`src/Buffer`](src/Buffer)

Keeps hot pages in memory to minimize disk I/O.

- Fixed pool of in-memory frames with pin-count tracking
- **LRU eviction** flushes dirty pages automatically on eviction
- Thread-safe dirty-page writeback

```
fetchPage(page_id)         newPage()
unpinPage(page_id, dirty)  deletePage(page_id)
markAsDirty(page_id)
```

---

### 3.  Page & Tuple [`src/Storage/Page`](src/Storage/Page)

Defines the physical layout of a 4 KB storage block.

**Slotted Page Structure**

```
┌─────────────────────────────────────────┐
│  PageHeader (id, next_page, count, fsp) │
├─────────────────────────────────────────┤
│  Slot Array  →                          │
├─────────────────────────────────────────┤
│                  ← Tuple Data           │
└─────────────────────────────────────────┘
```

- **Tuple** — a row; serializes a list of `Field` objects to/from raw bytes
- **Field** — a single typed column value (`int`, `string`, …) with serialization support

---

### 4.  Table Layer [`src/Storage/Table`](src/Storage/Table)

High-level API over a chain of pages.

**TableHeap** provides CRUD and returns Record IDs (`RID = page_id + slot_num`):

```
insertTuple(tuple) → RID    getTuple(rid)
updateTuple(rid, tuple)     deleteTuple(rid)
```

**TableIterator** drives full sequential scans:

```cpp
for (auto it = table.begin(); it != table.end(); ++it) {
    Tuple t = *it;
}
```

---

### 5.  Indexing [`src/Storage/Indexing`](src/Storage/Indexing)

#### Static Hash Index — `O(1)` average lookup

- Chained-bucket hash table persisted across pages
- **Auto-rehash** at 75 % load factor (doubles capacity)
- Full serialization to disk (`serializeHashIndex` / `deserializeHashIndex`)

#### B+ Tree Index — `O(log n)` lookup + range scans

- Classic leaf-linked B+ Tree, every node stored on its own disk page
- Supports equality and range predicates
- Tree persisted with `saveBPlusTree()` / `loadBPlusTree()`

---

### 6.  Query Execution Engine [`src/Q_Execution`](src/Q_Execution)

Volcano-style pull model — each operator exposes `open / getNext / close`.

| Operator | Description | Example |
|---|---|---|
| **Sequential Scan** | Full table scan | `SELECT * FROM User` |
| **Selection** | WHERE predicates (`=`, `>`, `<`, `>=`, `<=`, `AND`, `OR`) | `WHERE user_id > 102 AND user_id <= 107` |
| **Projection** | Column filtering | `SELECT user_id, firstName FROM ...` |
| **Nested Loop Join** | Pairwise scan `O(N×M)` | `JOIN Order ON User.user_id = Order.user_id` |
| **Indexed Nested Loop Join** | Uses Hash/B+ Tree on inner table `O(N × lookup)` | Same join, index-assisted |
| **Hash Join** | Build + probe `O(N+M)` average | Equality joins on large relations |
| **Merge Join** | Build + probe `4*(N+M)` average | equal join on 2 sorted tables |
---

##  Join Algorithm Benchmarks
Join Algorithm Comparison

Benchmarks and analysis of four classic join algorithms implemented from scratch in a custom C++ database engine with a Buffer Pool Manager.
## Test Setup:
```
Users: 10,000 rows
Orders: 1,000,000 rows
Join: equi-join on user_id
Skew: high — 1,000 orders per user (user_id = 100 + (i % 1000))
Buffer Pool: 100 frames
```

## Join Algorithms Benchmark

The following benchmark compares the execution time of different join algorithms implemented in the DB engine.

| Algorithm | Time              | Index Required | Pre-sort Required |
|-----------|-------------------|----------------|-------------------|
| Hash Join | 22 sec | ✗ | ✗ |
| Indexed Nested Loop Join | 46 sec | ✓ (built at query time) | ✗ |
| Sort-Merge Join | 78 sec | ✗ | ✗ |
| Nested Loop Join | too bad | ✗ | ✗ |

### Observations

- **Hash Join** achieved the best performance because it avoids repeated comparisons by using a hash table.
- **Indexed Nested Loop Join** benefits from indexing but has additional overhead because the index is created during query execution.
- **Sort-Merge Join** requires sorting phases, which increases execution time.
- Printing results significantly increases runtime due to console I/O overhead.

Hash Join significantly outperforms plain Nested Loop on large datasets, while Indexed Nested Loop shines for selective outer relations.

---
### 7. External Merge Sort src/Q_Execution
#### Sorts a table on any column (ASC or DESC) using a two-phase disk-based strategy, following the Volcano model so it composes naturally with any upstream operator.
##### How it works
```
Phase 1 — Run Generation
  ┌──────────────────────────────────────────────┐
  │  Fetch tuples from child operator            │
  │  Fill an in-memory buffer (one page at a     │
  │  time), sort it locally, flush to disk       │
  │  → produces N sorted single-page runs        │
  └──────────────────────────────────────────────┘

Phase 2 — Merge Passes
  ┌──────────────────────────────────────────────┐
  │  Pop pairs of runs from the queue            │
  │  Merge each pair into a new sorted run       │
  │  (multi-page, linked via next_page_id)       │
  │  Push merged run back onto the queue         │
  │  Repeat until one run remains                │
  └──────────────────────────────────────────────┘

  Total I/O Complexity: 
  2N*(1+logB−1(N/B))

```
---
##  Persistence & Recovery

All storage artifacts survive process termination and reload cleanly on startup.

```
Database Restart Sequence
─────────────────────────
1. DiskManager opens existing .db file
   └─ Loads headers, directory, page mappings

2. TableHeap.loadMetaData()
   └─ Restores schema, page chain, tuple boundaries

3. Index reload loop
   └─ Static Hash: deserializeHashIndex()
   └─ B+ Tree:     loadBPlusTree() → loadNode()

 All tuples accessible, all indexes functional
```

No manual rebuild step — the engine reconstructs itself entirely from disk pages.

---

## near roadmap

- [ ] Query Optimizer :)
---

##  Build & Run

**Prerequisites:** GCC / Clang with C++17, standard STL only — no external dependencies.

### Step 1 — Insert data

```bash
g++ -std=c++17 -static \
  src/Buffer/BufferPoolManager.c++ \
  src/Buffer/LRU_replacement.c++ \
  src/Storage/Disk/DiskManager.c++ \
  src/Storage/Indexing/BPlusTreeIndex.c++ \
  src/Storage/Indexing/BPlusTreeIndexWrapper.c++ \
  src/Storage/Indexing/static_hash_index.c++ \
  src/Storage/Indexing/StaticHashIndexWrapper.c++ \
  src/Storage/Page/Field.c++ \
  src/Storage/Page/page.c++ \
  src/Storage/Page/Tuple.c++ \
  src/Storage/Table/Column.c++ \
  src/Storage/Table/RID.c++ \
  src/Storage/Table/TableHeap.c++ \
  src/Storage/Table/TableIterator.c++ \
  test/test_table_load_store.c++ \
  test/test_multiple_tables.c++ \
  -g -o test_multiple_tables.exe

./test_multiple_tables.exe
```

### Step 2 — Reload from disk (verify persistence)

```bash
g++ -std=c++17 -static \
  src/Buffer/BufferPoolManager.c++ \
  src/Buffer/LRU_replacement.c++ \
  src/Storage/Disk/DiskManager.c++ \
  src/Storage/Indexing/BPlusTreeIndex.c++ \
  src/Storage/Indexing/BPlusTreeIndexWrapper.c++ \
  src/Storage/Indexing/static_hash_index.c++ \
  src/Storage/Indexing/StaticHashIndexWrapper.c++ \
  src/Storage/Page/Field.c++ \
  src/Storage/Page/page.c++ \
  src/Storage/Page/Tuple.c++ \
  src/Storage/Table/Column.c++ \
  src/Storage/Table/RID.c++ \
  src/Storage/Table/TableHeap.c++ \
  src/Storage/Table/TableIterator.c++ \
  test/test_table_load_store.c++ \
  test/test_loading_DB.c++ \
  -g -o test_loading_DB.exe

./test_loading_DB.exe
```

---


---

*Built to understand how databases actually work — from raw bytes on disk to query execution.*
