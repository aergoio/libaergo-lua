all:
	@echo "please inform the lua version\nexamples:\nmake 5.1\nmake 5.2\nmake 5.3\nmake 5.4\nmake default"

default:
	gcc main.c -laergo -llua -lm -ldl -shared -fPIC -o aergo.so

5.1:
	gcc main.c -I/usr/include/lua5.1 -laergo -llua5.1 -lm -ldl -shared -fPIC -o aergo.so

5.2:
	gcc main.c -I/usr/include/lua5.2 -laergo -llua5.2 -lm -ldl -shared -fPIC -o aergo.so

5.3:
	gcc main.c -I/usr/include/lua5.3 -laergo -llua5.3 -lm -ldl -shared -fPIC -o aergo.so

5.4:
	gcc main.c -I/usr/include/lua5.4 -laergo -llua5.4 -lm -ldl -shared -fPIC -o aergo.so

install:
	cp aergo.so /usr/local/lib/

clean:
	rm aergo.so
