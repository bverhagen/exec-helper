COMPILER=g++

NB_OF_CORES:=$(shell grep -c ^processor /proc/cpuinfo)

all: init build

init:
	cmake -H. -Bbuild/$(COMPILER)/debug -DCMAKE_CXX_COMPILER=$(COMPILER) -DCMAKE_INSTALL_PREFIX=build/$(COMPILER)/debug -DCMAKE_BUILD_TYPE=Debug -DUSE_SYSTEM_CATCH=ON

build: init
	make -C build/$(COMPILER)/debug --jobs $(NB_OF_CORES)
	make -C build/$(COMPILER)/debug install

clean-build:
	make -C build/$(COMPILER)/debug --jobs $(NB_OF_CORES) clean

init-release:
	cmake -H. -Bbuild/$(COMPILER)/release -DCMAKE_CXX_COMPILER=$(COMPILER) -DCMAKE_INSTALL_PREFIX=build/$(COMPILER)/release -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CATCH=OFF

app: init-release
	make -C build/$(COMPILER)/release --jobs $(NB_OF_CORES) exec-helper
 
clean: clean-build
	rm -f *.exec-helper

.PHONY: init build clean-build init-release app clean
