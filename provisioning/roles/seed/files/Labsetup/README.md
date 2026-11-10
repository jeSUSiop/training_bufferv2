# Buffer Overflow exercise — student files

> Educational use only. Everything here runs **locally on this `seed` machine**
> inside the training sandbox.

## Files
- `vuln.c` — source of the vulnerable program (read it; it's the whole target).
- `vuln` — the compiled, **setuid-root** binary you will exploit.
- `Makefile` — how `vuln` was built (32-bit, no stack protector, no PIE).
- `exploit.py` — a payload-builder skeleton for you to complete.

## The goal
`vuln` reads your input into a 64-byte stack buffer with **no bounds check**:

```c
char buffer[64];
read(0, buffer, 512);   // reads up to 512 bytes into 64 — overflow!
```

There is a function `secret()` that is **never called** by the program. It opens
the root-only file `/flag` and prints it. Because `vuln` runs **setuid-root**, if
you can make the program "return" into `secret()`, it will read `/flag` for you.

Your task: overflow `buffer` and overwrite the **saved return address** of
`vulnerable()` with the address of `secret()`.

## Step-by-step
1. See the leaked addresses:
   ```
   cd ~/bof
   ./vuln < /dev/null
   ```
   Note `secret() is located at` and the offset formula it prints:
   `offset = (ebp + 4) - buffer`.

2. Put those two numbers (`SECRET_ADDR`, `OFFSET`) into `exploit.py`.

3. Build the payload and fire it:
   ```
   ./exploit.py > payload
   ./vuln < payload
   ```
   On success you'll see `Access granted. Flag: flag{...}`.

## Stuck?
- Addresses are stable because ASLR is disabled here (`sysctl kernel.randomize_va_space`).
- `OFFSET` is just the distance from the start of `buffer` to the 4-byte slot that
  holds the return address. The program prints both `buffer` and `ebp`, and the
  return address lives at `ebp + 4`.
- Inspect with `gdb ./vuln` or `objdump -d ./vuln | grep -A2 '<secret>:'` if you
  want to confirm the address another way.
