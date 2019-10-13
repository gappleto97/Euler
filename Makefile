PY?=python3

py%:
	cd python && $(MAKE) $* $(MFLAGS)

c%:
	cd c && $(MAKE) $* $(MFLAGS)

js%:
	cd javascript && $(MAKE) $* $(MFLAGS)

%:
	$(MAKE) c$* js$* py$* $(MFLAGS)

doxygen:
	doxygen doxygen.cfg

