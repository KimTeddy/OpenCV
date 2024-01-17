# <이미지 영상처리 프로그램>

# 프로그램 설명서(사용법)
## 1) 가능한 기능 목록
- Image Open/Save
  - Image Open
  - Image re-Open
  - Image Save
- 프로그램 종료 버튼 Quit

- 영상 처리
  - Canny Edge Detection
  - Threshold
  - Histogram Equalize
  - Wrap
  - Find Color
  - Open/Close
  - HPF
  - Labeling
  - Gaussian
  - Corner Detection

## 2) 결과이미지
- 전체 실행 모습<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/a7ed17f8-3cfa-4741-8b4c-b859b8e1b830)
- GLUI<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/b2a89118-30f9-48cb-9cf7-070f3afd4067)

# 3) 사용 방법
## A. 초기 상태
- 처음에는 None, Open, Quit 버튼을 제외한 나머지 ui는 모두 disable 되어있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/e3a51b17-b521-4dd2-9348-5165d26f8d44)

## B. Image Open/Save
### B.1.1. Image Open
- Open을 누르고 원하는 이미지를 선택하고 열기를 누르면<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/0e71b09d-0bba-44b1-86d2-15ac2273cc7a)<br>
  input, output 이미지 창이 열리며 모든 ui가 enable 된다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/31902023-dad3-40b0-8846-fee75594085f)
### B.1.2. Image re-Open
- Open 상태에서 Open 버튼을 누르면 모든 창이 닫히고 새로운 이미지를 불러올 수도 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/3ebb1868-c6ae-4a7f-98d5-893c3738b521)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/17aab261-f6c3-46fe-913f-9d4cf025a369)

### B.2. Image Save
- Save 버튼을 누르고 이미지 확장자로 저장하면 되며, <br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/656475d1-ce52-4344-98a3-40e3df143ad1)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/496d8026-0aa4-4e1b-aed8-af92d521a7c1)
- 윈도우 이미지 뷰어에서도 잘 작동한다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/259aaf99-ade7-4913-bca3-219489266f59)


## C. 각종 이미지 처리 기능
- NONE을 선택하면 아무 효과도 적용되지 않은 초기 상태로 돌아온다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/2ebce440-b8cc-4cc6-aadf-0bf2cac37e6c)

### C.1. Canny Edge detection
- 원본 이미지의 컬러값이 적용된 Canny Edge detection 결과를 얻을 수 있다.
- low threshold 값과 threshold 전에 적용되는 Blur Size를 조절할 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/f6943eb4-194e-4f5e-a496-cb8fbfb17b41)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7d7dd05b-7a95-42eb-9302-d57c3cff4667)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/bdc2c951-1966-4026-a461-2c4d45a246a9)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/238cded2-500b-46c4-bf46-1c8968b1c7c4)

### C.2. Threshold
- threshold 종류를 선택하고 threshold 값을 조절할 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/0df168ff-e672-4b88-8cf0-35b3c659db97)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/9c758cf1-e241-4199-a544-d7d36d3bb073)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/86f9f81f-e057-414a-83a5-feb337f611ef)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/2e7140c3-696d-4ceb-a23c-73c5e6609a3d)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/3efb55d1-efbe-4e09-adec-20f8a96cd260)

### C.3. Histogram Equalize
- 히스토그램 평활화 기능이다.
- 컬러 이미지를 처리하는 프로그램이기에
  1. COLOR_BGR2YCrCb로 변환 후
  2. 밝기 성분만 히스토그램 평활화를 한 후
  3. merge하여 다시 COLOR_YCrCb2BGR로 변환하여 추출한 복사본 dst에 설정이 적용된다.<br>
  ![image](https://github.com/KimTeddy/OpenCV/assets/68770209/a4df86cb-0005-4d4f-b7a4-c0a0fc4bf4a9)
  ![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7b45dba7-7ddb-4277-a88c-95225267142b)


### C.4. Wrap
- Wrap 기능이 켜져있을 때 영상의 4개 꼭짓점 중 움직이고 싶은 점을 오른쪽의 라디오버튼에서 선택하고 왼쪽의 translation을 통해 해당 점을 움직일 수 있다.
- translation 자체의 특성으로 인한 이동 문제 해결을 위해 해당 점을 선택하고 translation을 하면 초기값이 들어간다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/d59c617c-349d-473e-b059-120154771235)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/f9be5fac-f686-4d93-a19e-df430d9add52)


### C.5. Find Color
- HSV 컬러스케일을 기반으로 해당 컬러 범위만 보여주는 기능이다.
- 원하는 범위를 H를 통해 설정해주면 해당 H를 기준으로 Range 만큼 컬러 허용 범위가 설정된다
- S, V는 최대 255 고정이며, 최소값을 설정하는 부분이다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/dc9c17f4-6d5c-4aa0-8ba5-ce75d3a499b1)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/ced9ca7b-ca27-4cf2-a44e-2f7ed72f4fd6)

- 색상 허용 범위 설정 기능<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/c29e7695-767d-403e-9384-856b9f7bd05e)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/475c2c5e-23a7-475c-a4bd-02ea0aa927a5)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/52057d7b-dd7d-4f70-8483-d9415a6df263)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/c646b04a-1fe0-4705-8eab-c239b278cebb)


### C.6. Open/Close
- threshold 후 opening과 closing 중에 선택할 수 있는 기능이다.
- 열림 연산 적용 모습<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/085f1961-a9f6-40e8-a229-6919321bf385)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/1fbbdba5-96f6-4dca-b71b-593db918b3e7)
- 닫힘 연산 적용 모습<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/e477bb27-a0b6-4dfb-a569-a6429f9a21c4)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/173f1d76-46af-4c45-8976-0420bb6c1494)


### C.7. HPF
- HPF를 적용한 결과 이미지를 얻을 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/d4da353b-8d1d-44b8-bcfd-642d26db0665)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/cd24f37e-d27a-40e3-b3fe-87ba468ade6b)


### C.8. Labeling
- 2-패스 알고리즘 적용 후 랜덤 색상으로 레이블링 한 결과 이미지를 얻을 수 있다.
- Threshold 값을 직접 설정할 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/b3b0074b-baf9-4270-bd9c-0691c45caf7b)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/23a16958-50fe-4868-9fcc-b4ece3572215)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7a3e1cbb-c23b-4318-b9f4-a8d1301da0b4)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7fcbfc63-7e81-4361-b5f8-0300941e93de)


### C.9. Gaussian
- 적응적 이진화 방법인 Adaptive Gaussian Thresholding을 적용할 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/e4500def-41f3-476c-a28e-8a15a94bcb4e)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/3b0c7e9d-9474-4ac5-b080-29ad8365736f)


### C.10. Find Corner
- 꼭짓점을 찾는 알고리즘을 적용할 수 있다.
- threshold 값을 조절하여 corner 감지 민감도를 조절할 수 있다.
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/4cafa99d-8e9b-440a-8fea-d24438871dc7)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/8a67bdb0-10de-4e3d-b1ca-57cd6357fed6)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/9b71e0da-bdb4-4860-b789-05f9916eabaf)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/4cf2bd3a-5952-4d3a-9d90-b06c9f9218f0)


# 2023 컴퓨터비전시스템 과제2
|게시일|2023.11.20 오전 12:39|
|-|-|
|마감일|2023.12.08 오후 11:59|
<pre>
과제2 : 강의 챕터별로 2~3개 영상처리 알고리즘을 선택하여 10개의 영상처리기능을 갖는 SW를 프로그래밍 하시오.

1. 모든 메뉴는 GLUI를 사용하여 구현해야 한다. (10점)
2. 임의 정지영상을 읽어 올 수 있어야 함. (5점)
3. 처리된 영상을 새로운 이름의 영상 파일로 저장할 수 있어야 한다. (5점)
4. 각 영상처리 알고리즘 구현 당 8점씩 총 80점.
</pre>
