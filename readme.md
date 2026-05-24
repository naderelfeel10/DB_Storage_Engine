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
---- 
# System Architecture

# <img width="623" height="620" alt="image" src="https://github.com/user-attachments/assets/6fad8a37-b5b0-4a63-ac1a-0467e01892a3" />


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

# 6. [Testing&Examples](https://github.com/naderelfeel10/DB_Storage_Engine/blob/main/src/Storage/testTable_iterator.c%2B%2B)
## 1.Create DataBase
#### ->query : "create database testDB"
####   1. Disk Initialization: The DiskManager creates a physical file named testDB on the storage drive.
   <img width="615" height="200" alt="image" src="https://github.com/user-attachments/assets/2c2f045d-174c-4322-88fa-2f21bcb1dfe6" />
   
## 2.CREATE TABLE
####  -> query : "create table User(user_id int, firstName varchar(30),lastName varchar(30), age int)"
####     1. define the Cols used and pass them to TableHeap associated with the name of the file 
####    <img width="1000" height="70" alt="image" src="https://github.com/user-attachments/assets/9fe80a38-c2d0-4948-958b-71497be12b61" />
####    <img width="1000" height="300" alt="image" src="https://github.com/user-attachments/assets/f1ae2603-c0d6-4e90-850a-aa03f0fc35ac" />
####    <img width="1000" height="300" alt="image" src="https://github.com/user-attachments/assets/489385c4-0cab-46e1-b392-c0d700bf6b4a" />

## 3.INSERT INTO
####   -> query : "insert into User(user_id, firstName, lastName, age) values(0, \"nader\", \"elfeel\", 21.8);"
####       1. Tuple Construction: Data is packed into a Tuple object containing several Field objects.
####       2. Slotted-Page Write: The TableHeap looks for a page with enough free space. The tuple is serialized into a byte array and placed at the end of  the page, while a "slot" (offset + size) is added to the page header.
 ####     3. Persistence: The BufferPoolManager marks the page as dirty. It will eventually be written to disk by the DiskManager.
 ####     <img width="1173" height="381" alt="image" src="https://github.com/user-attachments/assets/7d6257f4-3c98-4d08-8d0a-bd6b8cc5b25b" />
      
## 4. SELECT * WHERE
####   -> query : "select * from User where user_id=7"
####      1. Table Scan: A TableIterator starts at the first RID (Record ID) and sequentially moves through all pages.
####      <img width="835" height="500" alt="image" src="https://github.com/user-attachments/assets/eb27fec9-af98-48f9-9e11-6bca74c01f4d" />
####      <img width="395" height="476" alt="image" src="https://github.com/user-attachments/assets/3455ffae-7a34-4356-be8a-d76122013056" />
      
## 5. UPDATE SET
####   -> query : "update User firstName = \"GOAT\" where user_id=5"
###   Processing:
####      1. Locate: The iterator scans the table to find the record where user_id == 5.
####      2. In-Place Modification: The code creates a new vector<Field>, replaces the old firstName field at index 1 with a new "GOAT" field, and constructs a new_tuple.
####      3. Heap Update: table_heap->updateTuple is called. If the new tuple is a different size, the Slotted-Page must shift other records to maintain data integrity.
<img width="910" height="486" alt="image" src="https://github.com/user-attachments/assets/aaf2388a-2265-43d1-85dc-f67761c5980e" />
<img width="340" height="347" alt="image" src="https://github.com/user-attachments/assets/b655a9b7-f9e6-4f76-9532-36a23e36808c" />

## 6. DELETE FROM
####   -> query : "delete from User where user_id=8"
####   1. The iterator finds the RID for the record where user_id == 8, then soft delete it by editing it's is_deleted flag.
####     <img width="899" height="542" alt="image" src="https://github.com/user-attachments/assets/e0ab88b1-a946-4428-8e2a-1756d77fd432" />
####     <img width="360" height="358" alt="image" src="https://github.com/user-attachments/assets/d171d12a-02da-4c00-aa9d-fbdfdd62ab1f" />


## 7. Hash indexing 
####    -> query = "CREATE INDEX idx_stu_id_hash ON User USING HASH (stu_id)"
####    using static hash index we search over the hash entries in O(1)
<img width="871" height="191" alt="image" src="https://github.com/user-attachments/assets/bd17214b-9c68-487a-8c9b-9ff64e19fca0" />

## 8. Linear scan vs static hash index in time complexity :
### using linear scan on only 10 entries it took 2679 microseconds:

<img width="1470" height="332" alt="image" src="https://github.com/user-attachments/assets/fb82adf0-73e3-4a04-b268-cdc65f199dc1" />
<img width="663" height="152" alt="image" src="https://github.com/user-attachments/assets/bde4dcd4-c5a9-446f-9bda-c699da653fe7" />

### using hash index  on same 10 entries it took 279 microseconds

<img width="1464" height="524" alt="image" src="https://github.com/user-attachments/assets/ffab5a89-b80b-4010-8954-c35ad127843c" />

## 9. B+ Tree Indexing 
####    -> query = "CREATE INDEX idx_stu_id_btree ON User USING BTREE (stu_id);"
####   using B+ tree index with O(logn) time complexity
<img width="832" height="312" alt="image" src="https://github.com/user-attachments/assets/3bb2cf7f-6a1f-48db-beaf-a4a6c625f7d0" />

## 10. B+tree vs Hash index in time complexity
### 1. searching for user_id = 450 , out of 500 row
<img width="1452" height="325" alt="image" src="https://github.com/user-attachments/assets/6d8aed6a-5965-403d-9c1d-2d891ea4b989" />
<img width="1363" height="331" alt="image" src="https://github.com/user-attachments/assets/573368c8-cf62-4891-8388-5af18f78c836" />

#### we can clearly see that hash index if faster in exact queries [O(1) vs O(logn)], (163Ms vs 211Ms)
<img width="1593" height="482" alt="image" src="https://github.com/user-attachments/assets/26c6defb-5d17-4a7f-9f77-b2b4d8524ac1" />
<img width="1607" height="452" alt="image" src="https://github.com/user-attachments/assets/1b0b8f42-ca1b-47bf-b96f-3b83f1c8e711" />

### 1. searching for the range [4:400] user_id , out of 500 rows
<img width="1310" height="433" alt="image" src="https://github.com/user-attachments/assets/1e816167-6e79-47b7-be1f-35c1b3616623" />
<img width="1406" height="357" alt="image" src="https://github.com/user-attachments/assets/7b62c9e7-bda0-4596-898a-3139745149e1" />

## here comes the advantage of B+ trees , with O( logn + k(range size) ), with sequetional access , vs hash index with O(k) , but all are random access wich makes it slower in range queries. (~86,000Ms, 80,000Ms ).
<img width="1035" height="373" alt="image" src="https://github.com/user-attachments/assets/41bcdbaa-9fc9-43d9-ae57-0d7a1f8c49ce" />


# 11. Persistence & Database Recovery
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
### Query Executor


## Build & Development
Prerequisites
C++17 Compiler (GCC/Clang)

Standard Template Library (STL)

### Compilation :
1. insert some data through test_multiple_tables.c++ in test dir :

g++ -std=c++17 -static D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Buffer\BufferPoolManager.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Buffer\LRU_replacement.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Disk\DiskManager.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\BPlusTreeIndex.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\BPlusTreeIndexWrapper.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\static_hash_index.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\StaticHashIndexWrapper.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Field.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\page.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Tuple.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\Column.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\RID.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\TableHeap.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\TableIterator.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\test\test_table_load_store.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\test\test_multiple_tables.c++ -g -o test_multiple_tables.exe

2. then test loaded file using : test_loading_DB.c++ inside test dir:

g++ -std=c++17 -static D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Buffer\BufferPoolManager.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Buffer\LRU_replacement.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Disk\DiskManager.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\BPlusTreeIndex.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\BPlusTreeIndexWrapper.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\static_hash_index.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Indexing\StaticHashIndexWrapper.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Field.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\page.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Tuple.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\Column.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\RID.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\TableHeap.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\TableIterator.c++ D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\test\test_table_load_store.c++  D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\test\test_loading_DB.c++ -g -o test_loading_DB.exe 
