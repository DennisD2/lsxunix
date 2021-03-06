# LSX Unix revival

*LSX is a port of Sixth Edition Unix by Heinz Lycklama to the LSI-11, a cut-down version of the PDP-11 with a microprocessor CPU. It was feared lost as it was never released outside of Bell Labs.*

LSX UNIX has some more info here: https://www.tuhs.org/cgi-bin/utree.pl?file=LSX

This work is heavily based on the BKUnix effort
of Leonid Broukhis and Serge Vakulenko.
Their original readme is [here](old_bkunix/README).

I took their work (mainly cross compile environment)
and started again with the old, original LSX tape/drive images that were found
in Paul Zacharys Garage. These contained most parts of the original
LSX Unix with sourcecode.

# Preparing lsx-util utility
This utility is required to create and manage files containing
disk drives that can be used by LSX UNIX in connection with
SIMH.

Build the tool:
```shell
cd fsutil
make install
```

# Extracting the disks
All files used in this section can be found below 
directory ```0_pauvl_zacharys_garage```.
Original disks are there stored as blobs in ```original-disks```.
Their content can be extracted.
It is required that the tool ```lsx-util``` is built before
trying the extraction process.

Execute script ```extract-disks.sh```
This creates a directory per disk in directory 
```extracted-disks```

```shell
cd 0_pavl_zacharys_garage
./extract-disks.sh
```

After having extracted the raw disks to file systems,
the script is doing its best to rearrange the content of all 
filesystems in a better organized file system.

This new file system is created in projects root directory ```src```.
All builds of the software will start from that directory.

# Checking content of original disks

## root/lib/crt0.o
With the PDP11 disassembler we can check ```crt0.o```
```
pdp11-disasm ./extracted-disks/root/lib/crt0.o
File: ./extracted-disks/root/lib/crt0.o
Type: FMAGIC (0407) relocatable
Section .text: 26 bytes
Section .data: 0 bytes
Section .bss: 2 bytes
Symbol table: 4 names (48 bytes)
Entry address: 02000
```
```
Disassembly of section .text:
002000 170011                   setd
002002 010600                   mov     sp, r0
002004 011046                   mov     (r0), -(sp)
002006 005720                   tst     (r0)+
002010 010066 000002            mov     r0, 2(sp)
002014 004767 177760            jsr     pc, <_main+02000>
002020 022626                   cmp     (sp)+, (sp)+
002022 010016                   mov     r0, (sp)
002024 004737 000000            jsr     pc, *$<_exit>
002030 104401                   sys     1

Disassembly of section .bss:
002032 <.bss>:                  . = .+2
```

The structure of the object file header can be found in 
```extracted-disks/cc2/ld.c```, structure ```filhdr```:

```
struct	filhdr {
	int	fmagic;
	int	tsize;
	int	dsize;
	int	bsize;
	int	ssize;
	int	entry;
	int	pad;
	int	relflg;
} filhdr;
```
So the real code starts with an offset of 16 bytes in the object file.
First bytes are the 'FMAGIC' with value ```000407``` (octal) follows.
In the hex listing, that value occurs as ```f009```.

Dumping the object file in octal mode with ```od``` shows the bytes 
as listed in the disassembler listing.
For example, in the second line of the od listing we can see the bytes 
```170011``` (octal) corresponding to the assembler line ```setd``` 
above. In the hex listing, that value occurs as ```f009```.

```
# OCTAL dump
od -An -o crt0.o 
 000407 000032 000000 000002 000060 000000 000000 000000
 170011 010600 011046 005720 010066 000002 004767 177760
 022626 010016 004737 000000 104401 000000 000000 000000
 000000 000000 000000 000000 000051 000000 000000 000000
 000030 000000 060563 071166 000065 000000 000044 000032
 062537 064570 000164 000000 000040 000000 066537 064541
 000156 000000 000040 000000 072163 071141 000164 000000
 000002 000000

# HEX dump
od -An -x crt0.o 
 0107 001a 0000 0002 0030 0000 0000 0000
 f009 1180 1226 0bd0 1036 0002 09f7 fff0
 2596 100e 09df 0000 8901 0000 0000 0000
 0000 0000 0000 0000 0029 0000 0000 0000
 0018 0000 6173 7276 0035 0000 0024 001a
 655f 6978 0074 0000 0020 0000 6d5f 6961
 006e 0000 0020 0000 7473 7261 0074 0000
 0002 0000
```

## Content of root/lib/liba.a
This contains mathematical functions including floating point routines.

But also some object files get, put, mesg, switch, ttyn, crypt, savr5

```
get.o
put.o
atan.o
hypot.o
mesg.o
switch.o
ttyn.o
rand.o
crypt.o
ecvt.o
pow.o
ldiv.o
dpadd.o
fp.o
gamma.o
floor.o
fmod.o
sin.o
sqrt.o
exp.o
log.o
savr5.o
```

## Content of root/lib/libc.a
Standard C library.

```
abort.o
abs.o
access.o
alarm.o
alloc.o
atof.o
atoi.o
chdir.o
chmod.o
chown.o
ctime.o
dup.o
execl.o
execv.o
fltpr.o
fork.o
fstat.o
getcsw.o
getgid.o
getpw.o
getuid.o
gtty.o
hmul.o
kill.o
ladd.o
ldfps.o
link.o
locv.o
longops.o
ltod.o
makdir.o
mcount.o
mdate.o
mknod.o
mktemp.o
mon.o
mount.o
nice.o
nlist.o
perror.o
pipe.o
printf.o
ptrace.o
putc.o
qsort.o
reset.o
rin.o
setgid.o
setuid.o
signal.o
sleep.o
snstat.o
stime.o
stty.o
sync.o
time.o
times.o
umount.o
unlink.o
wait.o
close.o
creat.o
csv.o
errlst.o
exit.o
ffltpr.o
getc.o
getchr.o
getpid.o
itol.o
lseek.o
nargs.o
open.o
prof.o
putchr.o
read.o
sbrk.o
seek.o
stat.o
write.o
cerror.o
```

# Summary of examination of existing disks

Source for crt0.o is missing. 

Source for libc.a and liba.a is missing.

Boot sector source is missing.

Most commands seems to exist. Also the complete build
toolchain seems to be there. There is no 'make', but shell scripts 
show how to compile.

# How to add a libc.a
See file [libc.md](doc/libc.md)

# Bootsector recreation
See file [bootsector.md](doc/bootsector.md)

# Bootstrap procedure of operating system
See file [bootstrap.md](doc/bootstrap.md)

# Create a new root disk
First step to do is to try to create a new root disk with the 
original files extracted from the original root disk.
This shows that the ```lsx-util``` works as excpected.

Later in the process we could provide additional content, 
like new commands or a changed Kernel and create the root disk.

The code below is for bkunix:
```
# root disk
u6-fsutil -n -s256000 -bsys/boot1 -Bsys/boot2lo root.dsk
cp sys/lsx .
u6-fsutil -a root.dsk lsx usr/ tmp/
rm -f lsx
u6-fsutil -a root.dsk etc/ etc/init etc/glob etc/mknod etc/mkfs etc/fsck
u6-fsutil -a root.dsk bin/ bin/sh bin/ls bin/echo bin/cal bin/cp bin/date bin/mkdir bin/sync bin/mv bin/rm bin/rmdir bin/stty bin/od bin/ed bin/cat bin/ln bin/wc bin/pwd bin/df bin/mount bin/umount
u6-fsutil -a root.dsk dev/ dev/tty8!c0:0 dev/fd0!b0:0 dev/fd1!b0:1
u6-fsutil -c root.dsk

# usr disk
u6-fsutil -n -s256000 usr.dsk
u6-fsutil -c usr.dsk
u6-fsutil -v root.dsk
```

And for LSX Unix
```
# root disk
#!/bin/bash
BASEDIR=`pwd`
FSUTIL="$BASEDIR/../fsutil/lsx-util"
#FSUTIL="$BASEDIR/../fsutil/lsx-util -v -v -v"

cd extracted-disks/root

# rm -f newroot.dsk
# ${FSUTIL} -n -S -s256000 -b$BASEDIR/../fsutil/rxboot -B$BASEDIR/../fsutil/rxboot2 newroot.dsk

${FSUTIL} -a newroot.dsk lsx usr/ tmp/
${FSUTIL} -a newroot.dsk etc/ etc/clri etc/cset etc/init etc/glob etc/mknod etc/mkfs #etc/fsck
${FSUTIL} -a newroot.dsk bin/ bin/sh bin/ls bin/cp bin/date bin/mkdir \
    bin/mv bin/rm bin/rmdir bin/stty bin/od bin/ed  \
    bin/as bin/cc bin/check bin/db bin/ld bin/load bin/ls bin/reloc bin/sh bin/size \
    bin/strip #bin/sync bin/echo bin/cal bin/cat bin/ln bin/wc bin/pwd bin/df bin/mount bin/umount
${FSUTIL} -a newroot.dsk lib/ lib/as2  lib/c0  lib/c1  lib/c2  lib/crt0.o  lib/liba.a  lib/libc.a
# OLD ${FSUTIL} -a newroot.dsk dev/ dev/tty8!c0:0 dev/fd0!b0:0 dev/fd1!b0:1
${FSUTIL} -c newroot.dsk
${FSUTIL} -v newroot.dsk

# usr disk

```