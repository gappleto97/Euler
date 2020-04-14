NOT_TERMUX?=$(shell command -v termux-setup-storage > /dev/null 2>&1 ; echo $$?)
HAS_NPM?=$(shell command -v npm > /dev/null 2>&1 ; echo $$?)

test: dependencies
	npx mocha

test_%: dependencies
	echo "WARNING: multithreaded tests not supported on javascript"
	npx mocha

ifneq ($(HAS_NPM),0)
ifeq ($(OS),Windows_NT)  # if no NPM and Windows
dependencies:
	1>&2 echo "Windows not yet supported"
	exit /b -1
else
ifeq ($(NOT_TERMUX),0)  # if no NPM and Termux
dependencies:
	pkg install -y nodejs;
	$(MAKE) _dependencies $(MFLAGS)
else  # if no NPM and not Termux
dependencies:
	curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.34.0/install.sh | bash; \
	export NVM_DIR="$$HOME/.nvm"; \
	[ -s "$$NVM_DIR/nvm.sh" ] && \. "$$NVM_DIR/nvm.sh" \
	nvm install --lts
	$(MAKE) _dependencies $(MFLAGS)
endif
endif
else  # has NPM
dependencies: _dependencies
endif

ifneq ($(NOT_TERMUX),0)
_dependencies:
	npm install;
else
_dependencies:
	npm install -g;
endif

