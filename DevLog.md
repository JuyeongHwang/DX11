
## 개발일지

### 2021.08.06
  뿅하면 만들어지는 유니티, 언리얼과 다르게 창을 만드는 것부터 아니 프로젝트를 세팅하는 것부터 많은 고난이 있었다...<br>
  특히 윈도우 api를 처음 쓰다보니 LRESULT, wWinMain,hwnd 등 작은 단어 하나하나가 모두 모르는 단어여서 <br>
  코드를 읽어내리는데만 오랜 시간이 걸리고 이해도 잘 안가서 너무 힘들었는데 참고자료[1] 링크 덕분에 전반적으로 이해하기 좋았다.<br>
  
  <오늘 알게 된 점><br>
    다이렉트x 공부를 위해 이것저것 코드를 읽다보니 wWinMain앞에 붙어 있는 무엇인가가 다 달랐다. 찾아보니 결국 이름만 다르고 같은 것들이었다.<br>
    callback == winapi == apientry
    
### 2021.08.07
  <strike>다른 컴퓨터에서는 문제가 없었는데 그래픽카드가 없는 내 노트북에서 하려니까 sdk를 까는 것조차 안되었다,,데스크탑 새로 맞출때까지 C++ 공부 먼저 하는 걸로 ㅜㅜ</strike><br>
  내 놑북 그래픽이 다렉을 지원하는 버전이여야 하는 건 맞지만 글카가 없어서 안 깔리는건 아니었던걸로.. 오류였다.
  

### 2021.08.09
  ![dx](https://user-images.githubusercontent.com/49023736/128650580-85dd1d33-02a5-44bd-a6ce-4b6153cd03c8.jpg) <br>
  
  1장을 읽고있는데 선형대수의 집합체였다.. 5장 렌더링 파이프까지 봤는데 기본적인 개념을 정립하기는 좋았지만 directx12라 계속 봐야할지 고민이다<

  https://minok-portfolio.tistory.com/2?category=370780 <br>
  구글링하다가 찾게 된 사이트인데 나도 같이 다렉 스터디 할 사람 있으면 좋겠다ㅜㅅㅜ 부럽
  https://hombody.tistory.com/11?category=846211
  2d 참고하기 좋을 것 같다.

### 2021.08.10

삼각형을 그렸다
<img src="https://user-images.githubusercontent.com/49023736/128795674-805808d6-0d63-48b4-a3a2-e75a57280504.png" width="30%" height="30%" />

? : opengl과는 index 버퍼 순서가 반대인가???<br>
A : ㅇㅇ!(https://m.blog.naver.com/zxwnstn/222028734504)

사각형을 그렸당
<img src="https://user-images.githubusercontent.com/49023736/128796130-5be70c61-6347-4977-b38f-f7331168e7b7.png" width="30%" height="30%" />

direct3D 개요. 코드를 짜다보니까 device, deviceContext, 윈도우 핸들 등 반복되는 단어가 계속 나오는데 어렴풋이 알고 이해하려니까 속이 너무 답답.. 잘 모르겠는 단어들 먼저 공부를 하고 코드 이해를 하는게 좋을 것 같다.
https://m.blog.naver.com/masca140/220704230471 

내일 할거
https://m.blog.naver.com/atom723/190311655
https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-dxgi

### 2021.08.11

카메라 & 큐브 점프 및 움직이기
![ezgif com-gif-maker](https://user-images.githubusercontent.com/49023736/128961176-46216515-b1b9-4398-b8f1-2442af54a443.gif)

하루종일 texture만 했는데 알 수 없는 오류가 또 날 괴롭힌다.......
어디서 나는건지 모르겠어서 디버깅해보니까 d3dcompilefromfile 쪽에서 나고있었다. https://gpgstudy.com/forum/viewtopic.php?t=25475 <- 이 사이트에서 참고해보니, 세번째 인자가 비면 error가 발생할 수도 있다고 해서 다른 방식으로도 해보았지만 여전히 안된다....<br>
텍스쳐를 해야 라이팅을 하던 폰트를 하던 할텐데 ㅜㅜ <br>

https://www.braynzarsoft.net/viewtutorial/q16390-directx-11-an-introduction-to-the-win32-api
이 예제도 한번 따라해봐야겠다 에휴
