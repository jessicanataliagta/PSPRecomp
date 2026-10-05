#include "psprecomp/elf32.hpp"
#include "psprecomp/runtime.hpp"

#include <exception>
#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
    try {
        const std::filesystem::path elf_path = argc >= 2
            ? std::filesystem::path(argv[1])
            : std::filesystem::path("profiles/ctw/game/PSP_GAME/SYSDIR/EBOOT_DECRYPTED.ELF");
        psprecomp::Runtime runtime;
        const auto elf = psprecomp::Elf32Image::from_file(elf_path);
        (void)elf.load_and_relocate(runtime.memory(), psprecomp::kDefaultPspUserLoadBase);
        psprecomp::register_generated_functions(runtime);
        std::cout << "Registered " << runtime.function_count() << " generated CTW entries from " << elf_path.string()
                  << '\n';
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "ctw_generated_check: " << error.what() << '\n';
        return 1;
    }
}
