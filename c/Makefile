PY?=python3
USER_FLAG?=--user
PIP?=$(PY) -m pip

ifneq ($(https_proxy), )
PROXY_ARG=--proxy=$(https_proxy)
else
ifneq ($(http_proxy), )
PROXY_ARG=--proxy=$(http_proxy)
else
PROXY_ARG=
endif
endif

test_%: dependencies
	$(PY) -m pytest -vl -n$* test_euler.py

test_simul_%: dependencies
	$(PY) -m pytest -vl -n$* --dist=each test_euler.py

test: dependencies
	$(PY) -m pytest -vl --benchmark-sort=fullname --benchmark-group-by=fullfunc --benchmark-verbose test_euler.py

dependencies:
	$(PIP) install -r requirements.txt $(USER_FLAG) $(PROXY_ARG)

lint:
	if test -z "$(clang-tidy p0000_template.c -warnings-as-errors=* 2>&1 | grep "Unknown command line argument")"; then \
		clang-tidy *.c -warnings-as-errors=*; \
	else \
		clang-tidy *.c; \
	fi
