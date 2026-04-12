# Disk-Based DataBase Storage Engine

#### a custom-built, persistent storage engine written in C++17.
#### It bridges the gap between raw byte storage and relational data management by implementing a Slotted-Page architecture, Disk Manager and a Buffer Pool Manager.
#### It is designed to handle variable-length records with high efficiency while minimizing disk I/O through an optimized LRU eviction policy.
#### CRUD operations on tuples using TableHeap & TableIterator for linear search.

# System Architecture

<img width="758" height="467" alt="image" src="https://github.com/user-attachments/assets/6942c2ec-85d8-404b-9b49-d963fbba99c0" />


## 1. [Buffer](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Buffer)
This folder handles the "Virtual Memory" of the database. It keeps frequently accessed pages in RAM so the system doesn't have to read from the disk constantly.

BufferPoolManager: The orchestrator. It fetches pages from the disk and places them into memory "frames." It tracks which pages are modified (is_dirty) and which are currently in use (pin_count).

LRU_replacement: The eviction policy. When the buffer is full, this module uses a Least Recently Used algorithm (implemented with a Doubly Linked List and Hash Map) to decide which page to remove to make room for new data.

## 2. [Disk](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Storage/Disk)
The lowest layer of the system that talks directly to the Operating System.

DiskManager: Handles the physical .db file. It maps logical PageIDs to actual byte offsets on your hard drive and performs the raw read and write operations.

## 3. [Page](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Storage/Page)
Defines how data is organized inside a single 4KB block.

Page (Slotted-Page): Instead of writing data sequentially, it uses a "Slot Directory" at the header. This allows you to move tuples around or resize them without breaking the pointers used by the rest of the database.

Tuple: Represents a single row in a table. It is a container for multiple Field objects and handles its own serialization into bytes.
Field: The very basic building block of the table (has type, value and some meta data)

## 4. [Table](https://github.com/naderelfeel10/DB_storage_manager/tree/main/src/Storage/Table)
The high-level API used by the user to interact with data.

TableHeap: Represents the physical table. It manages a linked list of pages and provides the API for insert, update, and delete.

TableIterator: A pointer-like object that lets you step through every record in the table using ++ syntax, automatically jumping from one page to the next when it reaches the end of a slot directory.

Column & RID: Column defines the schema (name and type), while RID (Record ID) is the unique "GPS coordinate" (PageID + SlotID) for every row.

# Examples

## 1.Create DataBase
#### ->query : "create database testDB"
####   1. Disk Initialization: The DiskManager creates a physical file named testDB on the storage drive.
   <img width="615" height="200" alt="image" src="https://github.com/user-attachments/assets/2c2f045d-174c-4322-88fa-2f21bcb1dfe6" />
   
## 2.CREATE TABLE
####  -> query : "create table User(user_id int, firstName varchar(30),lastName varchar(30), age int)"
####      1. define the Cols used and pass them to TableHeap associated with the name of the file 
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

