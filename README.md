# PHP Desktop Chrome 130 for Mac

## Build instructions

1. Clone this repository and checkout this branch.
2. Install [Python](https://www.python.org).
3. Install [CMake](https://cmake.org/).
4. Install [Ninja](https://github.com/ninja-build/ninja).
5. Install Xcode and Xcode Command Line Tools.
6. Install PHP build dependencies. See https://www.php.net/manual/en/install.unix.source.php .
6. Download PHP sources and extract the tar file to phpdesktop/php/ directory. So you should have for example a phpdesktop/php/php-8.x.x/ directory. See https://www.php.net/downloads.php .
7. Run `buildphp.sh` script, but before running see the script comments to install requirements.
8. Run `cleanbuild.sh` script.
