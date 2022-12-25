#开发框架头文件路径。
PUBINCL = -I/root/project/public

#开发框架:cpp文件名。
PUBCPP = /root/project/public/_public.cpp

#编译参数
CFLAGS = -g

all:crtsurfdata1 crtsurfdata2 crtsurfdata3 crtsurfdata4 crtsurfdata5

crtsurfdata1:crtsurfdata1.cpp
	g++ $(CFLAGS) -o crtsurfdata1 crtsurfdata1.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata1 ../bin/.

crtsurfdata2:crtsurfdata2.cpp
	g++ $(CFLAGS) -o crtsurfdata2 crtsurfdata2.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata2 ../bin/.
crtsurfdata3:crtsurfdata3.cpp
	g++ $(CFLAGS) -o crtsurfdata3 crtsurfdata3.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata3 ../bin/.
crtsurfdata4:crtsurfdata4.cpp
	g++ $(CFLAGS) -o crtsurfdata4 crtsurfdata4.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata4 ../bin/.
crtsurfdata5:crtsurfdata5.cpp
	g++ $(CFLAGS) -o crtsurfdata5 crtsurfdata5.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata5 ../bin/.


clean:
	rm crtsurfdata1 crtsurfdata2 crtsurfdata3 crtsurfdata4 crtsurfdata5
