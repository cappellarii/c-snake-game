## Aluno: João Guilherme Freitas Cappellari
Data: domingo 15/11/2026




## Linux

Instale o GCC, o Make e as bibliotecas de desenvolvimento do FreeGLUT, OpenGL e
GLU. Na pasta do projeto, execute:

```sh
make
./snake-game
```

## Windows (MSYS2 UCRT64)

Instale o [MSYS2](https://www.msys2.org/), abra o terminal **MSYS2 UCRT64** e
instale as ferramentas e a biblioteca:

```sh
pacman -S --needed make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-freeglut
```

Na pasta do projeto, execute:

```sh
make
./snake-game.exe
```

O programa usa o terminal para selecionar a dificuldade e mostrar o placar.
Execute-o a partir de um terminal, não apenas com um clique duplo. O arquivo
`libfreeglut.dll`, fornecido pelo pacote FreeGLUT, deve estar no PATH ou na
mesma pasta do executável; no terminal UCRT64, ele já está no PATH. Para
distribuir o jogo fora do MSYS2, inclua também as demais DLLs não pertencentes
ao Windows que forem necessárias, consultando `ldd snake-game.exe` no UCRT64.

Para compilar no Linux para Windows, use `make windows` com o compilador
`x86_64-w64-mingw32-gcc` e uma instalação Windows do FreeGLUT. Se necessário,
informe os caminhos com `WINDOWS_CPPFLAGS` e `WINDOWS_LDFLAGS`, por exemplo:

```sh
make windows WINDOWS_CPPFLAGS="-I/caminho/include" WINDOWS_LDFLAGS="-L/caminho/lib"
```

Em qualquer plataforma, `make clean` remove os executáveis compilados.
<br>    
<br>
Feito por:
João Cappellari <br>
e <br>
Copilot <3