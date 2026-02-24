# LOG:OUT (팀 Sonatia)

게임인재원 6기 졸업 팀 프로젝트

## 📝 프로젝트 소개

LOG:OUT는 게임인재원 6기에서 진행한 졸업작품 팀 프로젝트입니다.  
언리얼 5.5버전으로 2인 협동 멀티플레이 게임입니다.  
장르는 탈출, 공포 게임입니다.

## ✨ 주요 기능 및 기술 스택

- ### **Unreal Engine 5.5**:
  - C++ 중심 설계 (Blueprint 혼합)
  
- ### **Multiplayer**:
  - Listen Server 기반 멀티플레이 구조
  - PlayerController 기반 **Net UI Manager** 구현
  - **TravelManagerSubsystem** 설계 (PreLoadMap, StreamableManager 활용)
    
- ### **AI**:
  - **Behavior Tree + Gameplay Ability System(GAS)** 기반 AI 아키텍처
  - **AI Perception** (Sight / Hearing) 기반 상태 전이 설계
  - **Custom BTTask / BTService** 구현
  - GameplayTag 기반 상태·행동 제어, GameplayCue 연동
  - **NavLinkProxy + SmartLink** 이벤트 기반 동적 장애물 처리
  - Spline 기반 Patrol 시스템
  - AI 전용 AnimBlueprint 설계, GameplayTagPropertyMap 기반 ABP 동기화, 속도 보간(Lerp) 기반 자연스러운 전환 처리
    
- ### **Data & Architecture**:
  - Custom SaveGame 시스템 설계
  - Subsystem 기반 매니저 구조화

## 📁 프로젝트 구조

```
Project/
├── Develop/
├──├──New_ThePhantomTwins/  # 메인 솔루션
├──├──├──Source/New_ThePhantomTwins/
├──├──├──├──AI
├──├──├──├──Player
├──├──├──├──Object
├──├──├──├──...
├──├──The_Phantom_Twins/    # (구버전)메인 솔루션
├── Feature/                
├──├──...
└── Release/
├──├──...
```

## 👥 팀 구성

- **기획**:  3명
- **아트**:  3명
- **프로그래머**: 4명

**총 10명**

## 📌 참고사항

본 프로젝트는 2025 G-START C2C 부스 참가의 목적으로 제작되었습니다. 

---

© 2025 게임인재원 6기 팀 SonaTia
