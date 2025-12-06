# Konfiguracja Qt + MySQL (MinGW)
Aby aplikacja Qt działała poprawnie, należy skopiować biblioteki do katalogu kompilacji `cmake-build-debug`.

### Wymagane pliki
Do `cmake-build-debug`

- ***Qt6Sql.dll*** z katalogu `C:\Qt\6.10.1\mingw_64\bin`
- ***libmysql.dll*** z katalogu `C:\Program Files\MySQL\MySQL Server 8.0\lib`

Do `cmake-build-debug\plugins\sqldrivers`

- ***qsqlmysql.dll*** z katalogu `C:\Qt\6.10.1\mingw_64\plugins\sqldrivers`

### Brak plików
W przypadku braku któregoś z powyższych plików, należy postępować zgodnie z poniższym materiałem.
 
<a href="https://youtu.be/7YIJUL9pz2U" target="_blank">
  <img src="https://img.youtube.com/vi/7YIJUL9pz2U/maxresdefault.jpg" width="300">
</a>

