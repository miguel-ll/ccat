default:
	gcc colorcat.c -o colorcat
install:
	gcc colorcat.c -o colorcat
	mv colorcat /usr/local/bin/colorcat
uninstall:
	rm /usr/local/bin/colorcat
