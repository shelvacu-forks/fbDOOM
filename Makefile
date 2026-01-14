PREFIX ?= /usr
PROJECTS = fbdoom sndserv

all: $(PROJECTS)
.PHONY: all

$(PROJECTS): $@/*
	$(MAKE) -C $@ all
.PHONY: $(PROJECTS)

clean:
	$(foreach project_dir,$(PROJECTS),make -C $(project_dir) clean;)
.PHONY: clean

install:
	install -d $(PREFIX)/bin
	install -t $(PREFIX)/bin fbdoom/fbdoom sndserv/sndserver
.PHONY: install
