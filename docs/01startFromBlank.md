# 빈 프로젝트에서 시작하기

## 아래 상황을 가정하고 있다.
- TI사의 Code Composer Studio가 설치된 상태
- TI사의 Control Suite가 설치된 상태

## 내용
[Quick Guide for Creating & Running Assembly Projects in CCS/F28379D LaunchPad](http://www.add.ece.ufl.edu/4511/references/CCS%20C%20Project%20Creation.pdf) 

위 PDF에 나와있는 절차대로 진행함.

## 참고사항
### **1. Linker files**
```
If your project has the FLASH linker selected
```
문서에 보면 위와 같이 나와 있는데, 이는 아래와 같이 설정하라는 뜻이다.
-  `Project Explorer` 탭에서 현재 업로드 하려는 Project를 마우스 오른쪽 버튼을 클릭하여 `Properties`로 들어간다. 
-  `General` 탭에서 `Linker command file`을 **2837x_RAM_lnk_cpu1.cmd**에서 **2837x_FLASH_lnk_cpu1.cmd**로 바꾼다.
-  `Apply and close` 를 누르고 빠져나온다.