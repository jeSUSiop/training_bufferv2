# Stack Buffer Overflow Lab (SEED-style, on CyberRangeCZ) — Game Design Document

## 1. Overview
- **Scenario Name**: `buffer-overflow-seedlab`
- **Topic**: Software Security — Stack Buffer Overflow (return-address overwrite / ret2win)
- **Reference**: SEED Labs buffer-overflow material (Wenliang Du) & CyberRangeCZ scenario structure (`library-junior-hacker`)
- **Total Points**: 20 / 20
- **Target Audience**: Cybersecurity students / junior penetration testers
- **Design goal**: the **smallest reliable** buffer-overflow exercise — single machine, no Docker, no cross-host networking, fully deterministic addresses.

## Disclaimer
Educational use only, inside an isolated cyber range sandbox. The techniques must
not be used elsewhere. The scenario is fictitious.

## License
CC BY 4.0 (game design) / MIT (code). Lab content adapted from SEED Labs.

---

## 2. Scenario / Narrative
Everything happens on **one machine**, the `seed` workstation (`10.1.30.10`), which
is both the student's box and the target. In `~/bof` there is a 32-bit,
**setuid-root** program `vuln` that copies attacker input into a 64-byte stack
buffer with no bounds check. A function `secret()` — never called on the normal
path — reads the root-only `/flag`. The student overflows the buffer, overwrites
the saved return address with `&secret`, and captures the flag.

Why single-machine: earlier multi-host / Docker versions of this training were
fragile to provision on the range. Keeping the whole exercise local on `seed`
removes every networking dependency and makes the attack reproducible.

---

## 3. Infrastructure & Topology

| Host | Image | Mgmt User | Flavor | IP | Purpose |
|------|-------|-----------|--------|----|---------|
| `seed` | `seed` | `ubuntu` | `m1.medium` | `10.1.30.10` | student workstation **and** target |
| `router` | `debian-12-x86_64` | `debian` | `standard.small` | `10.1.30.1` | routing / gateway |

- Network `lab-switch`: `10.1.30.0/24`; WAN `internet-connection`: `100.100.100.0/24`.
- Student login: `user` / `Password123` (sudo enabled via the `user-access` role).

---

## 4. The vulnerable program (`vuln.c`)
- `char buffer[64]` filled by `read(0, buffer, 512)` — the overflow.
- Prints `&secret`, `&buffer`, and the saved `ebp` at runtime, plus the offset
  formula `(ebp + 4) - buffer`, so no debugger is strictly required.
- Compiled `-m32 -static -fno-stack-protector -no-pie` → 4-byte addresses, no
  canary, fixed code addresses.
- Installed `04755 root:root` (setuid-root), so `secret()`'s `fopen("/flag")`
  succeeds (euid 0). No shell is spawned, so there is no setuid-drop pitfall.
- ASLR disabled sandbox-wide for deterministic addresses.

---

## 5. Scoring Breakdown (Total 20 / 20)

| Level | Title | Type | Max Score | Hints & Penalties | Flag / Answer |
|-------|-------|------|-----------|-------------------|---------------|
| 0 | Introduction | INFO | 0 | None | — |
| 1 | Access the Sandbox | ACCESS | 0 | None | `start` |
| 2 | Task 1 — Understand the target | TRAINING | 4 | Hint 1 (-1.0) | `secret` |
| 3 | Task 2 — Exploit & capture the flag | TRAINING | 12 | Hint 1 (-1.5), Hint 2 (-2.0), Hint 3 (-2.5) | `flag{r3turn_2_s3cr3t_c4ptured}` |
| 4 | Task 3 — How would you stop this? | TRAINING | 4 | Hint 1 (-1.0) | `-fstack-protector` |
| **Total** | | | **20 / 20** | | |

Concrete flags (no APG), matching this repo's working Linear-training setup.
Estimated duration: **~45 minutes**.

---

## 6. Educational Workflow (Learning Outcomes)
1. **Recon (Task 1)** — read `vuln.c`, run the target once, identify the
   never-called `secret()` and understand why reaching it leaks the flag.
   *Outcome:* read a program's control flow and spot the attack goal.
2. **Exploit (Task 2)** — compute `offset = (ebp + 4) - buffer`, set `SECRET_ADDR`
   and `OFFSET` in `exploit.py`, build `payload = b"A"*offset + p32(&secret)`, pipe
   it in, and read `/flag` as root. *Outcome:* overwrite a saved return address to
   hijack control flow (ret2win) and abuse a setuid binary.
3. **Defense (Task 3)** — identify the **stack canary** (`-fstack-protector`) as the
   mitigation that would detect the overwrite before the function returns.
   *Outcome:* connect the attack to a concrete, named mitigation.

---

## 7. Provisioning Notes
- `provisioning/roles/seed/tasks/main.yml` installs tooling (incl. `gcc-multilib`),
  disables ASLR, deploys `Labsetup/` to `~/bof`, compiles `vuln`, sets it
  setuid-root, and plants `/flag`.
- `server`/Docker roles from the original CSRF scenario have been removed; the
  working CyberRangeCZ plumbing (`user-access`, `chrony`, `sandbox-logging`,
  `hosts`) is retained unchanged.
