# Database Engine

A production-quality, disk-based relational database engine implemented in modern C++, designed to simulate the internal architecture of real-world DBMS systems like PostgreSQL and MySQL. The engine implements a **complete SQL compilation pipeline**, from parsing through semantic analysis to query planning and physical execution.

## 🏗️ Architecture: From SQL String to Result Set

### High-Level Query Flow

```
SQL String
    ↓
┌─────────────────────────────────────────┐
│           Parser (AST Generation)       │  Parse: "SELECT ... FROM ... WHERE ..."
└────────────────┬────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│    Binder (Semantic Analysis)           │  Bind columns, tables, functions to catalog
│  BindContext + Catalog Lookup           │
└────────────────┬────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│    Planner (Logical Plan)               │  Build query tree: SeqScan → Filter → Join
└────────────────┬────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│   ExecutorFactory (Physical Plan)       │  Create operators: predicate builders, joins
└────────────────┬────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│        Query Execution Engine           │  Volcano-style iterator model
│  Operators: SeqScan, Filter, Join, etc. │
└────────────────┬────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│           Storage Layer                 │  TableHeap, IndexLookup, Buffer Pool
│   Buffer Pool → Disk Manager            │
└─────────────────────────────────────────┘
```

---

## 🔧 Core Features

| Feature | Details |
|---------|---------|
| **SQL Parsing** | AST-based parser; supports SELECT, WHERE, JOIN, GROUP BY, HAVING, ORDER BY |
| **Semantic Analysis** | Binder validates column references, table aliases, function signatures |
| **Catalog** | Persistent schema metadata; tables, columns, data types |
| **Query Planning** | Transforms bound statements into logical execution plans |
| **Storage** | Slotted-page layout, 4KB pages, disk-backed persistence |
| **Buffer Management** | LRU replacement policy, configurable pool size, dirty-page tracking |
| **Indexing** | Static Hash Index O(1) + B+ Tree Index O(log n) with range scans |
| **Query Execution** | Volcano-style iterator model for composable operators |
| **Join Algorithms** | Nested Loop, Indexed Nested Loop, Hash Join, Merge-Sort Join |
| **Sorting** | External Merge Sort — sorts datasets larger than memory |
| **Aggregation** | Sort and Hashed based aggregation with GROUP BY / HAVING support |
| **Persistence** | Full crash recovery — tables, indexes, metadata survive restarts |

---

## 📐 System Architecture

### Compilation Pipeline Components

### 1. [Parser](https://github.com/naderelfeel10/Database_Engine/tree/main/parser) (External + Integration)
- **Input**: Raw SQL string
- **Output**: Abstract Syntax Tree (AST)
- **Forked** a production-grade SQL parser; handles statement types, expressions, clauses

### 2. [Binder](https://github.com/naderelfeel10/Database_Engine/tree/main/src/Binder)
**Semantic analysis layer** — validates that parsed SQL refers to real objects.

**Key responsibilities:**
- Resolve column names → (table_oid, column_id)
- Validate table references and aliases
- Type-check operators and function arguments
- Build `BoundExpression` tree (typed, resolved AST)

### 3. [Catalog](https://github.com/naderelfeel10/Database_Engine/tree/main/src/Catalog)
**Schema metadata registry** — persistent storage of table/column definitions.

- Tables: schema, column names, types
- Indexes: index_id → (table_oid, key_column)
- Data types: int, float, string, date, etc.

### 4. [Planner](https://github.com/naderelfeel10/Database_Engine/tree/main/src/QueryPlan)
**Logical query plan generator** — builds operator tree from bound statement.

**Plan node types:**
- `SeqScanPlan` → sequential scan of table
- `FilterPlan` → WHERE clause predicates
- `ProjectionPlan` → SELECT column list
- `JoinPlan` → INNER, LEFT, RIGHT joins
- `OrderByPlan` → ORDER BY sort key + direction
- `GroupByPlan` → GROUP BY keys + aggregate functions + HAVING

**Example plan tree for a complex query:**

```
         PROJECTION [u.user_id, u.firstName, AVG(u.age), SUM(u.age)]
              ↑
         GROUP BY [u.user_id, u.firstName]
              ↑
           HAVING [SUM(u.age) > 70 AND AVG(u.age) >= 30.1]
              ↑
            FILTER [u.age > 18]
              ↑
            JOIN [INNER] [u.user_id = Orders.user_id]
            /          \
       SEQ_SCAN      SEQ_SCAN
         [User]      [Orders]
```

### 5. [ExecutorFactory](https://github.com/naderelfeel10/Database_Engine/tree/main/src/Executer)
**Physical operator instantiation** — converts logical plan to executable iterators.


**Responsibilities:**
- Allocate executor instances from plan nodes
- Build predicate evaluators from expressions
- Map bound columns to physical Column objects
- Wire child→parent iterator chains

---

## 📁 Project Structure

```
src/
├── Binder/
│   ├── Binder.h / Binder.c++
│   ├── BoundStatement.h
│   ├── BoundExpression.h
│   ├── BoundSelectStatement.h
│   ├── BindContext.h
│   └── BoundSelectItem.h
│
├── Catalog/
│   ├── Catalog.h / Catalog.c++
│   └── catalog.db                    (persistent schema)
│
├── QueryPlan/
│   ├── AbstractPlanNode.hpp
│   ├── PlanNodes.hpp                 (SeqScan, Filter, Join, GroupBy, OrderBy, Projection)
│   ├── Planner.h / Planner.c++
│   └── PlanType.h
│
├── Q_Execution/
│   ├── ExecutorFactory.h / ExecutorFactory.c++
│   ├── AbstractExecuter.h
│   ├── Operators/
│   │   ├── seq_scan_operator.c++
│   │   ├── select_operator.c++
│   │   ├── Projection_operator.c++
│   │   ├── Nested_loop_join.c++
│   │   ├── IndexedNested_loop_join.c++
│   │   ├── hash_join.c++
│   │   ├── MergeJoinExecuter.c++
│   │   ├── HashAggregateExecuter.c++
│   │   ├── SortAggregateExecuter.c++
│   │   └── ExternalMergeSortExecuter.c++
│   ├── Predicates/
│   │   ├── AbstractPredicate.h
│   │   ├── Predicate.c++
│   │   └── ComplexPredicate.c++
│   └── Column.h                      (physical column abstraction)
│
├── Buffer/
│   ├── BufferPoolManager.c++
│   └── LRU_replacement.c++
│
├── Storage/
│   ├── Disk/
│   │   └── DiskManager.c++
│   ├── Page/
│   │   ├── Field.c++
│   │   ├── page.c++
│   │   └── Tuple.c++
│   ├── Table/
│   │   ├── TableHeap.c++
│   │   ├── TableIterator.c++
│   │   ├── Column.c++
│   │   └── RID.c++
│   └── Indexing/
│       ├── static_hash_index.c++
│       ├── StaticHashIndexWrapper.c++
│       ├── BPlusTreeIndex.c++
│       └── BPlusTreeIndexWrapper.c++
│
├── parser/
│   └── external/sql-parser/         (forked parser submodule)
│
└── test/
    ├── integration_tests.c++
    ├── test_multiple_tables.c++
    ├── test_loading_DB.c++
    └── test_table_load_store.c++
```

---

## 🔄 End-to-End Query Execution Example

**Query:**
```sql
SELECT u.user_id, u.firstName, AVG(u.age) 
FROM User u 
WHERE u.age > 18 
GROUP BY u.user_id, u.firstName
```

**Step 1: Parse**
```
Parser generates AST with SelectStatement, FromClause (Table u), WhereClause, GroupByClause
```

**Step 2: Bind**
```
Binder::BindSelect()
  ├─ BindFrom(FromClause)
  │   └─ Resolve "User u" → table_oid=1, alias "u"
  ├─ BindOrderBy() / BindLimitOffset()
  ├─ BindWhere()
  │   └─ BindExpression(u.age > 18)
  │       ├─ BindColumnRef(u.age) → (table_oid=1, col_id=2, type=INT)
  │       ├─ BindOperator(>)
  │       └─ BindIntegerLiteral(18)
  └─ BindGroupBy()
      └─ Resolve group keys + aggregation functions
```

**Output:** `BoundSelectStatement` with resolved column references, validated function signatures.

**Step 3: Plan**
```
Planner::PlanSelect()
  └─ Creates plan tree:
       ProjectionPlan([u.user_id, u.firstName, AVG(u.age)])
         ↑
       GroupByPlan([u.user_id, u.firstName], [AVG(u.age)])
         ↑
       FilterPlan(u.age > 18)
         ↑
       SeqScanPlan(table_oid=1)
```

**Step 4: Execute**
```
ExecutorFactory::createExecutor(plan)
  ├─ Creates SeqScanExecutor(TableHeap, table_oid=1)
  ├─ Creates FilterExecutor(child=SeqScanExecutor, predicate=Predicate(age > 18))
  ├─ Creates GroupByExecutor(
  │    child=FilterExecutor,
  │    grouping_keys=[user_id, firstName],
  │    aggregates=[AVG(age)]
  │  )
  └─ Creates ProjectionExecutor(child=GroupByExecutor, columns=[user_id, firstName, AVG(age)])

QueryExecutor::execute()
  └─ Call ProjectionExecutor.open() → getNext() × N → close()
```

**Result:** Tuples streamed via Volcano iterator model; no intermediate materialization.

---

## 🎯 Storage Layer (Unchanged, Integrated)

The **Query Execution Engine** sits atop the existing storage layer:

### Disk Manager
- Persistent I/O against `.db` file
- Random-access page reads/writes
- Metadata & directory tracking

### Buffer Pool Manager
- In-memory frame cache (configurable size)
- LRU eviction with dirty-page flush
- Pin-count tracking for safe concurrency

### Page & Tuple
- **Slotted page layout** — 4 KB blocks with header, slot array, tuple data
- **Tuple** — row; serializes Field list to/from bytes
- **Field** — typed column value (int, float, string) with serialization

### Table Layer
- **TableHeap** — CRUD and RID assignment (page_id + slot_num)
- **TableIterator** — Volcano-style full scan
- Persistent table metadata (schema, page chain, tuple count)

### Indexing
- **Static Hash Index** — O(1) average, auto-rehash at 75% load
- **B+ Tree Index** — O(log n) + range scans, persistent across restarts

---

## 📊 Query Execution Operators

All implement the **Volcano iterator model** (`open() → getNext() → close()`):

| Operator | Complexity | Use Case |
|----------|-----------|----------|
| **Sequential Scan** | O(N) | Full table scan; no index |
| **Selection (Filter)** | O(N) | WHERE clause evaluation |
| **Projection** | O(N) | SELECT column list |
| **Nested Loop Join** | O(N×M) | Small tables; any join condition |
| **Indexed Nested Loop Join** | O(N × log M) | Index exists on inner table |
| **Hash Join** | O(N+M) average | Large equi-joins; build hash table |
| **Merge Join** | O((N+M) log(N+M)) | Pre-sorted or ORDER BY input |
| **Hash Aggregation** | O(N) | GROUP BY (fits in memory) |
| **Sort Aggregation** | O(N log N) | GROUP BY (large datasets) |
| **External Merge Sort** | O(N log N + 2N×log B(N/B)) I/O | ORDER BY (larger than memory) |

---

## 🧪 Benchmarks (Disk-Based Joins)

**Test Setup:**
- Users table: 10,000 rows
- Orders table: 1,000,000 rows
- Equi-join on `user_id`; high skew (1,000 orders per user)
- Buffer pool: 100 frames

| Algorithm | Time | Index Required | Pre-sort |
|-----------|------|---------------|---------| 
| Hash Join | 22 sec | ✗ | ✗ |
| Indexed NLJ | 46 sec | ✓ (built at query time) | ✗ |
| Sort-Merge | 78 sec | ✗ | ✗ |
| Nested Loop | Too slow | ✗ | ✗ |

**Key observations:**
- Hash Join avoids repeated comparisons via hash table
- Indexed NLJ benefits from indexing but adds overhead (index construction at runtime)
- Sort-Merge requires two sorting phases; I/O-intensive
- Plain NLJ unsuitable for large datasets

---

## 💾 Persistence & Recovery

**Full crash recovery** — all artifacts survive process termination.

### Database Restart Sequence

```
1. DiskManager opens existing .db file
   └─ Loads database header, page directory

2. Catalog loads schema metadata
   └─ Tables, columns, data types

3. TableHeap restores metadata
   └─ Schema, page chains, tuple counts

4. Index reload loop
   └─ Static Hash: deserializeHashIndex()
   └─ B+ Tree:     loadBPlusTree() → loadNode()

→ All tables accessible, all indexes functional
→ No manual rebuild step
```

---

## 🚀 Future Enhancements

- **Query Optimizer** — cost-based plan selection (Selinger algorithm)
- **Statistics & Cardinality Estimation** — informed join order selection
- **Predicate Pushdown** — optimize filter placement
- **Multi-threaded Execution** — parallel operator pipelines
- **Transaction Support** — ACID guarantees (MVCC, WAL)
- **Additional Indexes** — Bitmap, Partial, Covering indexes
- **More Join Types** — Grace Hash Join, Hybrid Hash Join
- **Window Functions** — OVER clauses for OLAP
- **Correlated Subqueries** — EXISTS, IN, scalar subqueries

---