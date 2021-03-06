/*
 * cp oldfile newfile
 * for whole floppy
 */

main(argc,argv)
char **argv;
{
	static int buf[256];
	int fold, fnew, n;
	register char *p1, *p2, *bp;
	int bcnt;
	int mode;

	if(argc != 3) {
		write(1, "Usage: cp oldfile newfile\n", 26);
		exit(1);
	}
	if((fold = open(argv[1], 0)) < 0) {
		write(1, "Cannot open old file.\n", 22);
		exit(1);
	}
	fstat(fold, buf);
	mode = buf[2];
	/* is target a directory? */
	if (stat(argv[2], buf+50)>=0 && (buf[52]&060000)==040000) {
		p1 = argv[1];
		p2 = argv[2];
		bp = buf+100;
		while(*bp++ = *p2++);
		bp[-1] = '/';
		p2 = bp;
		while(*bp = *p1++)
			if(*bp++ == '/')
				bp = p2;
		argv[2] = buf+100;
	}
	if (stat(argv[2], buf+50) >= 0) {
		if (buf[0]==buf[50] && buf[1]==buf[51]) {
			write(1, "Copying file to itself.\n", 24);
			exit(1);
		}
	}
	if ((fnew = creat(argv[2], mode)) < 0) {
		write(1, "Can't create new file.\n", 23);
		exit(1);
	}
	write(1,"Get Ready\n",10);
	read(0,"x",1);
    for(bcnt=0; bcnt<494; bcnt++) {
	if(n = read(fold,  buf,  512)) {
	if(n < 0) {
		write(1, "Read error\n", 11);
		err(); exit(1);
	} else
		if(write(fnew, buf, n) != n){
			write(1, "Write error.\n", 13);
			err(); exit(1);
		}
	}
	else
		bcnt = 494;
    }
	write(1,"Done\n",5);
	read(0,"x",1);
	exit(0);
}

err() {
	write(1,"Error\n",6);
	read(0,"x",1);
}
