script=$(realpath "$0")
root_dir=$(dirname "$script")

# Easy switching between arm/intel via command line.
# Install: softwareupdate --install-rosetta
# In .~/.zshrc add:
# alias intel="$env /usr/bin/arch -x86_64 /bin/zsh ---login"
# alias arm="$env /usr/bin/arch -arm64 /bin/zsh ---login"

if [[ $(uname -m) == "arm64" ]]; then
    arch="arm64"
elif [[ $(uname -m) == "x86_64" ]]; then
    arch="x86_64"
else
    echo "Unknown architecture"
    exit 1
fi

build_dir=$root_dir/build_$arch
downloads_dir=$root_dir/downloads

clear && printf '\e[3J'
set -o xtrace

cd $root_dir
rm -rf $build_dir/*
mkdir $build_dir
mkdir $downloads_dir
cd $build_dir

cmake -G "Ninja" -DPROJECT_ARCH="$arch" -DCMAKE_BUILD_TYPE=Release $root_dir
ninja -j 8 phpdesktop
