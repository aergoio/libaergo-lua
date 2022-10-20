all:
	gcc main.c -laergo -llua -lm -ldl -shared -fPIC -o aergo.so

install:
	cp aergo.so /usr/local/lib/

clean:
	rm aergo.so
