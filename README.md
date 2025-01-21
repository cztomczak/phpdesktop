# PHP Desktop Chrome 130 for Mac

## Build instructions

1. Clone this repository and checkout this branch.
1. Install [Python](https://www.python.org).
1. Install [CMake](https://cmake.org/).
1. Install [Ninja](https://github.com/ninja-build/ninja).
1. Install Xcode and Xcode Command Line Tools.
1. Install PHP build dependencies. See https://www.php.net/manual/en/install.unix.source.php .
1. Best to disable Homebrew or similar package managers, so that dependencies are portable.
1. Download OpenSSL sources and extract it in the "phpdesktop/php/" directory. See https://github.com/openssl/openssl/releases .
1. Download libiconv sources and extract in php/ directory. See https://www.gnu.org/software/libiconv/ .
1. Download libxml2 official distribution tarball (tar.xz) and extract in php/ directory. See https://gitlab.gnome.org/GNOME/libxml2/-/releases .
1. Download sqlite sources and extract in php/ directory. See https://www.sqlite.org/download.html .
1. Download zlib 1.3.1 sources and extract in php/ directory. See https://github.com/madler/zlib/releases .
1. Download PHP sources and extract it in the "phpdesktop/php/" directory. See https://www.php.net/downloads.php .
1. RUn `buildopenssl.sh` script.
1. Run `buildiconv.sh` script.
1. Run `buildxml.sh`script.
1. Run `buildsqlite.sh` script.
1. Run `buildzlib.sh` script.
1. Run `buildphp.sh` script, but before running see the script comments to install requirements.
1. Run `cleanbuild.sh` script.
