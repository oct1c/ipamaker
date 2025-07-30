# ipamaker
A simple but useful tool to quickly convert .app directorys into .ipa files

made by Octic

## Build
1. Install zip
   - MacOS: `brew install zip`
   - Linux: `sudo pacman -S zip` / `sudo apt install zip` / ...
2. run `git clone https://github.com/oct1c/ipamaker && cd ipamaker`
3. run `gcc -o ipamaker ipamaker.c`
4. run `sudo cp ./ipamaker /usr/bin/ipamaker`
5. run `sudo chmod +x /usr/bin/ipamaker`

## Usage
Usage: `ipamaker <.app_file_path> [-o output_path]`

If you don't set the output path, the .ipa file will be put into the same folder as the .app directory
