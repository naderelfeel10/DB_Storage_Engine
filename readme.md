# Disk-Based DB Storage Engine



A lightweight disk-based relational database storage engine implemented in modern C++, designed to simulate the internal architecture of real-world DBMS systems like PostgreSQL. The engine implements core database components including a Disk Manager, Buffer Pool Manager with LRU replacement, slotted-page storage, persistent table heaps, and tuple serialization. It supports CRUD operations, sequential scans, static hash indexing for O(1) lookups, and B+ Tree indexing for efficient O(log n) queries and range scans. All tables, metadata, and indexes are fully persistent on disk and can be reconstructed after restarting the system. The project demonstrates how modern databases optimize storage, minimize disk I/O, and accelerate query execution through caching and indexing techniques.



## This project implements core database internals including:
1. DiskManager:    handles reading/writing pages to disk.
2. BufferPoolManager (BPM):    caches pages in memory.
3. LRU:    decides which page to evict when memory is full.
4. Page & Tuple:    manage data layout inside pages.
5. TableHeap:    provides table-level abstraction.
6. TableIterator:    enables sequential scans.
7. Static hash index for constant time access O(1)
8. B+ Tree Index for O(logn) time access
9. Volcano-style Query Execution Engine
10. Relational Operators:
    - Sequential Scan
    - Selection
    - Projection
    - Nested Loop Join
    - Indexed Nested Loop Join
    - Hash Join

# System Architecture

<img width="1035" height="457" alt="image" src="https://github.com/user-attachments/assets/4efbddf1-5447-49d2-8149-63662c5d5a21" />
<img width="929" height="643" alt="image" src="https://github.com/user-attachments/assets/75ed3478-a798-47fe-9802-5a0a125ce094" />


## 1. [Buffer](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Buffer)

Implements the Buffer Pool Manager (BPM).
#### Implements Least Recently Used (LRU) page replacement.
### Responsibilities:
1. Cache pages from disk into memory frames
2. Track page usage and pin counts
3. Coordinate with LRU for eviction
4. Sync dirty pages back to disk
## Key Functions:
1. fetchPage(page_id) → Load page into memory
2. newPage() → Allocate a new page
3. deletePage(page_id) → Remove page from system
4. unpinPage(page_id, is_dirty) → Release page usage
5. markAsDirty(page_id) → Mark page for disk write
   
## 2. [Disk](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Storage/Disk)
Handles persistent storage of pages in a file.

### Responsibilities:
1. Manage database file
2. Allocate/deallocate pages
3. Maintain metadata (header, directory)
### Core Structures:
1. DBHeader → File metadata (capacity, sizes)
2. DirectoryEntry → Maps page_id → file offset
3. Key Functions:
4. writePage(page_id, data)
5. readPage(page_id, data)
6. allocatePage() → Get new disk page
7. deletePage(page_id)
8. saveMetaData() / loadMetaData()

## 3. [Page](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Storage/Page)
#### Defines how data is organized inside a single 4KB block.
#### Implements Slotted Page Structure.
#### PageHeader(page_id, next_page_id, tuple_counts, free_space_pointer)
### Key Functions:
1. insertTuple(tuple)
2. getTuple(slot_num, tuple)
3. deleteTuple(slot_num)
4. updateTuple(slot_num, tuple)
5. getData() → raw page access

#### Tuple: Represents a single row in a table. It is a container for multiple Field objects and handles its own serialization into bytes.
#### Field: Represents a single column value, Store typed data (int, string, etc.), Provide serialization support.


## 4. [Table](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Storage/Table)
##### The high-level API used by the user to interact with data.
##### Represents a table stored as a collection of pages.

### TableHeap:
#### Key Functions:
1. insertTuple(tuple) → returns RID
2. updateTuple(rid, tuple)
3. deleteTuple(rid)
4. getTuple(rid)
5. displayTablePages()
6. printColumns()


### TableIterator:
##### Implements sequential table scan.
#### Responsibilities:
1. Iterate through tuples across pages
#### Key Functions:
1. operator++() → move to next tuple
2. operator*() → access current tuple
3. end() → check termination


## 5. [Indexing](https://github.com/naderelfeel10/DB_Storage_Engine/tree/main/src/Storage/Indexing)
#### Static Hash Index for a custom Database Management System.
#### It allows for near-constant time O(1) lookups of records based on specific column values, significantly outperforming linear table scans.

### key functions:

1. hashFunction(Field key): Calculates a bucket index by taking the hash of the field value modulo the current capacity.
2. insertIndex(Field key, RID value): Inserts a new hashEntry into the calculated bucket.
3. getValue(Field key): Searches the specific bucket and its overflow chain for a matching key.
4. updateIndex(Field key, RID value): Finds an existing key in the index and updates its stored Record Identifier (RID).
5. deleteIndex(Field key): Removes a key and its RID from the bucket chain, ensuring the linked list pointers are correctly reconnected.
6. if capacity ratio is >= 0.75 , resize the old index into new one with double size.

-----

## 7. [Query Execution Engine](https://github.com/naderelfeel10/Minimal_DB_Engine/tree/main/src/Q_Execution)

### The engine now includes a modular relational query execution layer inspired by Volcano-style iterator execution used in real DBMS systems.

### Supported Operators
#### 1. Sequential Scan Operator
- Performs full table scans using the TableIterator.

##### Responsibilities:
- Iterate through all tuples in a table
- Provide tuples to upper operators in the execution pipeline
##### Key Functions:
- open() → initialize scan
- getNext(Tuple* tuple) → fetch next tuple
- close() → release scan state
- Ex from the project : select * from User;
  
#### 2. Selection Operator
##### Filters tuples using predicates.
##### Responsibilities:
- Apply WHERE conditions
- Return only matching tuples
- Ex from the project : select * User where user_id > 102 and user_id <=107 OR firstName = lastName;
- 
- ##### Key Components:
- Predicate
- ComplexPredicate
- comparison operations (=, >, <, >=, <=)
x
#### 3. Projection Operator
##### Implements column projection.
##### Responsibilities:
- Return only requested columns
- Construct projected tuples dynamically
- Ex from the project : select user_id, firstName from User where user_id > 102 and user_id <=107 OR firstName = lastName;

### 4. Nested Loop Join
#### Implements relational joins using nested iteration.
##### Responsibilities:
- Join two relations using pairwise tuple comparison
- Support conditional joins
- Complexity: O(N × M)
- Ex from the project : select user_id, firstName, Order.* from User join Order on User.user_id = Order.user_id where user_id > 102 and user_id <=107 OR firstName = lastName;

### 5. Indexed Nested Loop Join
#### Optimized join operator using indexes.
##### Responsibilities:
- Use Hash Index or B+ Tree Index for inner-table lookups
- Avoid full scans on join operations
- Complexity: O(N × lookup_cost)
##### Advantages:
- Much faster than regular nested loop joins
- Simulates index-assisted joins used in production DBMS systems
- Ex from the project : select user_id, firstName, Order.* from User join Order on User.user_id = Order.user_id where user_id > 102 and user_id <=107 OR firstName = lastName;

### 6. Hash Join
#### Implements hash-based relational joins.
#### Responsibilities:
- Build a hash table on the smaller relation
- Probe matching tuples from the second relation
#### Internal Structure:
- unordered_map<Field, vector<Tuple>>
- Complexity: Average Case: O(N + M)
#### Advantages:
- Efficient equality joins
- Reduces repeated scans
- Simulates modern analytical query execution

## BenchMarks on Join Algorithms :
<img width="1085" height="565" alt="image" src="https://github.com/user-attachments/assets/dc2aa010-208f-44ef-b2af-819c6fc813de" />
<img width="1222" height="881" alt="image" src="https://github.com/user-attachments/assets/2834453e-6aa2-4539-915e-a5c29d8ad0da" />
<img width="1183" height="867" alt="image" src="https://github.com/user-attachments/assets/b93aa814-934e-4ee8-b5c7-39ef50ec92b3" />

--- 

# 7. Persistence & Database Recovery
### Fully Persistent Disk-Based Storage
### The storage engine is fully persistent.
### All database components including:

- Tables
- Tuples
- Page layouts
- Metadata
- Static Hash Indexes
- B+ Tree Indexes

### are serialized and stored on disk pages inside the database file.

### After restarting the application, the entire database can be reconstructed directly from disk without rebuilding indexes or reinserting records.

## Persistence Architecture
### 1. Table Metadata Serialization
#### Each TableHeap stores metadata including:
- void saveMetaData();
- void loadMetaData();

The metadata allows the engine to reconstruct table state after restarting the DBMS.

## 2. Persistent Static Hash Index

The Static Hash Index is fully serialized into disk pages.

### Each hash bucket entry stores:
- void serializeHashIndex(char* data);
- void deserializeHashIndex(char* data);

This allows hash indexes to survive process termination and reload instantly on startup.

## 3. Persistent B+ Tree Index

The B+ Tree implementation persists every tree node to disk pages.

### Each node stores:
- void saveBPlusTree();
- void saveNode(Node* node);
- void loadBPlusTree();
- Node* loadNode(int page_id);

Internal nodes and leaf nodes are recursively reconstructed from disk pages during database startup.

### This simulates how production-grade database systems persist index structures.

## Database Reload Workflow

### When the engine starts:

### 1. Open Existing Database File
DiskManager* dm = new DiskManager(DB_name);

#### The DiskManager loads:

database headers
table directory
page mappings

#### 2. Reload TableHeap Objects
   TableHeap* loaded_table = new TableHeap(BPM, dm->tables_names["User"], -1);
   loaded_table->loadMetaData();

##### The table reconstructs:
schema
page chain
tuple boundaries
index metadata

#### 3. Reload Indexes Automatically
for(auto& [col_name, indexes_vec] : loaded_table->indexes_map)

##### The engine dynamically restores:
- Static Hash Indexes
- B+ Tree Indexes

### After loading:

- all tuples are accessible
- indexes remain functional
- CRUD operations continue normally


## Near Future Roadmap
### Query Executer :
- sort-merge join
- aggregiation operators
- external merge sort operator

## Build & Development
Prerequisites
C++17 Compiler (GCC/Clang)

Standard Template Library (STL)

### Compilation :
1. insert some data through test_multiple_tables.c++ in test dir :

g++ -std=c++17 -static \src\Buffer\BufferPoolManager.c++ \src\Buffer\LRU_replacement.c++ \src\Storage\Disk\DiskManager.c++ \src\Storage\Indexing\BPlusTreeIndex.c++ \src\Storage\Indexing\BPlusTreeIndexWrapper.c++ \src\Storage\Indexing\static_hash_index.c++ \src\Storage\Indexing\StaticHashIndexWrapper.c++ \src\Storage\Page\Field.c++ \src\Storage\Page\page.c++ \src\Storage\Page\Tuple.c++ \src\Storage\Table\Column.c++ \src\Storage\Table\RID.c++ \src\Storage\Table\TableHeap.c++ \src\Storage\Table\TableIterator.c++ \test\test_table_load_store.c++ \test\test_multiple_tables.c++ -g -o test_multiple_tables.exe

2. then test loaded file using : test_loading_DB.c++ inside test dir:

g++ -std=c++17 -static \src\Buffer\BufferPoolManager.c++ \src\Buffer\LRU_replacement.c++ \src\Storage\Disk\DiskManager.c++ \src\Storage\Indexing\BPlusTreeIndex.c++ \src\Storage\Indexing\BPlusTreeIndexWrapper.c++ \src\Storage\Indexing\static_hash_index.c++ \src\Storage\Indexing\StaticHashIndexWrapper.c++ \src\Storage\Page\Field.c++ \src\Storage\Page\page.c++ \src\Storage\Page\Tuple.c++ \src\Storage\Table\Column.c++ \src\Storage\Table\RID.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\TableHeap.c++ \src\Storage\Table\TableIterator.c++ \test\test_table_load_store.c++  \test\test_loading_DB.c++ -g -o test_loading_DB.exe 
