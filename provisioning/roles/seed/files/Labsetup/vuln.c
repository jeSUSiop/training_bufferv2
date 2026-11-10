/*
 * vuln.c  --  Vulnerable program for the Buffer Overflow exercise.
 *
 * EDUCATIONAL USE ONLY. This program contains a deliberate stack
 * buffer-overflow vulnerability. It is meant to be exploited only inside
 * this isolated training sandbox.
 *
 * It is compiled 32-bit, without the stack protector and without PIE, and
 * installed setuid-root. The function secret() is never called by the normal
 * control flow -- your job is to hijack the saved return address so that
 * vulnerable() "returns" into secret(), which reads the root-only /flag.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Never called directly. Reaching this function is the goal. */
void secret(void)
{
    char flag[128];
    FILE *f = fopen("/flag", "r");
    if (f == NULL) {
        printf("secret() reached, but /flag could not be opened.\n");
        return;
    }
    if (fgets(flag, sizeof(flag), f) != NULL) {
        printf("Access granted. Flag: %s\n", flag);
    }
    fclose(f);
}

void vulnerable(void)
{
    char buffer[64];

    /* Leak the two values you need. ASLR is disabled in this lab, so these
     * stay stable between runs.
     *   - secret()  : the address you want to jump to
     *   - buffer    : start of the overflowable buffer
     *   - ebp       : saved frame base; the saved return address sits at ebp+4
     */
    unsigned int ebp_val;
    __asm__ volatile ("mov %%ebp, %0" : "=r"(ebp_val));

    printf("secret() is located at : %p\n", (void *)&secret);
    printf("buffer[] starts at     : %p\n", (void *)buffer);
    printf("saved frame base (ebp) : 0x%08x\n", ebp_val);
    printf("=> offset to the saved return address = (ebp + 4) - buffer\n");
    printf("Send your input now:\n");
    fflush(stdout);

    /* VULNERABILITY: reads up to 512 bytes into a 64-byte buffer. */
    read(0, buffer, 512);

    printf("You said: %s\n", buffer);
}

int main(void)
{
    vulnerable();
    printf("vulnerable() returned normally. No flag this time.\n");
    return 0;
}
