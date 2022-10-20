all:
	gcc -I/usr/local/include main.c -laergo -llua -lm -ldl -shared -fPIC -o aergo.so

clean:
	rm aergo.so
