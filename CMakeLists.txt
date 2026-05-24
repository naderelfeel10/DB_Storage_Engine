CXX = g++
CXXFLAGS = -std=c++17 -static -g

SRC = \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Buffer/BufferPoolManager.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Buffer/LRU_replacement.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Disk/DiskManager.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Indexing/BPlusTreeIndex.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Indexing/BPlusTreeIndexWrapper.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Indexing/static_hash_index.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Indexing/StaticHashIndexWrapper.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Page/Field.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Page/page.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Page/Tuple.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Table/Column.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Table/RID.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Table/TableHeap.c++ \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/src/Storage/Table/TableIterator.c++

TEST_LOAD_STORE = \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/test/test_table_load_store.c++

TEST_LOADING_DB = \
D:/SWE/DB/CMU/MY_DB_ENGINE/Minimal_DB_ENGINE/test/test_loading_DB.c++

all: test_table_load_store.exe test_loading_DB.exe

test_table_load_store.exe:
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST_LOAD_STORE) -o test_table_load_store.exe

test_loading_DB.exe:
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST_LOADING_DB) -o test_loading_DB.exe

clean:
	del /Q *.exe