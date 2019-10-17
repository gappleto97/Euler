set -e;
if [ $pyver ]; then
    if [ $linter ]; then
        make pytest LINT=true PY=python USER_FLAG=
    else
        export PY_EXE=python
        if [ $py36 ]; then
            sudo add-apt-repository -y ppa:jonathonf/python-3.6
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
    if [ $pcc ]; then
        sudo apt-get install -y build-essential flex bison
        mkdir pcc pcc-libs
        wget -O - -o /dev/null http://pcc.ludd.ltu.se/ftp/pub/pcc-releases/pcc-1.1.0.tgz | tar -xz --no-seek -C pcc --strip-components=1
        wget -O - -o /dev/null http://pcc.ludd.ltu.se/ftp/pub/pcc-releases/pcc-libs-1.1.0.tgz | tar -xz --no-seek -C pcc-libs --strip-components=1
        cd pcc
        sed -i 's/MANPAGE=@BINPREFIX@cpp/MANPAGE=@BINPREFIX@pcc-cpp/' cc/cpp/Makefile.in
        ./configure --prefix=/usr --libexecdir=/usr/lib/{x86_64,i386}-linux-gnu
        sudo make && sudo make install
        cd ../pcc-libs
        ./configure --prefix=/usr --libexecdir=/usr/lib/{x86_64,i386}-linux-gnu
        sudo make && sudo make install
        cd ..
    elif [ $icc ]; then
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
        sudo apt-get install -y valgrind $COMPILER_OVERRIDE python3-pip
    fi
    cd c && make test USER_FLAG=
fi
