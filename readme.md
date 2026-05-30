# Minimal Database Engine

A lightweight disk-based relational database storage engine implemented in modern C++, designed to simulate the internal architecture of real-world DBMS systems like PostgreSQL. The engine implements core database components including a Query Executer, Disk Manager, Buffer Pool Manager with LRU replacement, slotted-page storage, persistent table heaps, and tuple serialization. It supports CRUD operations, sequential scans, static hash indexing for O(1) lookups, and B+ Tree indexing for efficient O(log n) queries and range scans. All tables, metadata, and indexes are fully persistent on disk and can be reconstructed after restarting the system. The project demonstrates how modern databases optimize storage, minimize disk I/O, and accelerate query execution through caching and indexing techniques. Built from scratch — no third-party database libraries.

---

## Features at a Glance

| Feature | Details |
|---|---|
| **Storage** | Slotted-page layout, 4KB pages, disk-backed |
| **Buffer Management** | LRU replacement policy, configurable pool size |
| **Indexing** | Static Hash Index `O(1)` + B+ Tree Index `O(log n)` |
| **Query Execution** | Volcano-style iterator model for (seq scan, select, project) |
| **Join Algorithms** | Nested Loop, Indexed Nested Loop, Hash Join |
| **Persistence** | Full crash recovery — tables, indexes, metadata survive restarts |

---

## 🏗️ System Architecture

```
┌──────────────────────────────────────────────────┐
│              Query Execution Engine               │
│   SeqScan │ Select │ Project │ Join │ HashJoin    │
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

---

##  Join Algorithm Benchmarks
| Join Algorithm | Complexity | Benchmark (10,000 Orders × 500 Users) |
|---|---|---|
| **Nested Loop Join** | `O(N × M)` | (~450,000 µs) |
| **Indexed Nested Loop Join** | `O(N × lookup)` | (~300,000 µs) |
| **Hash Join** | `O(N + M)` average | Build + probe hash table (~170,000 µs) |


Hash Join significantly outperforms plain Nested Loop on large datasets, while Indexed Nested Loop shines for selective outer relations.

<img width="1000" height="300" alt="image" src="https://github.com/user-attachments/assets/8d5afea9-4995-49db-8e85-5965ef1233b4" />

---
<img width="1000" height="500" alt="image" src="https://github.com/user-attachments/assets/14dcdd4d-0c4e-4e5a-81b7-9d55aa353857" />

---
<img width="1000" height="500" alt="image" src="https://github.com/user-attachments/assets/5d747429-1ad1-4380-9122-4a5bf4f38040" />

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

##  Roadmap

- [ ] Sort-Merge Join
- [ ] Aggregation operators (`COUNT`, `SUM`, `AVG`, `GROUP BY`)
- [ ] External Merge Sort

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
