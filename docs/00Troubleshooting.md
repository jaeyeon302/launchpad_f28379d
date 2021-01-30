# Troubleshooting

# 목차
- [Troubleshooting](#troubleshooting)
- [목차](#목차)
- [내용](#내용)
  - [Flash에 업로드하면 제대로 실행이 안되는 문제에 관하여](#flash에-업로드하면-제대로-실행이-안되는-문제에-관하여)
    - [문제상황](#문제상황)
    - [해결방법](#해결방법)
    - [참고사항](#참고사항)
    - [참고한 문서](#참고한-문서)



# 내용
## Flash에 업로드하면 제대로 실행이 안되는 문제에 관하여
### 문제상황
위 문제는 업로드는 제대로 되었으나 LINE BY LINE으로 디버깅 해나가면 `DELAY_US` 함수 라인에서 잘못된 주소로 프로그램 포인터가 이동해버리는 상황에 관한 것이다.

`DELAY_US` 함수를 열어보면 실제로는 macro임을 알 수 있고, 해당 명령어는 RAM으로 업로드할 때에만 지원되는 함수이다. flash로 업로드하였을 경우 `DELAY_US`는 같이 올라가지 않고, 잘못된 주소를 가리키게 된다. 따라서 flash로 업로드할 때에는 이러한 RAM function들을 모두 복사해줄 필요가 있다.

### 해결방법
1. `Project Explorer` 탭에서 프로젝트를 마우스 오른쪽 버튼으로 클릭하고 `Properties` 를 클릭한다
2. `General` 탭의 `Linker Command File`을 `2837x_FLASH_lnk_cpu1.cmd`로 설정한다.(Flash로 설정되어 있는 사람은 넘어간다)
3. `main.c` 파일 상단 `#include <F28x_Project.h>` 아래 아래의 문구를 추가한다
    ```C
    #define _FLASH
    ```
4. `int main(void)` 함수 가장 윗 부분에 아래와 같은 코드를 추가한다
    ```C
    int main(void)
    {
        #ifdef _FLASH
        memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
        #endif
    ...
    ```

4. `DELAY_US` 함수를 사용해도 디버깅이 잘 이루어지고 잘 동작함을 확인한다

### 참고사항
다시 RAM으로 업로드하고자 한다면 `Linker Command File`을 다시`2837x_FLASH_lnk_cpu1.cmd`로 변경하고 `#define _FLASH`를 주석처리하면 된다.
```C
// #define _FLASH
```
### 참고한 문서
- [LAUNCHXL-F28377S: DELAY_US() leads to illegal ISR while running from flash](https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/572414?LAUNCHXL-F28377S-DELAY-US-leads-to-illegal-ISR-while-running-from-flash)