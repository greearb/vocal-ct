

TARS= client.cxx  server.cxx  stun.cxx  stun.h  tlsServer.cxx  udp.cxx  udp.h \
	Makefile \
	client.sln  client.vcproj  server.sln  server.vcproj  Stun.sln \
	id.pem id_key.pem root.pem \
	WinStun/resource.h WinStun/stdafx.cpp WinStun/stdafx.h \
	WinStun/WinStun.cpp WinStun/WinStunDlg.cpp WinStun/WinStunDlg.h WinStun/WinStun.h \
	WinStun/WinStun.rc WinStun/WinStun.vcproj \
	WinStun/res/WinStun.ico WinStun/res/WinStun.manifest WinStun/res/WinStun.rc2 \
	WinStunSetup/WinStunSetup.vdproj


VERSION=0.8.3

#CXXFLAGS+=-O2
#LDFLAGS+=-O2 -lssl
STUNLIB=libstun.a

#
# Alternatively, for debugging.
#
CXXFLAGS+=-g
LDFLAGS+=-g -Wall -lssl -lcrypto

all: server client tlsServer

tar: $(TARS)
	cd ..; tar cvfz - $(addprefix stund/, $(TARS))  > \
		 ` date +"stund_$(VERSION)_$(PROG)%b%d.tgz" `

server: server.o stun.o udp.o 
	$(CXX) $(LDFLAGS) -o $@  $^

tlsServer: tlsServer.o stun.o udp.o
	$(CXX) $(LDFLAGS) -o $@  $^

client: client.o stun.o udp.o 
	$(CXX) $(LDFLAGS) -o $@  $^

%.o:%.cxx
	$(CXX)  -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

libstun.a: stun.o udp.o
	ar rv $@ $<

%:RCS/%
	co $@

# Dependancies
server.o: stun.h udp.h 
clint.o: stun.h udp.h 
stun.o: stun.h udp.h
udp.o: stun.h udp.h 
tlsServer.o: stun.h udp.h 
