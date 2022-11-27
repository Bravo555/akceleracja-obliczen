# Prezentacja prototypu

## Co pokażemy

- Budowanie i uruchomienie kodu
- Dane testowe
- Jak dużo czasu zajmuje cały program + sam algorytm [1]
- Omówienie kodu + jak profilowaliśmy

### Budowanie i uruchomienie kodu

- Targetem jest skończenie unifikacji dwóch projektów CMake i generowanie jednego rozwiązania VS składającego się z
  albo:
    - osobnych projektów na CPU i GPU LUB
    - wspólnego projektu na CPU i GPU gdzie strategia wybierana jest at runtime

Program chciałbym uruchamiać w podobny do GREPa sposób:

```
$ ./kmp --gpu PATTERN FILE # dla GPU
$ ./kmp --cpu-sp PATTERN FILE # dla CPU singleprocessing
$ ./kmp --cpu-mp PATTERN FILE # dla CPU multiprocessing
```

### Dane testowe

Mamy następujące dane testowe:

- Biblia
- Pan Tadeusz

i słowa kluczowe wczytywane z plików. Ponieważ słowa kluczowe to krótkie słowa bez spacji, to nwm czy jest sens je
wczytywać z plików.

### Jak dużo czasu zajmuje cały program + sam algorytm

Można liczyć `time`em, plus info o tym jak długo działa algorytm reportowany przez sam program

### Omówienie kodu + jak profilowaliśmy

Różne strategie separacji tekstu:

- na GPU dzielimy tekst wejściowy na części po 2m i każdy wątek dostaje zazębiające się zakresy: `[0; 2m), [1m; 3m),
    [2m; 4m)`, etc.
- na CPU robimy to co oryginalnie zamierzaliśmy, czyli dzielimy tekst na N części gdzie N=liczba logicznych CPU, od
  miejsca podziału szukamy pierwszej spacji i dzielimy tekst na tej spacji
