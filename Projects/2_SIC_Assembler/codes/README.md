SIC ASSEMBLER PROJECT
=================
2st SP Project
--------------

> ###### SP course in Sogang Univ.
> ###### Spring Semester in 2019
> ###### YUSEOK. 20171690

-------------------------
### 컴파일 및 실행 방법
```bash
$ make
$ ./20171690.out
```

### 지원 명령어
```bash
sicsim> help
```
> 실행가능한 모든 명령어들의 리스트 출력한다.

```bash
sicsim> dir
sicsim> d
```
> 현재 디렉터리에 있는 파일들 출력한다.
> **명령어 추가**

```bash
sicsim> quit
sicsim> q
```
> sicsim을 종료한다.

```bash
sicsim> history
sicsim> hi
```
> 현재까지 사용한 명령어들을 순서대로 번호와 함께 출력한다.

```bash
sicsim> dump [start, end]
sicsim> du [start, end]
```
> 할당되어 있는 메모리의 내용을 출력한다.
> start 주소와 end 주소를 함께 사용할 수 있다.

```bash
sicsim> edit address, value
sicsim> e address, value
```
> memory의 address 번지의 값을 value에 지정된 값으로 변경한다.

```bash
sicsim> fill start, end, value
sicsim> f start, end, value
```
> memory의 start 번지부터 end 번지까지의 값을 value에 지정된 값으로 변경한다.

```bash
sicsim> reset
```
> 메모리 전체를 전부 0으로 변경시킨다.

```bash
sicsim> opcode mnemonic
```
> mnemonic에 해당하는 opcode를 출력한다.

```bash
sicsim> opcodelist
```
> opcode Hash Table의 내용을 출력한다.

#### 추가된 명령어
```bash
sicsim> type filename
```
> **finename에 해당하는 파일을 읽어 화면에 출력한다.**

```bash
sicsim> symbol
```
> **가장 최근에 assemble한 파일의 symbol table을 Z-A 순서로 출력한다.**
