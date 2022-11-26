# akceleracja-obliczen

Projekt na kurs akceleracji obliczeń zawierający współbieżną implementacją algorytmu KMP. Na ten moment istnieją dwie
wersje:

- na CPU, w katalogu `cpu/`
- na GPU, w katalogu `gpu/`

## Ustawianie projektu

Wersja CPU nie wymaga żadnych SDK ani innych specjalnych konfiguracji, więc pozostawiamy to jako zadanie dla czytelnika.

Co do wersji GPU: proces bazuje głównie na oficjalnym przewodniku [Getting started with OpenCL on Microsoft
Windows](https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/getting_started_windows.md), z kilkoma małymi
dodatkami integrującymi VS Code.

Projekt wykorzystuje narzędzie vcpkg do instalacji i konfiguracji OpenCL SDK oraz CMake który ściąga tą konfigurację z
vcpkg i używa do generowania plików dla wybranych systemów budowania.

Celem skonfigurowania projektu proszę przechodzić przez przewodnik powyżej, mając na względzie poniższe:

1. vcpkg powinien być zainstalować w roocie projektu, tworząc następującą strukturę katalogu:
    ```
    .
    ├── .git
    ├── .gitignore
    ├── .vscode
    ├── README.md
    ├── cpu
    ├── gpu
    └── vcpkg

    5 directories, 2 files
    ```

2. Uwaga na różnicę w `CMakeLists.txt` w tutorialu a w repo, nie zmieniać pliku, użyć wersji z repo:
    ```
    - find_package(OpenCL CONFIG REQUIRED)
    + find_package(OpenCL REQUIRED)
    ```

3. Konfiguracja VSCode żeby pozbyć się include errorów i żeby można było
   uruchomić proces budowania bez terminala.

    Po przejściu przewodnika, wygenerowana została przez CMake konfiguracja przez CLI:
    ```
    $ cmake -A x64 -S . -B .\build -D CMAKE_TOOLCHAIN_FILE=<VCPKGROOT>\scripts\buildsystems\vcpkg.cmake
    ```
    Nie znalazłem żadnego sposobu na import tej konfiguracji w VSCode CMake Tools, więc będziemy musieli skonfigurować
    odpowiednio rozszerzenie i wygenerować jeszcze raz.

    Konfiguracja rozszerzenia odbywa się w `.vscode/settings.json`. Wystarczy skonfigurować i uruchomić projekt przez
    VSCode:

    1. `Command Palette > CMake: Configure`
    2. `Command Palette > CMake: Run without debugging` (albo `Shift + F5`)

## Uruchomienie programu

Argumenty: 
- ścieżka do pliku .txt z tekstem
- ścieżka do pliku .txt z wzorcem
- ścieżka do pliku .csv, gdzie zostanie zapisany wynik

### Wersja GPU

Przykład uruchomienia. Z katalogu `gpu/build`:

```powershell
Release\HelloOpenCL.exe ..\..\text_files\text1.txt ..\..\text_files\key1.txt ..\..\results\gpu-text1-key1.csv
```

### Wersja CPU

Przykład uruchomienia. Z katalogu `cpu/build`:

```powershell
Release\kmp_cpu.exe ..\..\text_files\text1.txt ..\..\text_files\key1.txt ..\..\results\cpu-text1-key1.csv
```