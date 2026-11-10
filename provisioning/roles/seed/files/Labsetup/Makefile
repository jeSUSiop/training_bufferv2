# Buffer Overflow exercise -- build the vulnerable program.
#
# Flags explained:
#   -m32                 build a 32-bit binary (simpler, 4-byte addresses)
#   -static              no shared-library dependencies at runtime
#   -fno-stack-protector disable the stack canary (so the overflow is possible)
#   -no-pie              fixed code addresses (secret() has a stable address)
#   -g -w                debug symbols, silence warnings

CC     = gcc
CFLAGS = -m32 -static -fno-stack-protector -no-pie -g -w

all: vuln

vuln: vuln.c
	$(CC) $(CFLAGS) -o vuln vuln.c

clean:
	rm -f vuln payload
