# AGENTS.md — Database Engine Learning Project

agents only guide user, strictly no writing code for them. user writes code, agent reviews and explains.
user has slightly weak fundamentals on low-level concepts (memory, disk I/O, system calls, C++ internals). explain concepts as needed.
goal: teach both computers and C++.

## Objective

Build a database engine from scratch to understand fundamental database concepts.

## Timeline

4 weeks, learning-focused (runnable but priority is internal understanding)

## Approach

**Multi-language:**
1. **Phase 1-2:** C++ — Storage foundation + B+ tree
2. **Phase 3:** C — Replicate to understand low-level
3. **Phase 4:** Go — Final deliverable with SQL interface

## Plan

### Phase 1: Storage Foundation (C++, Week 1)
- Fixed 4KB pages, file I/O
- Simple hash index for key→page mapping
- CLI: `SET key value`, `GET key`

### Phase 2: B+ Tree Index (C++, Week 2)
- Replace hash index with B+ tree
- Insertion, page splitting, tree height

### Phase 3: C Fundamentals (C, Week 3)
- Replicate Phase 2 in C, explicit memory management

### Phase 4: Final Project (Go, Week 4)
- Minimal SQL: CREATE TABLE, INSERT, SELECT WHERE key = X

## Technical Specs

- Page size: 4096 bytes
- Platform: Linux (Fedora), x86_64
- Storage: Disk-based, single-user embedded
- Build: `g++ -std=c++17`, Makefile in project root

## Study Resources

- "Database Internals" by Alex Petrov
- SQLite: `src/btree.c`, `src/pager.c`
- CMU 15-445 lecture notes

## Current Codebase State

```
db-from-scratch/
  AGENTS.md    — this file
  Makefile     — build system
  page.h       — Page class declaration
  page.cpp     — Page class implementation (serialize/deserialize complete)
  database.h   — empty placeholder
  database.cpp — empty placeholder
  main.cpp     — test harness (6 tests, all pass)
```

- Page class: complete, tested, working.
- Database layer / Pager / CLI: not yet started.
- No `.db` data files exist yet.

## Concepts Covered

### Session 1: Why 4KB Pages
Disks read in blocks, RAM pages are 4KB, filesystem blocks are 4KB. Databases manage fixed-size blocks called pages for predictable I/O, simple allocation, efficient caching.

### Session 1: Page Layout
Header (metadata: page type, allocation status, checksum) + data area (key-value pairs).

### Session 1: File I/O Approach
POSIX `pread()`/`pwrite()` with offset — explicit system calls, manage buffering. Chosen over `mmap()` for learning.

### Session 1: Key→Page Mapping
Simplest: linear scan. Better: in-memory `std::unordered_map<string, page_id>` rebuilt on startup.

### Session 2: Serialization Concepts
- `memcpy` for raw memory copying
- Fixed-width types (`uint32_t`, `uint16_t`, `uint8_t`) guarantee exact byte sizes across platforms
- `std::byte` — memory as raw bytes, no arithmetic allowed
- **On-disk vs in-memory format:** C++ objects have their own sizes (`enum` = 4 bytes). Disk format is a separate protocol with explicit byte widths
- `reinterpret_cast<T*>(&var)` — treat address as different pointer type. No new data, just glasses change. Size unchanged.
- `static_cast<T>(value)` — convert value to new type. New data of different size.
- `sizeof(enum)` = 4 (default int underlying type). Fixed by `static_cast` to `uint8_t` or explicit `: uint8_t`
- Variable-length strings: store length prefix (uint16_t), then character data. Can't memcpy `std::string` objects (they contain heap pointers)
- **On-disk layout:** `page_number(4) | magic(4) | type(1) | is_allocated(1) | key_len(2) | key_data | value_len(2) | value_data`

### Session 2: C++ Mechanics
- **Value vs reference**: by-value copies, by-reference (`&`) shares original. `const &` for read-only without copy.
- `std::string::assign(const char*, count)` — construct string from raw character buffer
- **`return` must match declared return type** — vector method must return vector
- Member initializer lists vs body assignment
- `const` correctness on methods

## Conversation History

### Initial Discussion — User Profile
- Motivation: Learning/didactic
- Background: comfortable-ish with C/C++ but rusty. Intermediate, not beginner.
- Mental model gap: thought SQL = 2D array. Corrected to pages + B+ trees.
- Chose build-first, theory-parallel approach.

### Session 2 — Serialize/Deserialize Debugging

**State before session:** Page class skeleton existed. `serialize()` had:
- `reinterpret_cast` without `&` (casting integer value to pointer — UB)
- No data copied, no return statement
- `deserialize()` was empty stub

**Debugging process:**
1. Fixed `&` on casts + added missing `*` on pointer type declaration
2. Added `key_length`/`value_length` uint16_t variables for string length prefix
3. `memcpy` key and value data with `.data()` method
4. Fixed offset: value_data starts at `14+key_length` not `14+value_length`
5. Discovered `sizeof(Page_type)` = 4 but on-disk format uses 1 byte. Writes overlapped offsets 8-11, then partially overwritten by is_allocated and key_length. Deserialize read 4 bytes back → garbage type_ value.
6. Fixed by `static_cast<uint8_t>(type_)` for 1-byte conversion
7. Dead `s_is_allocated` pointer variable removed (already gone)
8. Added `const &` parameter to deserialize (was by-value, copying 4KB)
9. Built test harness with 6 tests → all pass

**User understanding improved:**
- Now gets value vs address distinction with `&`
- Understands `reinterpret_cast` just relabels types (no size change)
- Understands `static_cast` actually converts between types (size changes)
- Understands `std::string` stores characters elsewhere (heap), can't memcpy the object
- Understands serialization as a byte-layout protocol independent of C++ memory layout
- Can mentally trace byte offsets through a buffer

## Next Steps (Phase 1 remaining)

1. **Pager class** — file I/O layer: open/close file, `pread`/`pwrite` pages, allocate new pages
2. **Database class** — hash index (`unordered_map<string, page_id>`), scan on startup, SET/GET logic
3. **CLI** — readline loop in main.cpp dispatching SET/GET
4. Handle page-full scenario (allocate new page, link or rehash)

## Notes

- Priority: Internal understanding over usability
- Not production code — learning project
- User launches opencode from `/home/anay/Desktop/db-from-scratch/`
