# Stack Buffer Overflow Lab for the CyberRangeCZ Platform

A small, self-contained **stack buffer-overflow** training scenario for the
[CyberRangeCZ Platform](https://docs.platform.cyberrange.cz/), modeled on the
official [`library-junior-hacker`](https://github.com/cyberrangecz/library-junior-hacker)
template.

The whole exercise runs **on a single machine** (`seed`), so there is no
cross-host or Docker networking to misconfigure. Students exploit a 32-bit,
setuid-root program with a classic return-address overwrite (ret2win) to read a
root-only flag.

---

## 🛠️ Topology

| Host | Image | Mgmt user | Flavor | IP | Role |
|------|-------|-----------|--------|----|------|
| `seed` | `seed` | `ubuntu` | `m1.medium` | `10.1.30.10` | student workstation **and** target (the whole lab lives here) |
| `router` | `debian-12-x86_64` | `debian` | `standard.small` | `10.1.30.1` | network routing / gateway |

- Network `lab-switch`: `10.1.30.0/24`
- Student login on `seed`: user **`user`** / password **`Password123`**

---

## 🎯 The exercise

`~/bof/vuln` (setuid-root, 32-bit, no stack protector, no PIE) reads input into a
64-byte stack buffer with no bounds check:

```c
char buffer[64];
read(0, buffer, 512);   // overflow
```

A function `secret()` — never called on the normal path — opens the root-only
`/flag` and prints it. The student overwrites the saved **return address** so the
program returns into `secret()`. The program leaks `secret()`'s address, the
buffer address and `ebp` at runtime, and ASLR is disabled, so the attack is fully
deterministic.

---

## 🎓 Levels & Scoring (20 points)

| # | Title | Type | Score | Answer |
|---|-------|------|-------|--------|
| 0 | Introduction | INFO | 0 | — |
| 1 | Access the Sandbox | ACCESS | 0 | `start` |
| 2 | Task 1 — Understand the target | TRAINING | 4 | `secret` |
| 3 | Task 2 — Exploit the overflow and capture the flag | TRAINING | 12 | contents of `/flag` |
| 4 | Task 3 — How would you stop this? | TRAINING | 4 | `-fstack-protector` |

Estimated duration: **~45 minutes**. Flags are concrete (no APG), matching this
repo's working Linear-training configuration.

---

## 📁 Provisioning

`provisioning/roles/seed/` builds the exercise at sandbox creation time:
- installs `build-essential`, `gcc-multilib` (32-bit support), `gdb`, `python3`;
- disables ASLR (`kernel.randomize_va_space=0`, persisted);
- copies `Labsetup/` (`vuln.c`, `Makefile`, `exploit.py`, student `README.md`) to `~/bof`;
- compiles `vuln` and makes it **setuid-root** (`04755`);
- plants `/flag` (`root:root`, mode `0600`).

Common roles reused from the template: `user-access`, `chrony`, `sandbox-logging`, `hosts`.

---

## 🚀 Deployment

1. Push this directory to your CyberRangeCZ scenario repository (contains `topology.yml` and `provisioning/`).
2. In the CyberRangeCZ web interface:
   - **Sandboxes → Sandbox Definitions → Add Definition** → point to this repo.
   - **Trainings → Training Definitions → Import** → select `training.json`.
3. Create a **Training Instance** with a sandbox pool for your class.

---

## License

Lab design adapted from the SEED Labs buffer-overflow material (Wenliang Du,
Syracuse University), for educational use. Scenario structure based on
`library-junior-hacker` (Masaryk University). Code under MIT; game design under
CC BY 4.0.
