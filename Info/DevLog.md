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
  =><strike>내 놑북 그래픽이 다렉을 지원하는 버전이여야 하는 건 맞지만 글카가 없어서 안 깔리는건 아니었던걸로.. 오류였다.</strike>
  =>08.11 오류가 나면서 알게된 사실인데 내가 쓰는 버전은 directX가 자동으로 깔린다구 한다. 따로 깔 필요가 없는듯,, 집가서 확인하기!
  

### 2021.08.09

<img src="https://user-images.githubusercontent.com/49023736/128650580-85dd1d33-02a5-44bd-a6ce-4b6153cd03c8.jpg" width="30%" height="30%" />
  1장을 읽고있는데 선형대수의 집합체였다.. 5장 렌더링 파이프까지 봤는데 기본적인 개념을 정립하기는 좋았지만 directx12라 계속 봐야할지 고민이다
  
  https://minok-portfolio.tistory.com/2?category=370780 <br>
  구글링하다가 찾게 된 사이트.
  https://hombody.tistory.com/11?category=846211
  2d 참고하기 좋을 것 같다.

### 2021.08.10

삼각형을 그렸다<br>
<img src="https://user-images.githubusercontent.com/49023736/128795674-805808d6-0d63-48b4-a3a2-e75a57280504.png" width="30%" height="30%" />

? : opengl과는 index 버퍼 순서가 반대인가???<br>
A : ㅇㅇ!(https://m.blog.naver.com/zxwnstn/222028734504)

사각형을 그렸당<br>
<img src="https://user-images.githubusercontent.com/49023736/128796130-5be70c61-6347-4977-b38f-f7331168e7b7.png" width="30%" height="30%" />

direct3D 개요. 코드를 짜다보니까 device, deviceContext, 윈도우 핸들 등 반복되는 단어가 계속 나오는데 어렴풋이 알고 이해하려니까 속이 너무 답답.. 잘 모르겠는 단어들 먼저 공부를 하고 코드 이해를 하는게 좋을 것 같다.
https://m.blog.naver.com/masca140/220704230471 

내일 할거
https://m.blog.naver.com/atom723/190311655
https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-dxgi

### 2021.08.11

<b>카메라 & 큐브 점프 및 움직이기</b> <br>
<img src="https://user-images.githubusercontent.com/49023736/128961176-46216515-b1b9-4398-b8f1-2442af54a443.gif" width="30%" height="30%" />

<strike>하루종일 texture만 했는데 알 수 없는 오류가 또 날 괴롭힌다.......
어디서 나는건지 모르겠어서 디버깅해보니까 d3dcompilefromfile 쪽에서 나고있었다. https://gpgstudy.com/forum/viewtopic.php?t=25475 <- 이 사이트에서 참고해보니, 세번째 인자가 비면 error가 발생할 수도 있다고 해서 다른 방식으로도 해보았지만 여전히 안된다....<br>
텍스쳐를 해야 라이팅을 하던 폰트를 하던 할텐데 ㅜㅜ</strike> <br>
텍스쳐 해결!! WCHAR 캐스팅을 잘못하고 있던 모양이다....(WCHAR*)L"hlsl파일명" 이런식으로 해주면 된당! ㅎㅅㅎ<br>

https://www.braynzarsoft.net/viewtutorial/q16390-directx-11-an-introduction-to-the-win32-api
이 예제도 한번 따라해봐야겠다

### 2021.08.12

<b>텍스쳐 입히기</b> <br>
https://user-images.githubusercontent.com/49023736/129129691-ac31f730-9d05-4d87-a171-17c7f9e03583.mp4

어찌저찌 하긴 했는데 솔직히 잘 이해 안된다,,,,,..

<오늘의 실수 포인트>
 - window 먼저 Initialization 하고, direct3d를 Initialization해줘야하는데 순서를 바꿔가지고 한참 걸렸다; ㅜ
 - 과연 내가 원하는 설정으로 바꿔서 사용할 수 있을지 의문이다..

위에 예제 사이트가 훨씬 이해하기 쉬웠다. 코드가 길어지면, 당연히 [0]보다는 가독성은 떨어지겠지만 처음 공부하는 입장에서 보면, 진행 과정을 따라 쭉 나열하고 있기 때문에 순서를 파악하기 좋았다!
그리고 개인적으로 느낀점인데 directX가 어려운 이유는 대충 깔짝이면서 공부하는건 불가능하기 때문이 아닐까..싶다 학교에서 opengl도 안배운 상태였다면 진작에 그만 뒀을것같다.

### 2021.08.13

<img src="https://user-images.githubusercontent.com/49023736/129440608-ece2b778-29bd-4263-a2d5-427a8203bf96.png" width="50%" height="40%" />

info/draw_rectanlge(projectionXX) 에 있는 코드의 흐름을 따라가보면 위와 같다.

-----------------

	VertexType vt;
	D3D11_INPUT_ELEMENT_DESC Playout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(vt.pos), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

** 정점 하나 기준
Playout[n]에 5번째 파라미터를 보면, position이 어디서부터 시작할 것인지를 물어보는 것이다. 현재 position같은 경우 XMFLOAT3을 이용하고 있기 때문에 
4*3, 총 12바이트를 사용한다. 그렇기 때문에 COLOR은 12바이트 뒤에서부터 할당되어야 한다. 
D3D11_APPEND_ALIGNED_ELEMENT<=를 이용해서 자동으로 할당할수도 있다고 한다. 
아니면 저렇게 구조체를 가져와서, 앞에 사용한 것들의 바이트를 직접 계산하는 방식으로도 할 수 있다.

데이터 구조가 하나 늘어날때마다 

<img src="https://user-images.githubusercontent.com/49023736/129440611-39637f64-04e3-4fa4-8b9a-d578043ed7ae.png" width="50%" height="40%" />

----------------------------------------------------------

### 2021.08.14-15

화면 생성부터 물체 이동까지

![image](https://user-images.githubusercontent.com/49023736/129452030-66edead9-e4b4-4057-98ae-7bd79239547f.png)

이걸 한번에 이해하는 천재들이 부럽다..

큐브 점프 & 카메라 이동

![녹화_2021_08_15_11_54_37_642](https://user-images.githubusercontent.com/49023736/129465469-d18966ff-5d33-43b5-8220-4128f94f4d04.gif)


<공부계기>

 2021 여름 방학에 인턴을 할 때, 오픈소스를 가져와 필요한 데이터를 추가한 뒤, 릴리즈모드로 디버깅할 일이 있었다. 당시 일년 전에 한번해봤던 QT와 블루투스, 그리고 윈도우 등 처음 접해보는 헤더들로 인하여 어디서 오류가 발생하는지 알 수 없었고 결국 다른 팀원분의 도움을 받았다. 옆에서 지켜보니, RegisterDeviceNotification라는 windows.h에 있는 함수에서 발생한 오류였다. 그리고 exe파일을 다른 컴퓨터에 깔 때도, 32비트(실행파일)와 64비트(데스크탑)의 충돌로 잘 깔리지 않았고 이것또한 팀원의 도움을 받을 수 밖에 없었다. <br>
 이 두번의 도움으로 깨닫게 된 점은 바로 내가 컴퓨터에 대한 이해나 지식이 전혀 없다는 것이었다. 유니티나 언리얼로 코딩을 하고있다고 생각했지만 이는 겉으로만 코딩을 하는 척한다는 생각이 계속 들었다. 그래서 더 로우레벨의 공부를 해야겠다고 생각했다.!<br>
 무엇을 공부해야할지 고민하던 와중에, 문득 2021-1 게임그래픽프로그래밍 수업을 들을 때 교수님께서 옛날에는 리소스매니저도 직접 만들었다는 얘기가 생각났다. 도대체 유니티없이 어떻게 게임을 만들 수 있었는지 궁금해서 찾아보니 지금의 언리얼과 유니티같은 게임엔진들은 directX나 openGL을 기반으로 만들어졌고 게임업계에서는 directX를 더 선호한다고 한다. 지금와서 생각해보니 다 수업시간에 들었던 내용인데, 중요하게 생각하지 않고 넘어갔던것 같다. 쨌든 그래서 directX를 가지고 게임을 만들어야겠다고 생각했다. 인터넷에 찾아보니 지금은 directX로 개발하는 곳은 거의 없고 게임엔진쪽으로 갈거면 공부하라고들 얘기한다. 하지만 나는 더 깊은 이해를 하고싶기 때문에 windows.h와 directX3D를 공부해보고자 했다!<br>
 그리고 마침 동아리에서 C++로 게임만들기 대회(??)를 개최해서 참가해보기로 했당!-!


![image](https://user-images.githubusercontent.com/49023736/129474379-d5a36e77-4b28-4995-bacb-b59f5d116c58.png)

### 2021.08.17
 전반적인 내용을 이해하는데도 오랜 시간이 걸렸는데 세세한 내용까지 공부하려니 머리가 터질것같다.
 
 ### 2021.08.18
 
 셰이더 버퍼와 버텍스 인덱스 버퍼를 혼동하지 말자~!!!
 
 <img src="https://user-images.githubusercontent.com/49023736/129826572-c135e14a-533a-4aeb-8fb1-205e28296c1b.gif" width="50%" height="40%" />
 
RT와 TR의 중요성..ㅋㅋㅋㅋ

![ezgif com-gif-maker (2)](https://user-images.githubusercontent.com/49023736/129855452-91ccc567-d148-48fa-9a54-e2b6c9384f71.gif)
![ezgif com-gif-maker (3)](https://user-images.githubusercontent.com/49023736/129855460-122fc8dc-d110-4f68-9c0c-18746db0a4d5.gif)


와..난 지금까지 rotation이랑 transform 순서만 조심하면 되는줄알았는데 scale도 조심해야한다;;;; 이것때매 한참 해맸네!
Rotation * Scale * Translation ;

