# UE5 TPS Study Project

Unreal Engine 5 C++ 학습용 TPS(Third Person Shooter) 프로젝트.  
근접 전투 시스템과 그리드 기반 인벤토리 시스템을 직접 구현하며 UE5 C++ 개발 방식을 익히는 것을 목표로 합니다.

---

## 기술 스택

| 분류 | 내용 |
|------|------|
| **엔진** | Unreal Engine 5.7 |
| **언어** | C++ |
| **입력** | Enhanced Input System |
| **AI** | StateTree + EQS (Environment Query System) |
| **UI** | UMG (Unreal Motion Graphics) |
| **데이터** | Primary Data Asset |
| **빌드** | Visual Studio 2022 |

---

## 구현 기능

### ⚔️ 전투 시스템
- **콤보 공격** — 몽타주 섹션 기반 멀티 히트 콤보 스트링 (`AnimNotify_CheckCombo`)
- **차지 공격** — 꾹 누르기 감지 및 차지 루프 처리 (`AnimNotify_CheckChargedAttack`)
- **피격/사망/리스폰** — 래그돌 물리, 타이머 기반 리스폰
- **구체 트레이스 판정** — 공격 범위 내 적 탐지 (`AnimNotify_DoAttackTrace`)

### 🤖 AI 시스템
- **StateTree 기반 AI** — UE5 StateTree로 적 행동 제어 (비헤이비어 트리 미사용)
- **EQS** — `EnvQueryContext_Player` / `EnvQueryContext_Danger`로 위치 쿼리
- **적 스포너** — `ACombatActivationVolume` 트리거로 적 스폰

### 🎒 인벤토리 시스템
- **그리드 기반** — 디아블로2 스타일 격자 인벤토리 (AABB 충돌 검사)
- **탭 구조** — 기본 인벤토리 + 가방 획득 시 탭 추가
- **스택** — 1×1 아이템 자동 수량 합산 (MaxStackSize 제한)
- **아이템 회전** — 90도 회전 배치 지원
- **Data Asset** — `UItemDataAsset`으로 아이템 데이터 에디터 관리
- **아이템 타입** — Consumable / Material / Equipment / Bag / Quest

### 🖥️ UI
- I키로 인벤토리 창 토글 (열기/닫기)
- 위젯 표시 시 마우스 커서 활성화 및 Game+UI 입력 모드 전환

### 🕹️ 플레이어
- **아이템 픽업** — F키, SphereComponent overlap 감지
- **Enhanced Input** — 키보드/게임패드 동일 코드 경로 공유

---

## 아키텍처 구조

```
TPSProject (단일 런타임 모듈)
└── Variant_Combat/
    ├── CombatCharacter       기반 캐릭터 (전투 + 인벤토리 + 입력)
    ├── AI/                   StateTree, EQS, 스포너
    ├── Animation/            AnimNotify 클래스들
    ├── Gameplay/             ItemPickup, ActivationVolume 등
    ├── Interfaces/           ICombatAttacker, ICombatDamageable
    ├── Inventory/            UInventoryComponent, FItemData, UItemDataAsset
    └── UI/                   UInventoryWidget, UCombatLifeBar
```

---

## 빌드 방법

```powershell
& "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" TPSProjectEditor Win64 Development "C:\Users\CMAP\Documents\Unreal Projects\IDU\TPSProject\TPSProject.uproject"
```

또는 Visual Studio 2022에서 `TPSProject.sln` 열어 빌드.

> ⚠️ 언리얼 에디터가 열려있으면 빌드 실패합니다. 에디터를 닫고 실행하세요.
