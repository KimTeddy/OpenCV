# <동영상 영상처리 프로그램>

## 결과이미지
### 전체 실행 모습
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/0ede77dc-7a44-46c5-95c6-b2f0658b0c52)
### GLUI
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/f9c125af-d51b-4363-873c-6205eef6387a)


# 가능한 기능 목록
- [영상 열기/저장](#b-video-opensave)
	- [Video Open](#b1-video-open)
	- [Video Save](#b2-video-save)
- 영상 재생/이동
	- [영상 빨리 되감기/되감기/일시정지/재생/빨리 재생](#c-영상-재생일시정지)
	- [영상 진행 바 -> 현재 영상 진행도 확인](#c-영상-재생일시정지)
- 마우스 클릭&드래그 -> 그림그리기
	- [Draw](#d2-draw)
- 이전, 이후 히스토그램 창 띄우기
	- [Histogram](#d5-color-histogram-창)
- 영상 처리
	- [Wrap](#d1-wrap)
	- [Colormap](#d3-colormap)
		- Choose ColormapType
	- [Histogram Equalize](#d4-histogram-equalize)
		- Clahe Histogram Equalize ON/OFF
	- [Canny Edge Detection](#d6-canny-edge-detection)
		-LowTh 조절
		-BlurSize 조절
- 영상 분석
	- [Hough Lines](#d7-hough-lines)
	- [Hough Circles](#d8-hough-circles)
	- [Remove Background](#d9-remove-background)


# 사용 방법
## A. 초기 상태
처음에는 Open, Quit 버튼을 제외한 나머지 ui는 모두 disable 되어있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7a88f336-1fa7-4950-a13d-4b7a886dd130)

## B. Video Open/Save
### B.1. Video Open
- Open을 누르고 원하는 영상을 선택하고 열기를 누르면<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/a49ff25e-4c07-445e-9b11-3d9ec825add6)<br>
영상이 열리며 현재 사용 가능한 ui가 enable 된다.<br>
- Open 상태에서 Open 버튼을 누르면 모든 창이 닫히고 새로운 영상을 불러올 수도 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/ca66ae3e-e5be-41ff-b0e4-d684317fd86d)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/946b854d-4a25-4d00-87fe-f34035d7f861)

### B.2. Video Save
- Save 버튼을 누르고 .mp4 확장자로 저장하면 되며,<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/d40e68ae-5d90-4a44-a3e5-9433495ddf10)<br>
- 현재 프레임부터 ESC를 누를 때까지 영상 처리가 적용된 영상이 저장된다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/48233ffa-40fe-4b61-88c9-8a72901b3d6e)<br>
- 윈도우 미디어 플레이어에서도 잘 작동한다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/b2df8325-0615-4a9a-92fe-fb901e0c44bc)<br>


## C. 영상 재생/일시정지
- button<br>
  - 영상 재생 관련 버튼을 누를 때 현재 재생 상태를 글자로 알려준다.<br>
  - 처음/ 끝에 도달했을 때 pause 상태로 바뀐다.<br>
  ![image](https://github.com/KimTeddy/OpenCV/assets/68770209/56b7e4f0-d493-4203-a376-69a876f892be)

- scrollbar<br>
  - Play Scrollbar를 통해 전체 영상중에 현재 재생 위치를 확인 가능하다.<br>
  - 일반 동영상 플레이어와 동일하게 Play Scrollbar를 일시정지/재생 상태 상관 없이마우스로 잡고 드래그하면 해당 프레임으로 이동한다.<br>
  ![image](https://github.com/KimTeddy/OpenCV/assets/68770209/c19ba1f3-fa06-4ef7-bf74-80f41ac4b95e)


## D. 각종 영상 처리 기능
- Wrap 기능과 Canny Edge Detection을 켰을 때 작동하는 Hough Lines 기능은 기본적으로 켜두었다.<br>
- MouseCallback으로 작동하는 Draw를 제외한 모든 기능은 영상이 재생하는 동안에만 작동한다.<br>

### D.1. Wrap
- Wrap 기능이 켜져있을 때 영상의 4개 꼭짓점 중 움직이고 싶은 점을 오른쪽의 라디오버튼에서 선택하고 왼쪽의 translation을 통해 해당 점을 움직일 수 있다.<br>
- translation 자체의 특성으로 인한 이동 문제 해결을 위해 해당 점을 선택하고 translation을 하면 초기값이 들어간다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7a4d39b8-59e3-493f-8be6-22d1fe9163dc)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/4b824dfd-fad7-4828-a1cb-1ab485c18e95)<br>
- Wrap 기능을 끄면<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/69cd2130-d459-403f-93a8-aa318756d00f)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/eb08fcbd-db7b-4194-bf56-fb0f1dbbf4c1)<br>
영상에도 적용되지 않는다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/e3b2d060-218b-49c8-914f-f581deb54857)


### D.2. Draw
- Draw 기능을 활성화하면 마우스 드래그를 통해 그림을 그릴 수 있다.<br>
- Save에는 적용되지 않는다.<br>
- Draw Color rollout도 같이 활성화되며, open해보면 그림 색깔을 바꿀 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/08fcdba4-4812-4603-95cd-456197463ebd)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/08171486-9b80-4402-87ea-cfa740246a52)

- 영상 정지 시에는 그림을 그릴 수 있고,<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/06c8307b-f7fb-40dc-9701-849d989392d6)

- 영상 재생중에는 레이저포인터처럼 사용할 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7c5928b5-60a7-4858-a4b8-0ecaa748d8d0)


### D.3. Colormap
- 기본적으로 RAINBOW 타입으로 설정되어 있다.<br>
- Colormap을 켜면 Colormap rollout이 활성화되며, 원하는 컬러 타입을 선택할 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7fe73bdd-5811-432d-9de7-f2855f38302e)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/0dbdfad1-dbc6-4297-8e51-4dfdee7391b2)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/5206eec6-ff9d-4ba2-aae1-1171b26fc948)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/a12a2982-db3f-42b8-89bf-1fa2c781f42e)

### D.4. Histogram Equalize
- 히스토그램 평활화 기능이다.<br>
- 컬러 영상을 처리하는 프로그램이기에 COLOR_BGR2YCrCb로 변환 후 밝기 성분만 히스토그램 평활화를 한 후 merge하여 다시 COLOR_YCrCb2BGR로 변환하여 추출한 현재 복사본 프레임에 설정이 적용된다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/62cd08b6-2af7-4e5a-94ec-96186a1871aa)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/8a94c9f4-95c9-454b-bbaf-2f17165276c4)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/21fc158b-d820-4d86-97d5-b52c6c395669)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/d99006cc-4e00-42bc-86e0-5ff041be953e)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7aee9c76-f93b-49e6-a970-1499142438d9)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/ba1b3112-5b8c-4eaa-bf17-a71ab926ac4a)<br>

- 이는 히스토그램 기능으로 더 정밀하게 확인 가능하다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/a0204b5b-7b10-4362-98be-23a3b96d1cd1)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7c518b87-43cc-4c97-a2cb-41147dba6cf6)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/ec39653a-00a1-42e5-9e5e-7463cd80c68a)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/5c5e6c35-7b6b-4373-adab-b59cf579b51e)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/19d5824c-e260-4407-b104-0054d829cb04)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/09301387-7bc9-4559-9099-da0d9e40d702)


### D.5. Color Histogram 창
- Histogram을 선택하면 Before Histogram, After Histogram 창이 열린다.<br>
- 예제의 히스토그램 그리는 함수를 변형하여 RGBW 모든 값을 1개의 창에서 동시에 확인할 수 있다.<br>
- 효과 적용 전, 후 히스토그램을 동시에 확인 가능하다.<br>
- Histogram을 선택 해제하면 자동으로 비디오 창은 그대로 둔 상태로 Histogram 창만 닫힌다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/e1751d44-9f7a-47a7-88f1-88fac94f9284)<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/5458f0f4-c11e-485c-89d0-ea70bc642046)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/7151569e-720b-436e-a2f4-5d0f97f991cc)


### D.6. Canny Edge detection
- 처음에 Canny Edge detection을 켜면 Hough Lines가 자동 적용돼있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/89313cee-5491-47d6-b4a1-a71318a0a509)
- 그리고 Cannny rollout이 활성화된다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/62a10a55-9f01-4d4f-af19-9756ba7a0c63)

- Hough Lines를 끄면 다음과 같이 Canny Edge detection이 잘 적용된 모습을 볼 수 있다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/ce40fec7-8698-45c3-a350-1f7436cc41d6)


### D.7. Hough Lines
- Canny Edge detection을 켠 상태에서 Hough Lines를 활성화하면 Hough Lines 알고리즘이 실행된다.
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/41f11cd6-8698-48e8-a9f7-b7d46a98b423)

### D.8. Hough Circles
- Hough Circles를 활성화하면 Hough Circles 알고리즘이 실행된다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/cfd891da-6a12-4a8d-b1b2-5da9a5099bab)

### D.9. Remove Background
- 예제를 변형하여 배경제거 결과를 마스크로 사용하여 영상이 컬러로 나온다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/d966a891-8f63-4be3-b06a-ee408b81c2b5)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/69b961f3-fc3c-4169-8e07-18705dcaf6cd)
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/78ce83ae-cfa5-4d30-b63b-bbffb454dd0b)

- 이 때 Background 화면 창도 함께 띄워진다.<br>
- 마찬가지로 이 기능을 끄면 창은 자동으로 닫힌다.<br>
![image](https://github.com/KimTeddy/OpenCV/assets/68770209/3920548c-839f-4d65-a34f-b02c1ec3720d)


# 2023 컴퓨터비전시스템 과제1 내용
|게시일|2023.11.16 오전 12:00|
|-|-|
|마감일|2023.12.01 오후 11:59|
|평가점수|100|
<pre>
과제1 : 동영상을 입력화일로 사용하여 동영상을 play할 수 있고, 추가적인 영상처리를 적용하여 부가기능을 구현

1. 모든 메뉴는 GLUI를 사용하여 구현해야 한다. (10점)
2. 임의의 동영상 화일을 선택하여 입력화일로 읽어 들일 수 있게 해야 한다. (10점)
3. 동영상을 play할 때 다음의 기능을 수행할 수 있어야 한다.
  1) forward play (동영상 끝 영상에 도달할 경우 정지되어야 함) (10점)
  2) backward play (동영상 첫 영상에 도달할 경우 정지되어야 함) (10점)
  3) forward fast play (동영상 끝 영상에 도달할 경우 정지되어야 함) (10점)
  4) backward fast play  (동영상 첫 영상에 도달할 경우 정지되어야 함) (10점)
  5) pause (한번 선택하면 동영상 play가 정지, 다시 선택하면 이전 play mode로 다시 play) (10점)
4. 동영상에 추가기능을 부가하여 처리된 결과 동영상도 같이 3번의 play 기능이 실행되어야 한다. (20점)
   난이도에 따라 점수 배점이 달라짐.
5. 처리된 동영상을 새로운 이름의 동영상 파일로 저장할 수 있어야 한다. (10점)
</pre>
