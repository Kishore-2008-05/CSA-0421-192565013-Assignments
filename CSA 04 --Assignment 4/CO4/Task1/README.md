# Task 1 – File-System Design Review (Register No. 192565013)

## 1.1 Weaknesses of the proposal (7)
1. **Name clashes** – 12,000 students in one flat directory means two students choosing `report.docx` overwrite each other.
2. **Search time** – a linear directory scan is O(n); with hundreds of thousands of files every open is slow (an indexed/hashed directory only partly helps).
3. **Contiguous allocation** – files cannot grow without being moved; external fragmentation leaves free space that cannot hold a new file.
4. **No protection** – no access control means any student can read, modify or delete another's work (privacy and integrity failure).
5. **Backup/recovery** – one flat directory cannot be backed up per student or restored selectively; one corruption affects everyone.
6. **Group projects** – no way to give a team shared access without giving everyone access.
7. **Naming by student choice** – inconsistent names make automation, quota and audit impossible.

## 1.2 Improved design
```
/home/<year>/<department>/<register_no>/        # one directory per student (owner = student, mode 700)
/projects/<course>/<project_id>/                 # shared project folders (group = project group, mode 2770 setgid)
/courses/<course>/                               # staff-owned, read-only for students (mode 755/750)
```
- **Structure:** a tree (home directories) plus links to shared folders – an acyclic graph, no cycles.
- **Naming rules:** directories use the register number (unique key); files are free-text but must be unique inside the owner directory.
- **Attributes:** owner, group, permission bits, size, timestamps, quota.
- **Permissions:** home 700; project folder 2770 with a project group, so new files inherit the group; staff added by ACL (`setfacl -m u:staff:rX`).
- **Links:** `ln -s /projects/os/team7 ~/team7` gives each member a path to the shared folder.
- **Allocation:** indexed (inode/extents) so files can grow without moving. Per-student quota and nightly per-directory backup.

## 1.3 Design defence (≤100 words)
Each student has a private home directory that only the owner can read or write, so privacy does not depend on trust. Shared work lives in project folders whose group contains only team members; setgid makes new files inherit that group, and staff access is added through ACLs rather than by loosening permissions. Unique register-number paths prevent name clashes, extent-based allocation lets files grow, and per-directory quotas and backups limit the damage of any one failure. Sharing is therefore explicit and limited to named groups.

## Alternative: Google Drive / OneDrive
| Aspect | Own server tree | Cloud drive |
|---|---|---|
| Time to find/open | O(1)-O(log n) directory lookup on campus LAN | Network round trip + web/API; search index is fast but depends on Internet |
| Storage used | One copy (+ own backups) | Provider keeps several copies; versions add space |
| Privacy control | College controls everything | Provider terms and location apply; college controls sharing only through admin settings |
| Effort | Admin must run, patch, back up | Provider runs it; college manages accounts |
Complexity: lookup O(log n) with a B-tree/hashed directory in both; space O(files) vs O(files × versions).
