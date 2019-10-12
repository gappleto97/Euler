
set -e;
if [ $pyver ]; then
    if [ $pyver != pypy3 ]; then
        git clone https://github.com/gappleto97/terryfy;
        source terryfy/travis_tools.sh;
        get_python_environment $pydist $pyver;
    else
        export PYTHON_EXE=$pyver;
        export PIP_CMD="$PYTHON_EXE -m pip";
    fi
    $PIP_CMD install virtualenv;
    $PYTHON_EXE -m virtualenv -p $PYTHON_EXE venv;
    source venv/bin/activate;
    if [ $linter ]; then
        make pytest PY=python LINT=true USER_FLAG=
    else
        make pytest PY=python LINT=false MYPY= USER_FLAG=
    fi
elif [ $jsver ]; then
    if [ $linter ]; then
        cd javascript;
        npm install eslint-config-google@latest eslint@>=5.16.0;
        npx eslint *.js && echo "Linting successful!"
    else
        make jstest;
    fi
else
    make ctest;
fi
