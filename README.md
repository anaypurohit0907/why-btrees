# why-btrees

A database engine built from scratch, for learning how databases actually work under the hood.

Most people think a database is just a 2D table. It isn't — it's fixed-size pages on disk, a pager shuffling bytes through the OS, and an index (usually a B+ tree) deciding where every key lives. This project rebuilds those layers by hand, one phase at a time, in three languages.

## Roadmap

| Phase | Language | Scope | Status |
|-------|----------|-------|--------|
| 1. Storage foundation | C++ | 4KB pages, file I/O, hash index, CLI | ✅ Done |
| 2. B+ tree index | C++ | Replace hash index with a disk-based B+ tree | 🔜 In progress |
| 3. C fundamentals | C | Replicate phase 2 with explicit memory management | Planned |
| 4. Final deliverable | Go | Minimal SQL: `CREATE TABLE`, `INSERT`, `SELECT WHERE` | Planned |

## Architecture

```
main.cpp      REPL CLI (SET / GET / EXIT)
database.*    key→page index + SET/GET logic
pager.*       file I/O layer (pread/pwrite, superblock, free list)
page.*        fixed 4KB page, serialize/deserialize
```

### Page

Fixed 4096-byte pages. On-disk layout:

```
page_number(4) | magic(4) | type(1) | is_allocated(1) | key_len(2) | key_data | value_len(2) | value_data
```

Strings are length-prefixed raw bytes — `std::string` objects can't be `memcpy`'d (they hold heap pointers). Serialization is a byte protocol independent of C++ memory layout.

### Pager

Owns the `data.db` file via POSIX `pread()`/`pwrite()` — explicit system calls, manual buffering, chosen over `mmap()` for learning. Page 0 is a superblock:

```
magic(4) | free_head(4) | free_count(4) | num_pages(4)
```

Freed pages form a singly-linked free list threaded through the first 4 bytes of each freed page. `create_page()` reuses free pages before extending the file.

### Database

In-memory `std::unordered_map<string, page_id>` index, rebuilt on startup by scanning pages. This hash index gets replaced by a B+ tree in phase 2 — which is the whole point of the repo's name: once range scans and ordered iteration matter, hashing falls apart and trees win.

## Build & Run

Requires Linux, g++ with C++17.

```bash
make
./db
```

```
> SET name alice
OK
> GET name
alice
> EXIT
```

## References

- *Database Internals* — Alex Petrov
- SQLite — `src/btree.c`, `src/pager.c`
- CMU 15-445 lecture notes

> Learning project, not production code.
