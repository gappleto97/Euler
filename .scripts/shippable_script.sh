set -e;
if [ $pyver ]; then
    if [ $linter ]; then
        make pytest LINT=true PY=python USER_FLAG=
    else
        export PY_EXE=python
        if [ $py36 ]; then
            sudo add-apt-repository -y ppa:deadsnakes/ppa
            sudo apt-get update --fix-missing
            sudo apt-get install python3.6
            virtualenv -p python3.6 venv
            export PY_EXE=$PWD/venv/bin/python
        fi
        make pytest LINT=false PY=$PY_EXE USER_FLAG= MYPY=
    fi
elif [ $jsver ]; then
    if [ $linter ]; then
        make jsdependencies
        cd javascript;
        npm install eslint-config-google@latest eslint@>=5.16.0;
        npx eslint *.js && echo "Linting successful!"
    else
        make jstest
    fi
else
    sudo apt-get update
    if [ $icc ]; then
        mkdir iccdir
        wget -O - -o /dev/null https://registrationcenter-download.intel.com/akdlm/irc_nas/15873/intel-sw-tools-installer.tar.gz | tar -xz --no-seek -C iccdir  --strip-components=1
        cd iccdir
        sudo apt-get update
        sudo apt-get install -y cpio
        sed -i -e "s/ACCEPT_EULA=decline/ACCEPT_EULA=accept/g" silent.cfg
        sed -i -e "s/ACTIVATION_TYPE=no_license/ACTIVATION_TYPE=serial_number/g" silent.cfg
        echo "ACTIVATION_SERIAL_NUMBER=CKDX-WNDG7BK7" >> silent.cfg
        echo "SELECTION_CONFIG_FILES_PATH=$PWD/packages.json" >> silent.cfg
        echo '{"selections":[{"moduleId":"intel_cpp_compiler"},{"moduleId":"intel_threading_building_blocks","isDependencyTool":true}]}' > packages.json
        sudo ./install.sh -s silent.cfg
        PATH="/opt/intel/system_studio_2019/bin${PATH:+:${PATH}}"
        export PATH
        cd ..
    else
        sudo apt-get install -y valgrind $COMPILER_OVERRIDE
    fi
    sudo apt-get install -y dwarves python3-pip
    cd c && make test USER_FLAG=
fi
