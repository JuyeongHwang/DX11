## 🎮💻 DX11

#### 목표 : directX11 공부 & 간단한 게임 만들기

#### 사용 버전 :visual studio 2019

- new project > 데스크탑 위자드 


## 개발 일지
### 2021.08.06
  뿅하면 만들어지는 유니티, 언리얼과 다르게 창을 만드는 것부터 아니 프로젝트를 세팅하는 것부터 많은 고난이 있었다...<br>
  특히 윈도우 api를 처음 쓰다보니 LRESULT, wWinMain,hwnd 등 작은 단어 하나하나가 모두 모르는 단어여서 <br>
  코드를 읽어내리는데만 오랜 시간이 걸리고 이해도 잘 안가서 너무 힘들었는데 참고자료[1] 링크 덕분에 전반적으로 이해하기 좋았다.
  
  <오늘 알게 된 점>
    다이렉트x 공부를 위해 이것저것 코드를 읽다보니 wWinMain앞에 붙어 있는 무엇인가가 다 달랐다. 찾아보니 결국 이름만 다르고 같은 것들이었다.
    callback == winapi == apientry

#### 📘📙📒참고자료

[1] https://www.youtube.com/watch?v=NTvhVxSC_80 (DirectX 11 SDK Tutorial코드 설명)
