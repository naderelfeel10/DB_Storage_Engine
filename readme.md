# Disk-Based DB Storage Engine

##### A lightweight C++ database storage engine that demonstrates how data moves between memory and disk. It implements core components like a Buffer Pool Manager, Disk Manager, LRU replacement, and slotted-page storage. The project provides table-level abstractions with tuple operations and iteration, mimicking real database internals.

## This project implements core database internals including:
1. DiskManager:    handles reading/writing pages to disk.
2. BufferPoolManager (BPM):    caches pages in memory.
3. LRU:    decides which page to evict when memory is full.
4. Page & Tuple:    manage data layout inside pages.
5. TableHeap:    provides table-level abstraction.
6. TableIterator:    enables sequential scans.

---- 
# System Architecture

<img width="758" height="467" alt="image" src="https://github.com/user-attachments/assets/6942c2ec-85d8-404b-9b49-d963fbba99c0" />


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

-----

# Testing& Examples 
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


## Near Future Roadmap
#### [ ] B+ Tree Indexing: To allow O(logn) searching instead of full table scans.
#### [ ] Hashing Indexing: for a O(1) scans 


## Build & Development
Prerequisites
C++17 Compiler (GCC/Clang)

Standard Template Library (STL)

### Compilation :
g++ -std=c++17 -static Storage/testTable_iterator.c++  Storage/Table/TableIterator.c++ Storage/Table/TableHeap.c++ Storage/Table/RID.c++ Buffer/LRU_replacement.c++ Buffer/BufferPoolManager.c++ Storage/Disk/DiskManager.c++ Storage/Page/page.c++  Storage/Page/Field.c++ Storage/Table/Column.c++ Storage/Page/Tuple.c++ -g -o testTableIterator.exe

